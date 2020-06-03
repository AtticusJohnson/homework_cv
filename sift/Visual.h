#ifndef VISUAL_H
#define VISUAL_H
#include<iostream>
#include<typeinfo>
#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include<opencv/highgui.h>
#include "SiftProcess.h"
#include "ImageProcess.h"

class Visual
{
public:
    Visual();
    /*结果显示*/
    CvMat* MosaicHorizen(CvMat* im1, CvMat* im2);
    CvMat* MosaicVertical(CvMat* im1, CvMat* im2);
    void DisplayKeypointLocation(IplImage* image, ImageOctaves *GaussianPyr);

};

#endif // VISUAL_H
