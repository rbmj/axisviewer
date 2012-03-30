#ifndef UI_MAIN_WINDOW_H_INC
#define UI_MAIN_WINDOW_H_INC

#include <gtkmm/window.h>
#include <gtkmm/image.h>
#include <gtkmm/grid.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <thread>

#include "image_transport.h"

class main_window : public Gtk::Window {
public:
    main_window();
    ~main_window();
    bool have_comm();
    bool idle_func();
protected:
    Glib::RefPtr<Gdk::Pixbuf> picture;
    Glib::RefPtr<Gdk::Pixbuf> processed_picture;
    Gtk::Image image;
    Gtk::Image processed_image;
    Gtk::Grid img_grid;
private:
    image_transport trans;
    std::thread transport_thread;
};
    
    
#endif
