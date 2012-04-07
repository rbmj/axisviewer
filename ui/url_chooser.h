#ifndef UI_URL_CHOOSER_H_INC
#define UI_URL_CHOOSER_H_INC

#include <gtkmm/dialog.h>
#include <gtkmm/grid.h>
#include <gtkmm/expander.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>

#include <string>

#include "../camera_viewer.h"

class url_chooser : public Gtk::Dialog {
public:
    url_chooser(std::string&, std::string&, std::string&);
protected:
    bool url_changed(GdkEventFocus*);
    bool ip_changed(GdkEventFocus*);
    bool delim_changed(GdkEventFocus*);
    Gtk::Grid content;
    Gtk::Entry ip;
    Gtk::Label ip_l;
    Gtk::Entry url;
    Gtk::Label url_l;
    Gtk::Entry delim;
    Gtk::Label delim_l;
    Gtk::Expander advanced_opts;
    Gtk::Grid advanced_grid;
    std::string& delim_s;
    std::string& ip_s;
    std::string& url_s;
};

#endif
