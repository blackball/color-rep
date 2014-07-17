#ifndef COLORREP_H
#define COLORREP_H

typedef struct _IplImage IplImage;

int color_rep_topk(const IplImage *bgrimg, unsigned char bgrs[][3], int k);

#endif
