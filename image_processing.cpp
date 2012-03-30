#include "image_processing.h"

#include <algorithm>
#include <array>
#include <cassert>

guint8 get_hue(color rgb) {
    long hue;
    long rgb_max = std::max(std::max(rgb[0], rgb[1]), rgb[2]);
    long rgb_min = std::min(std::min(rgb[0], rgb[1]), rgb[2]);
    long div = rgb_max - rgb_min;
    if (rgb_max == rgb[0]) {
        hue = 0 + (43*(rgb[1] - rgb[2]))/div;
        if (hue < 0) {
            hue += 255;
        }
    }
    else if (rgb_max == rgb[1]) {
        hue = 85 + (43*(rgb[2] - rgb[0]))/div;
    }
    else /* rgb_max == rgb.b */ {
        hue = 171 + (43*(rgb[0] - rgb[1]))/div;
    }
    return (guint8) hue;
}
//these color functions need to be tested!
color RGB2HSV(color rgb) {
    long saturation;
    long value;
    //foldl?
    long rgb_max = std::max(std::max(rgb[0], rgb[1]), rgb[2]);
    long rgb_min = std::min(std::min(rgb[0], rgb[1]), rgb[2]);
    value = rgb_max;
    long chroma = rgb_max - rgb_min;
    if (!value || !chroma) {
        return make_color(0, 0, (guint8)value);
    }
    saturation = (255 * chroma) / value;
    return make_color(get_hue(rgb), (guint8)saturation, (guint8)value);
}

color RGB2HSL(color rgb) {
    long saturation;
    long lightness;
    //foldl?
    long rgb_max = std::max(std::max(rgb[0], rgb[1]), rgb[2]);
    long rgb_min = std::min(std::min(rgb[0], rgb[1]), rgb[2]);
    lightness = (rgb_min + rgb_max) / 2;
    long chroma = rgb_max - rgb_min;
    if (!chroma) {
        return make_color(0, 0, (guint8)lightness);
    }
    long div = 2 * lightness;
    div -= 255;
    if (div > 0) {
        div = -div;
    }
    saturation = chroma / (255 - div);
    return make_color(get_hue(rgb), (guint8)saturation, (guint8)lightness);
}

color RGB2HSI(color rgb) {
    long saturation;
    long intensity;
    //foldl?
    long rgb_max = std::max(std::max(rgb[0], rgb[1]), rgb[2]);
    long rgb_min = std::min(std::min(rgb[0], rgb[1]), rgb[2]);
    intensity = rgb[0] + rgb[1] + rgb[2];
    intensity /= 3;
    long chroma = rgb_max - rgb_min;
    if (!chroma) {
        return make_color(0, 0, (guint8)intensity);
    }
    saturation = 255 - ((255 * rgb_min)/intensity);
    return make_color(get_hue(rgb), (guint8)saturation, (guint8)intensity);
}

void set_color(guint8 * pixel, color c) {
    pixel[0] = c[0];
    pixel[1] = c[1];
    pixel[2] = c[2];
}

bool is_in_range(guint8 * pixel, color l, color h, COLOR_SPACE space) {
    color c = make_color(pixel[0], pixel[1], pixel[2]);
    switch (space) {
    case COLOR_SPACE::HSL:
        c = RGB2HSL(c);
        break;
    case COLOR_SPACE::HSI:
        c = RGB2HSI(c);
        break;
    case COLOR_SPACE::HSV:
        c = RGB2HSV(c);
        break;
    case COLOR_SPACE::RGB:
        //we're good
        break;
    default:
        //invalid - throw exception?
        return false;
    };
    return
    (
        ( //hue
            ((l[0] < h[0]) && ( //low < high
                (c[0] >= l[0]) && (c[0] <= h[0])
            ))
            ||
            ((l[0] >= h[0]) && ( //low > high
                (c[0] >= l[0]) || (c[0] <= h[0])
            ))
        )
        &&
        ( //saturation
            (c[1] >= l[1]) && (c[1] <= h[1])
        )
        &&
        ( //value
            (c[2] >= l[2]) && (c[2] <= h[2])
        )
    );
}

void threshold_image(
    Glib::RefPtr<Gdk::Pixbuf>& img,
    color low,
    color high,
    color replace_false,
    color replace_true,
    COLOR_SPACE space)
{
    Gdk::Colorspace cspace = img->get_colorspace();
    guint8 * pixels = img->get_pixels();
    int bits_per_sample = img->get_bits_per_sample();
    int n_channels = img->get_n_channels();
    bool has_alpha = img->get_has_alpha();
    int width = img->get_width();
    int height = img->get_height();
    int rowstride = img->get_rowstride();
    
    assert(bits_per_sample == 8);
    assert(cspace == Gdk::COLORSPACE_RGB);
    //the above should be true for any image...
    //right now 24/32 bit RGB/RGBA is the only supported color space
    
    //todo: make this more pretty! wrapper?
    guint8 * row = pixels;
    for (int i = 0; i < height; i++) {
        guint8 * p = row;
        for (int j = 0; j < width; j++) {
            if (is_in_range(p, low, high, space)) {
                set_color(p, replace_true);
            }
            else {
                set_color(p, replace_false);
            }
            p += n_channels; //should be n_channels * bits_per_sample/8
        }
        row += rowstride;
    }
}

void process_image(Glib::RefPtr<Gdk::Pixbuf>& img) {
    color replace_false = make_color(0, 0, 0);
    color replace_true = make_color(255, 0, 0);
    color low = make_color(0, 0, 128);
    color high = make_color(255, 255, 255);
    threshold_image(img, low, high, replace_false, replace_true, COLOR_SPACE::HSL);
}

