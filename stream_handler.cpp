#include "stream_handler.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

size_t stream_handler::write(void * ptr, size_t size, size_t nmemb, void * o) {
	size_t numbytes = size * nmemb;
	return (((stream_handler*)o)->obj_write(ptr, numbytes));
}

size_t stream_handler::obj_write(void * ptr, size_t numbytes) {
    if (numbytes) {
        received_data.emit(numbytes);
    }
    return add_data((const char*)ptr, numbytes);
}

sigc::signal<void, size_t>& stream_handler::signal_received_data() {
    return received_data;
}

mjpeg_stream_handler::mjpeg_stream_handler(image_transport& t, const std::string& s) :
    stream_handler(t), //superclass - add transport
    delim(s), //delimiter between frames
    buf(0x4000) //reserve data for buffer
{
    state = SEARCH; //start out looking for delim
}

void mjpeg_stream_handler::reset() {
    state = SEARCH;
    buf.clear();
}

void mjpeg_stream_handler::reset(const std::string& newdelim) {
    delim = newdelim;
    reset();
}

void mjpeg_stream_handler::process_data() {
    if (state == SEARCH) {
        //looking for delim - std::end(delim)-1 so we don't include null terminator
        auto i = std::search(buf.begin(), buf.end(), delim.begin(), delim.end());
        if (i != buf.end()) {
            buf.erase(buf.begin(), i + (sizeof(delim)/sizeof(delim[0])));
            state = CONTENT_TYPE;
        }
    }
    if (state == CONTENT_TYPE) {
        //beginning of the vector is the header
        
        //header is of structure:
        //Content-Type: image/jpeg\r\n
        //Content-Length: X\r\n\r\n
        
        //get Content-Type:
        //TODO: Assert buf begins with Content-Type
        auto content_type_it = std::find(buf.begin(), buf.end(), ' ');
        auto content_type_end = std::find(content_type_it, buf.end(), '\n');
        if (content_type_end == buf.end()) {
            return; //try again later
        }
        //minus 1 below so that the string ends on the \r
        content_type = std::string(++content_type_it, content_type_end - 1);
        if (content_type.empty()) {
            //malformed header. Try again
            state = SEARCH;
            return process_data();
        }
        //TODO:  Assert image is image/jpeg
        buf.erase(buf.begin(), ++content_type_end); //include end
        state = CONTENT_LENGTH;
    }
    if (state == CONTENT_LENGTH) {
        //beginning of vector should be Content-Length
        auto content_length_it = std::find(buf.begin(), buf.end(), ' ');
        auto content_length_end = std::find(content_length_it, buf.end(), '\n');
        if (content_length_end == buf.end()) {
            return; //try again later
        }
        //find second \r\n
        auto header_end = std::find(content_length_end + 1, buf.end(), '\n');
        if (header_end == buf.end()) {
            return; //try again later
        }
        std::string str(++content_length_it, content_length_end - 1);
        if (str.empty()) {
            //malformed header.  Try again
            state = SEARCH;
            return process_data();
        }
        std::stringstream sstream(std::move(str), std::stringstream::in);
        sstream >> content_length;
        buf.erase(buf.begin(), ++header_end); //start at end of data
        state = READ_DATA;
    }
    if (state == READ_DATA) {
        //read in data
        if (buf.size() >= content_length) {
            transport().add_data((const char*)&(buf[0]), content_length);
            state = SEARCH;
            buf.erase(buf.begin(), buf.begin() + content_length);
            content_length = 0;
            transport().end_frame();
            process_data();
        }
        else {
            content_length -= buf.size();
            transport().add_data((const char*)&(buf[0]), buf.size());
            buf.clear();
        }
    }
}

size_t mjpeg_stream_handler::add_data(const char * data, size_t len) {
	if (state == READ_DATA) {
        if (len >= content_length) {
            transport().add_data(data, content_length);
            auto ret = content_length;
            data += content_length;
            len -= content_length;
            content_length = 0;
            transport().end_frame();
            state = SEARCH;
            return (ret + add_data(data, len));
        }
        else {
            transport().add_data(data, len);
            content_length -= len;
        }
    }
    else {
        buf.insert(buf.end(), data, data + len);
        process_data();
    }
    return len;
}
