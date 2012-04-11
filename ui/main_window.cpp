#include "main_window.h"
#include "url_chooser.h"
#include "no_comm.xpm"
#include "../camera_viewer.h"
#include "../image_processing.h"

#include <stdexcept>

const char URL[] = "http://10.6.12.11/mjpg/video.mjpg";

options_widget::options_widget() {
    spinbuttons = new Gtk::SpinButton[6];
    model = Gtk::ListStore::create(record);
    color_mode.set_model(model);
    //fill rows
    for (const auto& str : { "HSL", "HSI", "HSV", "RGB" }) {
        Gtk::TreeModel::Row row = *(model->append());
        row[record.text] = str;
    }
    //color_mode.set_active_text("HSL");
    color_mode.set_active(0);
    color_mode.pack_start(record.text);
#ifndef USE_DEPRECATED_GTKMM_API
    grid.attach(color_mode, 0, 0, 2, 1);
#endif
    for (int i = 0; i < 6; i++) {
        spinbuttons[i].set_range(0, 255);
        spinbuttons[i].set_increments(1, 16);
#ifndef USE_DEPRECATED_GTKMM_API
        grid.attach(spinbuttons[i], i % 2, (i/2)+1, 1, 1);
#endif
    }
#ifdef USE_DEPRECATED_GTKMM_API
    ch1_box.add(spinbuttons[0]);
    ch1_box.add(spinbuttons[1]);
    ch2_box.add(spinbuttons[2]);
    ch2_box.add(spinbuttons[3]);
    ch3_box.add(spinbuttons[4]);
    ch3_box.add(spinbuttons[5]);
    box.add(color_mode);
    box.add(ch1_box);
    box.add(ch2_box);
    box.add(ch3_box);
#endif
}

options_widget::~options_widget() {
    delete[] spinbuttons;
}

COLOR_SPACE options_widget::color_space() {
    Gtk::TreeModel::Row r = *(color_mode.get_active());
    Glib::ustring s = r[record.text];
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
#ifndef USE_DEPRECATED_GTKMM_API
    return grid;
#else
    return box;
#endif
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
    view->reconnect(url.c_str(), delim.c_str());
    image.set(no_comm_image);
    processed_image.set(no_comm_image);
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
    setip.signal_activate().connect(sigc::mem_fun(*this, &main_window::set_ip));
    //setup menu bar
    menubar.prepend(configitem);
    configitem.set_label("Config");
    configitem.set_submenu(configmenu);
    configmenu.prepend(setip);
    setip.set_label("Set Camera IP");
    //set border width
    set_border_width(10);
    //setup images
#ifndef USE_DEPRECATED_GTKMM_API
    img_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
#endif
    img_box.add(image);
    img_box.add(processed_image);
    img_frame.add(img_box);
    //setup options
    opt_frame.add(opts.widget());
    //setup main grid
#ifndef USE_DEPRECATED_GTKMM_API
    main_window_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
#endif
    main_window_box.add(img_frame);
    main_window_box.add(opt_frame);
    //setup main grid
#ifndef USE_DEPRECATED_GTKMM_API
    main_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
#endif
    main_box.add(menubar);
    main_box.add(main_window_box);
    add(main_box);
    show_all();
    //connect signals
    view->signal_lost_comm().connect(sigc::mem_fun(*this, &main_window::lost_comm));
    view->signal_new_image().connect(sigc::mem_fun(*this, &main_window::new_image));
    Glib::signal_idle().connect(sigc::mem_fun(*this, &main_window::idle_func));
}

main_window::~main_window() {
    //
}
