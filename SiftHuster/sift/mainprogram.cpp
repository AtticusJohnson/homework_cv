#include "sift/MainProgram.h"
#include <stdio.h>


using namespace std;
using namespace cv;

//const char* path = "C:/Users/ZW/Desktop/111.jpg";

SiftProcess sift;
Visual show;
extern ImageProcess image_process;
int     numoctaves;
int     numoctaves2;
CvMemStorage* storage; // temporary storage
//DOG pyr，DOG算子计算简单，是尺度归一化的LoG算子的近似。
extern ImageOctaves *DOGoctaves;
extern ImageOctaves *mag_thresh;
extern ImageOctaves *mag_pyr;
extern ImageOctaves *grad_pyr;
extern Keypoint keypoints;
extern Keypoint keyDescriptors;

int program(const char* path)
{

    cout << "start run ..." << endl;
    IplImage* src = nullptr;
    IplImage* image1 = nullptr;
    IplImage* grey_im1 = nullptr;
    IplImage* doubleSizeImage = nullptr;

    IplImage* mosaic1 = nullptr;
    IplImage* mosaic2 = nullptr;

    CvMat* mosaicHorizen1 = nullptr;
    CvMat* mosaicHorizen2 = nullptr;
    CvMat* mosaicVertical1 = nullptr;

    CvMat* image1Mat = nullptr;
    CvMat* tempMat = nullptr;
    ImageOctaves *Gaussianpyr;


#define Im1Mat(ROW,COL) ((float *)(image1Mat->data.fl + image1Mat->step/sizeof(float) *(ROW)))[(COL)]
#define Im1B(ROW,COL) ((uchar*)(image1->imageData + image1->widthStep*(ROW)))[(COL)*3]
#define Im1G(ROW,COL) ((uchar*)(image1->imageData + image1->widthStep*(ROW)))[(COL)*3+1]
#define Im1R(ROW,COL) ((uchar*)(image1->imageData + image1->widthStep*(ROW)))[(COL)*3+2]

    storage = cvCreateMemStorage(0);

    if ((src = cvLoadImage(path, 1)) == nullptr)
        return -1;


    image1 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 3);
    grey_im1 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
    doubleSizeImage = cvCreateImage(cvSize(2 * (src->width), 2 * (src->height)), IPL_DEPTH_8U, 3);


    image1Mat = cvCreateMat(src->height, src->width, CV_32FC1);

    cvCvtColor(src, grey_im1, CV_BGR2GRAY);

    cvConvert(grey_im1, image1Mat);

    double t = double(cvGetTickCount());

    cvConvertScale(image1Mat, image1Mat, 1.0 / 255, 0);

    int dim = min(image1Mat->rows, image1Mat->cols);
    int numoctaves = int((log(double(dim)) / log(2.0)) - 2);
    numoctaves = min(numoctaves, MAXOCTAVES);
    tempMat = sift.ScaleInitImage(image1Mat);
    Gaussianpyr = sift.BuildGaussianOctaves(tempMat);

    t = double(cvGetTickCount() - t);

    //    printf("the time of build Gaussian pyramid and DOG pyramid is %.1f/n", t / (cvGetTickFrequency()*1000.));

