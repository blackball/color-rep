#include "color-palette.h"
#include "color-rep.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

static void
bgr2lab(unsigned char bgr[3], double lab[3]) {
        double red = bgr[2];
        double green = bgr[1];
        double blue = bgr[0];

        red   /= 255;
        green /= 255;
        blue  /= 255;

        if (red > 0.04045) {
                red = pow(((red + 0.055) / 1.055), 2.4);
        }
        else {
                red = red / 12.92;
        }

        if (green > 0.04045) {
                green = pow(((green + 0.055) / 1.055), 2.4);
        }
        else {
                green = green / 12.92;
        }

        if (blue > 0.04045) {
                blue = pow(((blue + 0.055) / 1.055), 2.4);
        }
        else {
                blue = blue / 12.92;
        }

        red *= 100;
        green *= 100;
        blue *= 100;

        double x, y, z;
        x = red * 0.4124 + green * 0.3576 + blue * 0.1805;
        y = red * 0.2126 + green * 0.7152 + blue * 0.0722;
        z = red * 0.0193 + green * 0.1192 + blue * 0.9505;

        x /= 95.047;
        y /= 100;
        z /= 108.883;

        if (x > 0.008856) {
                x = pow(x, (.3333333333));
        }
        else {
                x = (7.787 * x) + (16 / 116);
        }

        if (y > 0.008856) {
                y = pow(y, (.3333333333));
        }
        else {
                y = (7.787 * y) + (16 / 116);
        }

        if (z > 0.008856) {
                z = pow(z, (.3333333333));
        }
        else {
                z = (7.787 * z) + (16 / 116);
        }

        lab[0] = (116 * y) - 16;
        lab[1] = 500 * (x - y);
        lab[2] = 200 * (y - z);
}

#define distance(a, b) \
        ((a[0] - b[0]) * (a[0] - b[0]) +  (a[1] - b[1]) * (a[1] - b[1]) +  (a[2] - b[2]) * (a[2] - b[2]))

/**
 * Find the nearest color by linear scan...A octree could made this fast.
 */
static unsigned int 
nearest_color_index(const double lab[3]) {
        unsigned int pos = 0;
        double mind = 100000000.f; /* big enough */
        for (unsigned int i = 0; i < COLOR_PALETTE_SIZE; ++i) {
                const double d = distance(lab, g_lab_colorpalette[i]);
                if (d < mind) {
                        mind = d;
                        pos = i;
                }
        }
        return pos;
}

static int 
max_pos(const unsigned int *counters, int n) {
        int maxd = 0;
        int pos = -1;
        for (int i = 0; i < n; ++i) {
                if (counters[i] > maxd) {
                        maxd = counters[i];
                        pos = i;
                }
        }
        return pos;
}

/**
 * Search top K, by K times
 * @param 
 * @return 
 */
static int 
get_topk_index(unsigned int *counters, int n, int topk[], int k) {
        int actual_size = 0;
        for (int i = 0; i < k; ++i) {
                const int pos = max_pos(counters, n);
                if (pos == -1) {
                        break;
                } 
                topk[i] = pos;
                counters[pos] = 0;
                ++actual_size;
        }
        return actual_size;
}

int 
color_rep_topk(const IplImage *bgrimg, unsigned char bgrs[][3], int k) {
        if (k < 1 || k > 100) {
                return -1;
        }
        
        unsigned int counters[COLOR_PALETTE_SIZE] = {0};
        double lab[3];
        
        
        for (int i = 0; i < bgrimg->height; ++i) {
                for (int j = 0; j < bgrimg->width; ++j) {
                        bgr2lab((unsigned char *)(bgrimg->imageData + i * bgrimg->widthStep + j * 3), lab);
                        counters[nearest_color_index(lab)]++;
                }
        }
        
        int top100[100] = {0};
        int actual_size = get_topk_index(counters, COLOR_PALETTE_SIZE, top100, k);
        for (int i = 0; i < actual_size; ++i) {
                const int pos = top100[i];
                bgrs[i][0] = g_bgr_colorpalette[pos][0];
                bgrs[i][1] = g_bgr_colorpalette[pos][1];
                bgrs[i][2] = g_bgr_colorpalette[pos][2];
        }
        return actual_size;
}

