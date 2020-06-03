#ifndef MYSIFT_H
#define MYSIFT_H
#include<iostream>
#include<typeinfo>
#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include<opencv/highgui.h>
#include "sift/ImageProcess.h"


class SiftProcess
{
public:
    SiftProcess();

    /*SIFT算法第一步：图像预处理,金字塔初始化*/
    CvMat* ScaleInitImage(CvMat * im);

    /*SIFT算法第二步：建立高斯金字塔函数*/
    ImageOctaves* BuildGaussianOctaves(CvMat * image);

    /*SIFT算法第三步：特征点位置检测，最后确定特征点的位置*/
    int DetectKeypoint(int numoctaves, ImageOctaves *GaussianPyr);

    /*SIFT算法第四步：计算高斯图像的梯度方向和幅值，计算各个特征点的主方向*/
    void ComputeGrad_DirecandMag(int numoctaves, ImageOctaves *GaussianPyr);
    //进行方向直方图统计
    int FindClosestRotationBin(int binCount, float angle);
    //对方向直方图滤波
    void AverageWeakBins(double* bins, int binCount);
    //确定真正的主方向
    bool InterpolateOrientation(double left, double middle, double right, double *degreeCorrection, double *peakValue);
    //确定各个特征点处的主方向函数
    void AssignTheMainOrientation(int numoctaves, ImageOctaves *GaussianPyr, ImageOctaves *mag_pyr, ImageOctaves *grad_pyr);
    //显示主方向
    void DisplayOrientation(IplImage* image, ImageOctaves *GaussianPyr);

    /*SIFT算法第五步：抽取各个特征点处的特征描述字*/
    int ExtractFeatureDescriptors(int numoctaves, ImageOctaves *GaussianPyr, Keypoint p);



};


#endif // MYSIFT_H
