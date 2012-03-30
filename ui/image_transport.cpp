#include "image_transport.h"

#include <iostream>

image_transport::image_transport() {
    comm = false;
    isnew = false;
    loader = Gdk::PixbufLoader::create();
}

void image_transport::image_write(const guint8 * buf, size_t len) {
    comm = true;
    try {
        loader->write(buf, len);
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

void image_transport::close_image() {
    comm = true;
    try {
        loader->close();
        image = loader->get_pixbuf();
        isnew = true;
    }
    catch (...) {
        //do nothing - what can we do?
    }
    loader = Gdk::PixbufLoader::create();
}

void image_transport::lost_comm() {
    if (comm) {
        try {
            loader->close();
        }
        catch (...) {
            //do nothing - we're probably invalid anyways.
        }
        loader = Gdk::PixbufLoader::create();
        comm = false;
    }
}

bool image_transport::new_image() {
    if (isnew) {
        isnew = false;
        return true;
    }
    return false;
}

bool image_transport::has_comm() {
    return comm;
}

Glib::RefPtr<Gdk::Pixbuf> image_transport::get_image() {
    return image;
}
