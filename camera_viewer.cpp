#include "camera_viewer.h"
#include "stream_handler.h"
#include "ui/main_window.h"

#include <curl/curl.h>

#include <memory>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <vector>

const std::chrono::milliseconds camera_viewer::timeout(1000); //timeout 1sec

camera_viewer::camera_viewer(const std::string& url, const std::string& delim) :
    stream_handle(trans, delim)
{
    has_comm = false;
    m_url = url;
    m_delim = delim;
    init_socket();
	//create multi object for nonblocking input/output
	set = std::unique_ptr<CURL, CURLM_deleter>(curl_multi_init());
	if (!set) {
		throw std::runtime_error("curl_multi_init() failed");
	}
	curl_multi_add_handle(set.get(), curl_handle.get());
	remaining = 1;
    stream_handle.signal_received_data().connect(sigc::mem_fun(*this, &camera_viewer::received_data_sink));
}

void camera_viewer::init_socket() {
	curl_handle = std::unique_ptr<CURL, CURL_deleter>(curl_easy_init());
	curl_easy_setopt(curl_handle.get(), CURLOPT_URL, m_url.c_str());
	//full debug
	curl_easy_setopt(curl_handle.get(), CURLOPT_VERBOSE, 0L);
	//disable progress
	curl_easy_setopt(curl_handle.get(), CURLOPT_NOPROGRESS, 1L);
	//set callback
	curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEFUNCTION, stream_handler::write);
	curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEDATA, (void*)&stream_handle);
}

void camera_viewer::reconnect(const char * url, const char * delim) {
    if (url) {
        m_url = url;
    }
    if (delim) {
        m_delim = delim;
    }
    remaining = 1;
    curl_multi_remove_handle(set.get(), curl_handle.get());
    init_socket();
    curl_multi_add_handle(set.get(), curl_handle.get());
    trans.end_frame();
    stream_handle.reset(m_delim);
    has_comm = false;
}
    

camera_viewer::~camera_viewer() {
	curl_multi_remove_handle(set.get(), curl_handle.get());
}

void camera_viewer::receive() {
	if (remaining && (!has_comm || (std::chrono::system_clock::now() < deadline))) {
		curl_multi_perform(set.get(), &remaining);
	}
    else {
        trans.lost_comm();
        has_comm = false;
    }
}

void camera_viewer::received_data_sink(size_t bytes) {
    deadline = std::chrono::system_clock::now() + timeout;
    has_comm = true;
}

jpeg2pixbuf::signal_lost_comm_t& camera_viewer::signal_lost_comm() {
    return trans.signal_lost_comm();
}

jpeg2pixbuf::signal_new_image_t& camera_viewer::signal_new_image() {
    return trans.signal_new_image();
}
