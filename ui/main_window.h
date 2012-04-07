#ifndef UI_MAIN_WINDOW_H_INC
#define UI_MAIN_WINDOW_H_INC

#include <gtkmm/window.h>
#include <gtkmm/image.h>
#include <gtkmm/grid.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/action.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/menu.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <thread>

#include "image_transport.h"
#include "../image_processing.h"
#include "../camera_viewer.h"

class options_widget {
    public:
        guint8 ch1_low();
        guint8 ch1_high();
        guint8 ch2_low();
        guint8 ch2_high();
        guint8 ch3_low();
        guint8 ch3_high();
        COLOR_SPACE color_space();
        Gtk::Widget& widget();
        options_widget();
        ~options_widget();
    protected:
        Gtk::Grid grid;
        Gtk::ComboBoxText color_mode;
        Gtk::SpinButton * spinbuttons;
    };

class main_window : public Gtk::Window {
public:
    main_window();
    ~main_window();
protected:
    Glib::RefPtr<Gdk::Pixbuf> picture;
    Glib::RefPtr<Gdk::Pixbuf> processed_picture;
    Gtk::Image image;
    Gtk::Image processed_image;
    Gtk::Grid img_grid;
    Gtk::Frame img_frame;
    Gtk::Frame opt_frame;
    Gtk::Grid main_window_grid;
    Gtk::Grid main_grid;
    //menu bar
    Gtk::MenuBar menubar;
    Gtk::MenuItem configitem;
    Gtk::Menu configmenu;
    Gtk::MenuItem setip;
    options_widget opts;
    void lost_comm();
    void new_image(const Glib::RefPtr<const Gdk::Pixbuf>&);
    //actions
    Glib::RefPtr<Gtk::Action> ip_config_action;
    void set_ip();
private:
    std::unique_ptr<camera_viewer> view;
    Glib::RefPtr<Gdk::Pixbuf> no_comm_image;
    bool idle_func();
    //url bits
    std::string http;
    std::string ip;
    std::string path;
    std::string delim;
};
    
    
#endif
