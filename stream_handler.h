#ifndef STREAM_HANDLER_H_INC
#define STREAM_HANDLER_H_INC

#include <vector>
#include <string>
#include <functional>

#include "ui/image_transport.h"

//#include "memory_buffer.h"

class stream_handler {
private:
    image_transport& trans;
protected:
    image_transport& transport() { return trans; }
public:
    virtual size_t add_data(const char*, size_t) = 0; //need to copy buffer
    static size_t write(void*, size_t, size_t, void*);
    stream_handler(image_transport& t) : trans(t) {}
    void reset();
};

class mjpeg_stream_handler : public stream_handler {
public:
    mjpeg_stream_handler(image_transport&, const std::string&);
	size_t add_data(const char*, size_t); //const as we need to copy the buffer
    void process_data();
    void reset();
    void reset(const std::string&);
private:
    std::string delim;
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

#endif
