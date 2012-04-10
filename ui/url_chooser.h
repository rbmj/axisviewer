#ifndef UI_URL_CHOOSER_H_INC
#define UI_URL_CHOOSER_H_INC

#include <config.h>

#include <gtkmm/dialog.h>
#include <gtkmm/expander.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>

#ifndef USE_DEPRECATED_GTKMM_API
#include <gtkmm/grid.h>
#else
#include <gtkmm/box.h>
#endif

#include <string>

#include "../camera_viewer.h"

class url_chooser : public Gtk::Dialog {
public:
    url_chooser(std::string&, std::string&, std::string&);
protected:
    bool url_changed(GdkEventFocus*);
    bool ip_changed(GdkEventFocus*);
    bool delim_changed(GdkEventFocus*);
    Gtk::Entry ip;
    Gtk::Label ip_l;
    Gtk::Entry url;
    Gtk::Label url_l;
    Gtk::Entry delim;
    Gtk::Label delim_l;
    Gtk::Expander advanced_opts;
#ifndef USE_DEPRECATED_GTKMM_API
    Gtk::Grid advanced_grid;
    Gtk::Grid content;
#else
    Gtk::VBox content;
    Gtk::HBox ip_box;
    Gtk::HBox url_box;
    Gtk::HBox delim_box;
    Gtk::VBox advanced_box;
#endif
    std::string& delim_s;
    std::string& ip_s;
    std::string& url_s;
};

#endif
