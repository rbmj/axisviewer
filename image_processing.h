#ifndef IMAGE_PROCESSING_H_INC
#define IMAGE_PROCESSING_H_INC

#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>

#include <array>

enum class COLOR_SPACE {
    HSL,
    HSI,
    HSV,
    RGB
};
    
typedef std::array<guint8, 3> color;

void threshold_image(
    Glib::RefPtr<Gdk::Pixbuf>& img,
    color low,
    color high,
    color replace_false,
    color replace_true,
    COLOR_SPACE space
);

static inline color make_color(guint8 a, guint8 b, guint8 c) {
    color ret;
    ret[0] = a;
    ret[1] = b;
    ret[2] = c;
    return ret;
}

#endif
