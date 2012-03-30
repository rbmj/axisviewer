//#include "camera_viewer.h"
#include "ui/main_window.h"

#include "camera_viewer.h"

#include <gtkmm/main.h>

#include <thread>

int main(int argc, char *argv[]) {
    Gtk::Main kit(argc, argv);
    main_window window;
    Gtk::Main::run(window);
    return 0;
}
    
