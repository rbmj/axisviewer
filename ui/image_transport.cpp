#include "image_transport.h"

#include <iostream>

jpeg2pixbuf::jpeg2pixbuf() {
    loader = Gdk::PixbufLoader::create();
}

jpeg2pixbuf::~jpeg2pixbuf() {
    try {
        loader->close();
    }
    catch (...) {
        //
    }
}

void jpeg2pixbuf::add_data(const char * buf, size_t len) {
    try {
        loader->write((const guint8*)buf, len);
    }
    catch (Gdk::PixbufError & e) {
        auto c = e.code();
        if (c == Gdk::PixbufError::CORRUPT_IMAGE) {
            std::cout << "Corrupt Image" << std::endl;
        }
        else if (c == Gdk::PixbufError::INSUFFICIENT_MEMORY) {
            std::cout << "Insufficient Memory" << std::endl;
        }
        else if (c == Gdk::PixbufError::BAD_OPTION) {
            std::cout << "Bad Option" << std::endl;
        }
        else if (c == Gdk::PixbufError::UNKNOWN_TYPE) {
            std::cout << "Unknown Type" << std::endl;
        }
        else if (c == Gdk::PixbufError::UNSUPPORTED_OPERATION) {
            std::cout << "Unsupported Operation" << std::endl;
        }
        else {
            std::cout << "Generic Pixbuf Error" << std::endl;
        }
    }
}

void jpeg2pixbuf::end_frame() {
    try {
        loader->close();
        m_signal_new_image.emit(loader->get_pixbuf());
    }
    catch (...) {
        //do nothing - what can we do?
    }
    loader = Gdk::PixbufLoader::create();
}

void jpeg2pixbuf::lost_comm() {
    try {
        loader->close();
    }
    catch (...) {
        //do nothing - we're probably invalid anyways.
    }
    loader = Gdk::PixbufLoader::create();
    m_signal_lost_comm.emit();
}

jpeg2pixbuf::signal_lost_comm_t& jpeg2pixbuf::signal_lost_comm() {
    return m_signal_lost_comm;
}

jpeg2pixbuf::signal_new_image_t& jpeg2pixbuf::signal_new_image() {
    return m_signal_new_image;
}
