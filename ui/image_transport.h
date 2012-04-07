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
    typedef sigc::signal<void> signal_lost_comm_t;
    typedef sigc::signal<void, const Glib::RefPtr<const Gdk::Pixbuf>&> signal_new_image_t;
    void lost_comm();
    void end_frame();
    void add_data(const char*, size_t);
    jpeg2pixbuf();
    ~jpeg2pixbuf();
    signal_lost_comm_t& signal_lost_comm();
    signal_new_image_t& signal_new_image();
private:
    Glib::RefPtr<Gdk::PixbufLoader> loader;
    Glib::RefPtr<Gdk::Pixbuf> image;
    signal_lost_comm_t m_signal_lost_comm;
    signal_new_image_t m_signal_new_image;
};

#endif