#define ImLevels(OCTAVE,LEVEL,ROW,COL) ((float *)(Gaussianpyr[(OCTAVE)].Octave[(LEVEL)].Level->data.fl + Gaussianpyr[(OCTAVE)].Octave[(LEVEL)].Level->step/sizeof(float) *(ROW)))[(COL)]

    for (int i = 0; i < numoctaves; i++)
    {
        if (i == 0)
        {

            mosaicHorizen1 = show.MosaicHorizen((Gaussianpyr[0].Octave)[0].Level, (Gaussianpyr[0].Octave)[1].Level);
            for (int j = 2; j < SCALESPEROCTAVE + 3; j++)
                mosaicHorizen1 = show.MosaicHorizen(mosaicHorizen1, (Gaussianpyr[0].Octave)[j].Level);
            for (int j = 0; j < NUMSIZE; j++)
                mosaicHorizen1 = image_process.halfSizeImage(mosaicHorizen1);
        }

        else if (i == 1)
        {
            mosaicHorizen2 = show.MosaicHorizen((Gaussianpyr[1].Octave)[0].Level, (Gaussianpyr[1].Octave)[1].Level);
            for (int j = 2; j < SCALESPEROCTAVE + 3; j++)
                mosaicHorizen2 = show.MosaicHorizen(mosaicHorizen2, (Gaussianpyr[1].Octave)[j].Level);
            for (int j = 0; j < NUMSIZE; j++)
                mosaicHorizen2 = image_process.halfSizeImage(mosaicHorizen2);
            mosaicVertical1 = show.MosaicVertical(mosaicHorizen1, mosaicHorizen2);
        }
        else
        {
            mosaicHorizen1 = show.MosaicHorizen((Gaussianpyr[i].Octave)[0].Level, (Gaussianpyr[i].Octave)[1].Level);
            for (int j = 2; j < SCALESPEROCTAVE + 3; j++)
                mosaicHorizen1 = show.MosaicHorizen(mosaicHorizen1, (Gaussianpyr[i].Octave)[j].Level);
            for (int j = 0; j < NUMSIZE; j++)
                mosaicHorizen1 = image_process.halfSizeImage(mosaicHorizen1);
            mosaicVertical1 = show.MosaicVertical(mosaicVertical1, mosaicHorizen1);
        }
    }
    mosaic1 = cvCreateImage(cvSize(mosaicVertical1->width, mosaicVertical1->height), IPL_DEPTH_8U, 1);
    cvConvertScale(mosaicVertical1, mosaicVertical1, 255.0, 0);
    cvConvertScaleAbs(mosaicVertical1, mosaic1, 1, 0);

    //  cvSaveImage("GaussianPyramid of me.jpg",mosaic1);
    //    cvNamedWindow("mosaic1", 1);
    //    cvShowImage("mosaic1", mosaic1);
    Mat Img0;
    Img0 = cvarrToMat(mosaic1);

    imwrite("D:/Code/cpp/homework_cv/SiftHuster/results/mosaic1.jpg", Img0);

    //    cvWaitKey(0);
    //    cvDestroyWindow("mosaic1");


    for (int i = 0; i < numoctaves; i++)
    {
        if (i == 0)
        {
            mosaicHorizen1 = show.MosaicHorizen((DOGoctaves[0].Octave)[0].Level, (DOGoctaves[0].Octave)[1].Level);
            for (int j = 2; j < SCALESPEROCTAVE + 2; j++)
                mosaicHorizen1 = show.MosaicHorizen(mosaicHorizen1, (DOGoctaves[0].Octave)[j].Level);
            for (int j = 0; j < NUMSIZE; j++)
                mosaicHorizen1 = image_process.halfSizeImage(mosaicHorizen1);
        }
        else if (i == 1)
        {
            mosaicHorizen2 = show.MosaicHorizen((DOGoctaves[1].Octave)[0].Level, (DOGoctaves[1].Octave)[1].Level);
            for (int j = 2; j < SCALESPEROCTAVE + 2; j++)
                mosaicHorizen2 = show.MosaicHorizen(mosaicHorizen2, (DOGoctaves[1].Octave)[j].Level);
            for (int j = 0; j < NUMSIZE; j++)
                mosaicHorizen2 = image_process.halfSizeImage(mosaicHorizen2);
            mosaicVertical1 = show.MosaicVertical(mosaicHorizen1, mosaicHorizen2);
        }
        else
        {
            mosaicHorizen1 = show.MosaicHorizen((DOGoctaves[i].Octave)[0].Level, (DOGoctaves[i].Octave)[1].Level);
            for (int j = 2; j < SCALESPEROCTAVE + 2; j++)
                mosaicHorizen1 = show.MosaicHorizen(mosaicHorizen1, (DOGoctaves[i].Octave)[j].Level);
            for (int j = 0; j < NUMSIZE; j++)
                mosaicHorizen1 = image_process.halfSizeImage(mosaicHorizen1);
            mosaicVertical1 = show.MosaicVertical(mosaicVertical1, mosaicHorizen1);
        }
    }

    double min_val = 0;
    double max_val = 0;
    cvMinMaxLoc(mosaicVertical1, &min_val, &max_val, nullptr, nullptr, nullptr);
    if (min_val < 0.0)
        cvAddS(mosaicVertical1, cvScalarAll((-1.0)*min_val), mosaicVertical1, nullptr);
    mosaic2 = cvCreateImage(cvSize(mosaicVertical1->width, mosaicVertical1->height), IPL_DEPTH_8U, 1);
    cvConvertScale(mosaicVertical1, mosaicVertical1, 255.0 / (max_val - min_val), 0);
    cvConvertScaleAbs(mosaicVertical1, mosaic2, 1, 0);

    Mat Img1;
    Img1 = cvarrToMat(mosaic2);
    imwrite("D:/Code/cpp/homework_cv/SiftHuster/results/DOGPyramid.jpg", Img1);

    //    cvNamedWindow("mosaic1", 1);
    //    cvShowImage("mosaic1", mosaic2);
    //    cvWaitKey(0);


    int keycount = sift.DetectKeypoint(numoctaves, Gaussianpyr);
    //int keycount2 = sift.DetectKeypoint(numoctaves2, Gaussianpyr2);
    cout << endl << "the keypoints number are " << keycount << endl;
    cvCopy(src, image1, nullptr);
    show.DisplayKeypointLocation(image1, Gaussianpyr);

    cvPyrUp(image1, doubleSizeImage, CV_GAUSSIAN_5x5);
    //    cvNamedWindow("image1", 1);
    //    cvShowImage("image1", doubleSizeImage);
    Mat Img2;
