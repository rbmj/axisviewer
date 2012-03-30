#ifndef CAMERA_VIEWER_H_INC
#define CAMERA_VIEWER_H_INC

#include <memory>
#include <curl/curl.h>

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
	camera_viewer(const char*, stream_handler::data_sink, stream_handler::end_notifier);
	~camera_viewer();
	int recieve();
private:
	std::unique_ptr<CURL, CURL_deleter> curl_handle;
	std::unique_ptr<CURL, CURLM_deleter> set;
	stream_handler stream_handle;
	int remaining;
};

class image_transport;
void camera_thread(image_transport&);

#endif
