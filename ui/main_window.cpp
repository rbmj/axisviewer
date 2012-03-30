#include "main_window.h"
#include "../camera_viewer.h"
#include "../image_processing.h"

#include <thread>

bool main_window::idle_func() {
    if (trans.new_image()) {
        picture = trans.get_image(); //smart pointer
        //now we have shared ownership - we will have sole ownership when
        //trans gets a new image
        processed_picture = picture->copy();
        process_image(processed_picture);
        image.set(picture);
        processed_image.set(processed_picture);
    }
    return true;
}

bool main_window::have_comm() {
    return trans.has_comm();
}

main_window::main_window() : transport_thread(camera_thread, std::ref(trans)) {
    //set border width
    set_border_width(10);
    img_grid.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    img_grid.add(image);
    img_grid.add(processed_image);
    add(img_grid);
    show_all();
    Glib::signal_idle().connect(sigc::mem_fun(*this, &main_window::idle_func));
}

main_window::~main_window() {
    //
}
