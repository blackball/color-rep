#include "color-rep.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>

static void 
color_representation(const char *imgname) {
        IplImage *img = cvLoadImage(imgname, 1);
        if (!img) {
                return ;
        }

#define COLOR_NUM 8
        unsigned char bgr[COLOR_NUM][3];
        int actual_size = color_rep_topk(img, bgr, COLOR_NUM);
        printf("%d\n", actual_size);
        if (actual_size >= 1)  {
                IplImage *colorimg = cvCreateImage(cvSize(actual_size * 30, 50), 8, 3);
                cvSet(colorimg, cvScalar(255, 255, 255, 0), NULL);
                for (int i = 0; i < actual_size; ++i) {
                        cvSetImageROI(colorimg, cvRect(i*30, 0, 30, 50));
                        cvSet(colorimg, cvScalar(bgr[i][0], bgr[i][1], bgr[i][2], 0), NULL);
                        cvResetImageROI(colorimg);
                }

                cvNamedWindow("image", 1);
                cvNamedWindow("colors", 1);
                cvShowImage("image", img);
                cvShowImage("colors", colorimg);
                cvWaitKey(0);
                cvDestroyWindow("image");
                cvDestroyWindow("colors");
                cvReleaseImage(&colorimg);
        }
        
        cvReleaseImage(&img);
}

int 
main(int argc, char *argv[]) {
        if (argc != 2) {
                return 0;
        }
        color_representation(argv[1]);
        return 0;
}

