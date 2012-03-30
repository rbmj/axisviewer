#include "stream_handler.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

void stream_callback(std::vector<char> v) {
    //v is raw jpeg image data
    static int i = 0;
    std::ostringstream fname;
    fname << i++ << ".jpg";
    std::ofstream o(fname.str().c_str(), std::fstream::binary);
    o.write(&(v[0]), v.size()*sizeof(v[0]));
    o.close();
}

const char stream_handler::delim[] = "--myboundary\r\n";

stream_handler::stream_handler(data_sink s, end_notifier n) : buf(0x4000) {
    sink = s;
    notifier = n;
    state = SEARCH;
}

void stream_handler::process_data() {
    if (state == SEARCH) {
        //looking for delim - std::end(delim)-1 so we don't include null terminator
        auto i = std::search(buf.begin(), buf.end(), std::begin(delim), std::end(delim)-1);
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
            sink((const char*)&(buf[0]), content_length);
            state = SEARCH;
            buf.erase(buf.begin(), buf.begin() + content_length);
            content_length = 0;
            notifier();
            process_data();
        }
        else {
            content_length -= buf.size();
            sink((const char*)&(buf[0]), buf.size());
            buf.clear();
        }
    }
}

void stream_handler::add_data(const char * data, size_t len) {
	if (state == READ_DATA) {
        if (len >= content_length) {
            sink(data, content_length);
            data += content_length;
            len -= content_length;
            content_length = 0;
            notifier();
            state = SEARCH;
            return add_data(data, len);
        }
        else {
            sink(data, len);
            content_length -= len;
        }
    }
    else {
        buf.insert(buf.end(), data, data + len);
        process_data();
    }
}

size_t stream_handler::write(void * ptr, size_t size, size_t nmemb, void * handle) {
	size_t numbytes = size * nmemb;
	((stream_handler*)handle)->add_data((const char*)ptr, numbytes);
	return numbytes;
}