//    Mat _Img2;
//    Size dsize = Size(cvarrToMat(image1).cols, cvarrToMat(image1).rows);
    Img2 = cvarrToMat(doubleSizeImage);
//    resize(Img2, _Img2, dsize);
    imwrite("D:/Code/cpp/homework_cv/SiftHuster/results/keypoint.jpg", Img2);

    //    cvSaveImage("keypoint.jpg", doubleSizeImage);
    //    cvWaitKey(0);
    //    cvDestroyWindow("image1");

    sift.ComputeGrad_DirecandMag(numoctaves, Gaussianpyr);
    sift.AssignTheMainOrientation(numoctaves, Gaussianpyr, mag_pyr, grad_pyr);
    cvCopy(src, image1, nullptr);
    sift.DisplayOrientation(image1, Gaussianpyr);

    //  cvPyrUp( image1, doubleSizeImage, CV_GAUSSIAN_5x5 );
    //    cvNamedWindow("image1", 1);
    //  cvResizeWindow("image1", 2*(image1->width), 2*(image1->height) );
    //    cvShowImage("image1", image1);
    Mat Img3;
    Img3 = cvarrToMat(image1);
    imwrite("D:/Code/cpp/homework_cv/SiftHuster/results/direction.jpg", Img3);
    //    cvWaitKey(0);


    int count = sift.ExtractFeatureDescriptors(numoctaves, Gaussianpyr, keyDescriptors);
    Keypoint p = keyDescriptors;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < 128; j++) {
            printf("%f ", double(p->descrip[j]));
        }
        cout << endl;
        p = p->next;
    }
    /*for (int i = 0; i < count2; i++) {
        for (int j = 0; j < 128; j++) {
            printf("%f ", p2->descrip[j]);
        }
        cout << endl;
        p2 = p2->next;
    }*/
    /*cout << endl << count << endl << count2;*/
    //    cvWaitKey(0);
    //    cvDestroyWindow("image1");
    //    cvDestroyWindow("mosaic1");
    cvReleaseImage(&image1);
    cvReleaseImage(&grey_im1);
    cvReleaseImage(&mosaic1);
    cvReleaseImage(&mosaic2);
    return 0;
}


