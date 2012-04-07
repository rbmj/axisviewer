#ifndef CAMERA_VIEWER_H_INC
#define CAMERA_VIEWER_H_INC

#include <memory>
#include <curl/curl.h>

#include "ui/image_transport.h"
#include "stream_handler.h"

struct CURL_deleter {
	void operator()(CURL * c) {
		curl_easy_cleanup(c);
	}
};

struct CURLM_deleter {
	void operator()(CURLM * c) {
		curl_multi_cleanup(c);
	}
};

class camera_viewer {
public:
	camera_viewer(const char*, const char*);
	~camera_viewer();
    jpeg2pixbuf::signal_lost_comm_t& signal_lost_comm();
    jpeg2pixbuf::signal_new_image_t& signal_new_image();
	int receive();
private:
	std::unique_ptr<CURL, CURL_deleter> curl_handle;
	std::unique_ptr<CURL, CURLM_deleter> set;
    jpeg2pixbuf trans;
	mjpeg_stream_handler stream_handle;
	int remaining;
};

#endif