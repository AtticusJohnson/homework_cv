#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include<iostream>
#include<typeinfo>
#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include<opencv/highgui.h>

//Sigma of base image -- See D.L.'s paper.
#define INITSIGMA 0.5
//Sigma of each octave -- See D.L.'s paper.
#define SIGMA sqrt(3)//1.6//

//Number of scales per octave.  See D.L.'s paper.
#define SCALESPEROCTAVE 2
#define MAXOCTAVES 4
#define CONTRAST_THRESHOLD   0.02
#define CURVATURE_THRESHOLD  10.0
#define DOUBLE_BASE_IMAGE_SIZE 1
#define peakRelThresh 0.8
#define LEN 128
#define GridSpacing 4  //特征描述点，网格
#define NUMSIZE 2
#define PI 3.14159265358979323846
#define GAUSSKERN 3.5

//Data structure for a float image.
typedef struct ImageSI { /*金字塔每一层*/
    float levelsigma;
    int levelsigmalength;
    float absolute_sigma;
    CvMat *Level;  //CvMat是OPENCV的矩阵类，其元素可以是图像的象素值
} ImageLevels;

typedef struct ImageSI2 {   /*金字塔每一阶梯*/
    int row, col;          //Dimensions of image.
    float subsample;
    ImageSI *Octave;
} ImageOctaves;

typedef struct KeypointSt
{
    float row, col;
    float sx, sy;
    int octave, level;

    float scale, ori, mag;
    float *descrip;
    KeypointSt *next;
} *Keypoint;


class ImageProcess
{
public:
    ImageProcess();
    CvMat* halfSizeImage(CvMat * im);     //缩小图像：下采样
    CvMat* doubleSizeImage(CvMat * im);   //扩大图像：最近临方法
    CvMat* doubleSizeImage2(CvMat * im);  //扩大图像：线性插值
    float getPixelBI(CvMat * im, float col, float row);//双线性插值函数
    void normalizeVec(float* vec, int dim);//向量归一化
    void normalizeMat(CvMat* mat);        //矩阵归一化
    float GetVecNorm(float* vec, int dim);

    CvMat* GaussianKernel2D(float sigma);  //得到2维高斯核
    float* GaussianKernel1D(float sigma, int dim); //得到1维高斯核
    float ConvolveLocWidth(float* kernel, int dim, CvMat * src, int x, int y);
    //在整个图像宽度方向进行1D高斯卷积
    void Convolve1DWidth(float* kern, int dim, CvMat * src, CvMat * dst);
    //在具体像素处高度方向进行高斯卷积
    float ConvolveLocHeight(float* kernel, int dim, CvMat * src, int x, int y);
    //在整个图像高度方向进行1D高斯卷积
    void Convolve1DHeight(float* kern, int dim, CvMat * src, CvMat * dst);

    //用高斯函数模糊图像
    int BlurImage(CvMat * src, CvMat * dst, float sigma);
};

#endif // IMAGEPROCESS_H
