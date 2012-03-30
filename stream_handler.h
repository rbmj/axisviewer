#ifndef STREAM_HANDLER_H_INC
#define STREAM_HANDLER_H_INC

#include <vector>
#include <string>
#include <functional>

//#include "memory_buffer.h"

class stream_handler {
public:
    typedef std::function<void(const char*, size_t)> data_sink;
    typedef std::function<void()> end_notifier;
    stream_handler(data_sink, end_notifier);
	void add_data(const char*, size_t); //const as we need to copy the buffer
    void process_data();
	static size_t write(void*, size_t, size_t, void*);
private:
    static const char delim[];
    data_sink sink;
    end_notifier notifier;
    std::vector<char> buf;
    std::string content_type;
    size_t content_length;
    enum state_t {
        SEARCH,
        CONTENT_TYPE,
        CONTENT_LENGTH,
        READ_DATA
    };
    state_t state;
};

void stream_callback(std::vector<char> v);

#endif
