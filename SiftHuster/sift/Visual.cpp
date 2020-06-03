#include "sift/Visual.h"

using namespace std;
using namespace cv;

extern Keypoint keypoints;


Visual::Visual()
{

}


CvMat* Visual::MosaicHorizen(CvMat* im1, CvMat* im2)
{
    int row, col;
    CvMat *mosaic = cvCreateMat(max(im1->rows, im2->rows), (im1->cols + im2->cols), CV_32FC1);
#define Mosaic(ROW,COL) ((float*)(mosaic->data.fl + mosaic->step/sizeof(float)*(ROW)))[(COL)]
#define Im11Mat(ROW,COL) ((float *)(im1->data.fl + im1->step/sizeof(float) *(ROW)))[(COL)]
#define Im22Mat(ROW,COL) ((float *)(im2->data.fl + im2->step/sizeof(float) *(ROW)))[(COL)]
    cvZero(mosaic);
    /* Copy images into mosaic1. */
    for (row = 0; row < im1->rows; row++)
        for (col = 0; col < im1->cols; col++)
            Mosaic(row, col) = Im11Mat(row, col);
    for (row = 0; row < im2->rows; row++)
        for (col = 0; col < im2->cols; col++)
            Mosaic(row, (col + im1->cols)) = Im22Mat(row, col);
    return mosaic;
}

CvMat* Visual::MosaicVertical(CvMat* im1, CvMat* im2)
{
    int row, col;
    CvMat *mosaic = cvCreateMat(im1->rows + im2->rows, max(im1->cols, im2->cols), CV_32FC1);
#define Mosaic(ROW,COL) ((float*)(mosaic->data.fl + mosaic->step/sizeof(float)*(ROW)))[(COL)]
#define Im11Mat(ROW,COL) ((float *)(im1->data.fl + im1->step/sizeof(float) *(ROW)))[(COL)]
#define Im22Mat(ROW,COL) ((float *)(im2->data.fl + im2->step/sizeof(float) *(ROW)))[(COL)]
    cvZero(mosaic);

    /* Copy images into mosaic1. */
    for (row = 0; row < im1->rows; row++)
        for (col = 0; col < im1->cols; col++)
            Mosaic(row, col) = Im11Mat(row, col);
    for (row = 0; row < im2->rows; row++)
        for (col = 0; col < im2->cols; col++)
            Mosaic((row + im1->rows), col) = Im22Mat(row, col);

    return mosaic;
}

void Visual::DisplayKeypointLocation(IplImage* image, ImageOctaves *GaussianPyr)
{

    Keypoint p = keypoints;
    while (p)
    {
        cvLine(image, cvPoint((int)((p->col) - 3), (int)(p->row)),
               cvPoint((int)((p->col) + 3), (int)(p->row)), CV_RGB(255, 255, 0),
               1, 8, 0);
        cvLine(image, cvPoint((int)(p->col), (int)((p->row) - 3)),
               cvPoint((int)(p->col), (int)((p->row) + 3)), CV_RGB(255, 255, 0),
               1, 8, 0);
        //  cvCircle(image,cvPoint((uchar)(p->col),(uchar)(p->row)),
        //   (int)((GaussianPyr[p->octave].Octave)[p->level].absolute_sigma),
        //   CV_RGB(255,0,0),1,8,0);
        p = p->next;
    }
}

