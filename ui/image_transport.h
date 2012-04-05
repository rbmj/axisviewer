#ifndef IMAGE_TRANSPORT_H_INC
#define IMAGE_TRANSPORT_H_INC
  
#include <gdkmm/pixbuf.h>
#include <gdkmm/pixbufloader.h>
#include <glibmm/refptr.h>

class image_transport {
public:
    virtual void add_data(const char*, size_t) = 0;
    virtual void end_frame() = 0;
    virtual void lost_comm() = 0;
};

class jpeg2pixbuf : public image_transport {
public:
    void lost_comm();
    void end_frame();
    void add_data(const char*, size_t);
    bool new_image();
    Glib::RefPtr<Gdk::Pixbuf> get_image();
    bool has_comm();
    jpeg2pixbuf();
    ~jpeg2pixbuf();
private:
    bool comm;
    bool isnew;
    Glib::RefPtr<Gdk::PixbufLoader> loader;
    Glib::RefPtr<Gdk::Pixbuf> image;
};

#endif
