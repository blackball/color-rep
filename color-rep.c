#include "color-palette.h"
#include "color-rep.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

static void
bgr2lab(unsigned char bgr[3], double lab[3]) {
        double r = bgr[2] / 255.0;
        double g = bgr[1] / 255.0;
        double b = bgr[0] / 255.0;

        r = r > 0.04045 ? pow(((r + 0.055) / 1.055), 2.4) : r / 12.92;
        g = g > 0.04045 ? pow(((g + 0.055) / 1.055), 2.4) : g / 12.92;
        b = b > 0.04045 ? pow(((b + 0.055) / 1.055), 2.4) : b / 12.92;
        
        double x, y, z;
        x = (r * 0.4124 + g * 0.3576 + b * 0.1805) / 0.95047;
        y = (r * 0.2126 + g * 0.7152 + b * 0.0722); /* / 1.000 */
        z = (r * 0.0193 + g * 0.1192 + b * 0.9505) / 1.08883;

        x = x > 0.008856 ? pow(x, (.333333333333)) : (7.787 * x) + (16.0 / 116.0);
        y = y > 0.008856 ? pow(y, (.333333333333)) : (7.787 * y) + (16.0 / 116.0);
        z = z > 0.008856 ? pow(z, (.333333333333)) : (7.787 * z) + (16.0 / 116.0);
        
        lab[0] = (116.0 * y) - 16.0;
        lab[1] = 500.0 * (x - y);
        lab[2] = 200.0 * (y - z);
}

#define distance(a, b)                                                  \
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

