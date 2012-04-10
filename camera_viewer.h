#ifndef CAMERA_VIEWER_H_INC
#define CAMERA_VIEWER_H_INC

#include <memory>
#include <curl/curl.h>
#include <chrono>

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
	camera_viewer(const std::string&, const std::string&);
	~camera_viewer();
    jpeg2pixbuf::signal_lost_comm_t& signal_lost_comm();
    jpeg2pixbuf::signal_new_image_t& signal_new_image();
	void receive();
    void reconnect(const char * = nullptr, const char * = nullptr);
private:
	std::unique_ptr<CURL, CURL_deleter> curl_handle;
	std::unique_ptr<CURL, CURLM_deleter> set;
    jpeg2pixbuf trans;
	mjpeg_stream_handler stream_handle;
	int remaining;
    std::string m_url;
    std::string m_delim;
    std::chrono::system_clock::time_point deadline;
    bool has_comm;
    void received_data_sink(size_t);
    void init_socket();
    static const std::chrono::milliseconds timeout;
};

#endif
