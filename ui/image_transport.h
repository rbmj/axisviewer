#ifndef IMAGE_TRANSPORT_H_INC
#define IMAGE_TRANSPORT_H_INC

#include <atomic>    
#include <gdkmm/pixbuf.h>
#include <gdkmm/pixbufloader.h>
#include <glibmm/refptr.h>

class image_transport {
public:
    void lost_comm();
    void close_image();
    void image_write(const guint8*, size_t);
    bool new_image();
    Glib::RefPtr<Gdk::Pixbuf> get_image();
    bool has_comm();
    image_transport();
private:
    bool comm;
    std::atomic_bool isnew;
    Glib::RefPtr<Gdk::PixbufLoader> loader;
    Glib::RefPtr<Gdk::Pixbuf> image;
};

#endif
