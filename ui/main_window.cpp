#include "main_window.h"
#include "url_chooser.h"
#include "no_comm.xpm"
#include "../camera_viewer.h"
#include "../image_processing.h"

#include <stdexcept>

const char URL[] = "http://10.6.12.11/mjpg/video.mjpg";

options_widget::options_widget() {
    spinbuttons = new Gtk::SpinButton[6];
    color_mode.append("HSL");
    color_mode.append("HSI");
    color_mode.append("HSV");
    color_mode.append("RGB");
    color_mode.set_active_text("HSL");
    grid.attach(color_mode, 0, 0, 2, 1);
    for (int i = 0; i < 6; i++) {
        spinbuttons[i].set_range(0, 255);
        spinbuttons[i].set_increments(1, 16);
        grid.attach(spinbuttons[i], i % 2, (i/2)+1, 1, 1);
    }
    
}

options_widget::~options_widget() {
    delete[] spinbuttons;
}

COLOR_SPACE options_widget::color_space() {
    Glib::ustring s = color_mode.get_active_text();
    COLOR_SPACE ret;
    if (s == "HSL") {
        ret = COLOR_SPACE::HSL;
    }
    else if (s == "HSI") {
        ret = COLOR_SPACE::HSI;
    }
    else if (s == "HSV") {
        ret = COLOR_SPACE::HSV;
    }
    else if (s == "RGB") {
        ret = COLOR_SPACE::RGB;
    }
    else {
        throw std::runtime_error("Invalid value from ComboBoxText");
    }
    return ret;
}

guint8 options_widget::ch1_low() {
    return (guint8)spinbuttons[0].get_value();
}

guint8 options_widget::ch1_high() {
    return (guint8)spinbuttons[1].get_value();
}

guint8 options_widget::ch2_low() {
    return (guint8)spinbuttons[2].get_value();
}

guint8 options_widget::ch2_high() {
    return (guint8)spinbuttons[3].get_value();
}

guint8 options_widget::ch3_low() {
    return (guint8)spinbuttons[4].get_value();
}

guint8 options_widget::ch3_high() {
    return (guint8)spinbuttons[5].get_value();
}

Gtk::Widget& options_widget::widget() {
    return grid;
}

void main_window::new_image(const Glib::RefPtr<const Gdk::Pixbuf>& p) {
    picture = p->copy();
    processed_picture = picture->copy();
    color replace_false = make_color(0, 0, 0);
    color replace_true = make_color(255, 0, 0);
    color low = make_color(opts.ch1_low(), opts.ch2_low(), opts.ch3_low());
    color high = make_color(opts.ch1_high(), opts.ch2_high(), opts.ch3_high());
    threshold_image(processed_picture, low, high, replace_false, replace_true, opts.color_space());
    image.set(picture);
    processed_image.set(processed_picture);
}

void main_window::lost_comm() {
    image.set(no_comm_image);
    processed_image.set(no_comm_image);
    view->reconnect();
}

bool main_window::idle_func() {
    //update mjpeg stream
    view->receive();
    return true;
}

void main_window::set_ip() {
    url_chooser choose(ip, path, delim);
    choose.run();
    std::string url = http + ip + path;
    view->reconnect(url.c_str(), path.c_str());
}

main_window::main_window() :
    img_frame("Images"),
    opt_frame("Processing Options")
{
    //create no comm image
    no_comm_image = Gdk::Pixbuf::create_from_xpm_data(no_comm_xpm);
    //initialize pictures to no_comm_image
    image.set(no_comm_image);
    processed_image.set(no_comm_image);
    //TODO: set url to something sane based on current IP configuration
    http = "http://";
    ip = "10.6.12.11";
    path = "/mjpg/video.mjpg";
    delim = "--myboundary\r\n";
    view = std::unique_ptr<camera_viewer>(new camera_viewer(http + ip + path, delim));
    //setup configuration
    ip_config_action = Gtk::Action::create("Set Camera IP");
    ip_config_action->signal_activate().connect(sigc::mem_fun(*this, &main_window::set_ip));
    setip.set_related_action(ip_config_action);
    //setup menu bar
    menubar.prepend(configitem);
    configitem.set_label("Config");
    configitem.set_submenu(configmenu);
    configmenu.prepend(setip);
    setip.set_label("Set Camera IP");
    //set border width
    set_border_width(10);
    //setup images
    img_grid.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    img_grid.add(image);
    img_grid.add(processed_image);
    img_frame.add(img_grid);
    //setup options
    opt_frame.add(opts.widget());
    //setup main grid
    main_window_grid.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    main_window_grid.add(img_frame);
    main_window_grid.add(opt_frame);
    //setup main grid
    main_grid.set_orientation(Gtk::ORIENTATION_VERTICAL);
    main_grid.add(menubar);
    main_grid.add(main_window_grid);
    add(main_grid);
    show_all();
    //connect signals
    view->signal_lost_comm().connect(sigc::mem_fun(*this, &main_window::lost_comm));
    view->signal_new_image().connect(sigc::mem_fun(*this, &main_window::new_image));
    Glib::signal_idle().connect(sigc::mem_fun(*this, &main_window::idle_func));
}

main_window::~main_window() {
    //
}
