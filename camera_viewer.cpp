#include "camera_viewer.h"
#include "stream_handler.h"
#include "ui/main_window.h"

#include <curl/curl.h>

#include <memory>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <vector>

const char URL[] = "http://10.6.12.11/mjpg/video.mjpg";

camera_viewer::camera_viewer(const char * url, stream_handler::data_sink s,
    stream_handler::end_notifier n) : stream_handle(s, n)
{
	curl_handle = std::unique_ptr<CURL, CURL_deleter>(curl_easy_init());
	curl_easy_setopt(curl_handle.get(), CURLOPT_URL, url);
	//full debug
	curl_easy_setopt(curl_handle.get(), CURLOPT_VERBOSE, 0L);
	//disable progress
	curl_easy_setopt(curl_handle.get(), CURLOPT_NOPROGRESS, 1L);
	//set callback
	curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEFUNCTION, stream_handler::write);
	curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEDATA, (void*)&stream_handle);
	//create multi object for nonblocking input/output
	set = std::unique_ptr<CURL, CURLM_deleter>(curl_multi_init());
	if (!set) {
		throw std::runtime_error("curl_multi_init() failed");
	}
	curl_multi_add_handle(set.get(), curl_handle.get());
	remaining = 1;
}

camera_viewer::~camera_viewer() {
	curl_multi_remove_handle(set.get(), curl_handle.get());
}

int camera_viewer::recieve() {
	if (remaining > 0) {
		curl_multi_perform(set.get(), &remaining);
	}
	return remaining;
}

void camera_thread(image_transport& trans) {
    while (true) { //constantly loop trying to reestablish connection
        camera_viewer view(URL, [&trans](const char * c, size_t s) {
            trans.image_write((const guint8*)c, s);
        },
        [&trans]() {
            trans.close_image();
        } );
        while (view.recieve()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        trans.lost_comm();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}
