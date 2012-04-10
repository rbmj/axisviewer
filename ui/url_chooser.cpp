#include "url_chooser.h"

url_chooser::url_chooser(std::string& i, std::string& u, std::string& d) :
    ip_s(i),
    url_s(u),
    delim_s(d),
    advanced_opts("Advanced Options"),
    ip_l("Camera IP Address: "),
    url_l("MJPEG Stream Path: "),
    delim_l("MJPEG Frame Delimiter: ")
{
    //setup entries
    ip.set_text(ip_s);
    url.set_text(url_s);
    delim.set_text(delim_s);
    //signal handlers
    ip.signal_focus_out_event().connect(sigc::mem_fun(*this, &url_chooser::ip_changed));
    url.signal_focus_out_event().connect(sigc::mem_fun(*this, &url_chooser::url_changed));
    delim.signal_focus_out_event().connect(sigc::mem_fun(*this, &url_chooser::delim_changed));
#ifndef USE_DEPRECATED_GTKMM_API
    //setup frame for advanced options
    //attach url entry
    advanced_grid.attach(url_l, 0, 0, 1, 1);
    advanced_grid.attach(url, 1, 0, 1, 1);
    //attach delim entry
    advanced_grid.attach(delim_l, 0, 1, 1, 1);
    advanced_grid.attach(delim, 1, 1, 1, 1);
    //put grid in expander
    advanced_opts.add(advanced_grid);
    //setup main grid
    //attach ip address entry
    content.attach(ip_l, 0, 0, 1, 1);
    content.attach(ip, 1, 0, 1, 1);
    //attach advanced frame
    content.attach(advanced_opts, 0, 1, 2, 1);
    //add content
    get_content_area()->add(content);
#else
    ip_box.add(ip_l);
    ip_box.add(ip);
    url_box.add(url_l);
    url_box.add(url);
    delim_box.add(delim_l);
    delim_box.add(delim);
    advanced_box.add(url_box);
    advanced_box.add(delim_box);
    advanced_opts.add(advanced_box);
    content.add(ip_box);
    content.add(advanced_opts);
    get_vbox()->add(content);
#endif
    //add button
    add_button("OK", 0);
    show_all_children();
}

bool url_chooser::ip_changed(GdkEventFocus * f) {
    ip_s = ip.get_text();
    return true;
}

bool url_chooser::url_changed(GdkEventFocus * f) {
    url_s = url.get_text();
    return true;
}

bool url_chooser::delim_changed(GdkEventFocus * f) {
    delim_s = delim.get_text();
    return true;
}
