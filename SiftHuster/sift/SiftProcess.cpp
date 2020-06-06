#include "sift/SiftProcess.h"

using namespace std;
using namespace cv;

ImageProcess image_process;
ImageOctaves *DOGoctaves;
ImageOctaves *mag_thresh;
ImageOctaves *mag_pyr;
ImageOctaves *grad_pyr;
Keypoint keypoints;
Keypoint keyDescriptors;

SiftProcess::SiftProcess()
{

}

CvMat* SiftProcess::ScaleInitImage(CvMat * im)
{
    double sigma, preblur_sigma;
    CvMat *imMat;
    CvMat * dst;
    CvMat *tempMat;

    imMat = cvCreateMat(im->rows, im->cols, CV_32FC1);
    image_process.BlurImage(im, imMat, INITSIGMA);

    if (DOUBLE_BASE_IMAGE_SIZE)
    {
        tempMat = image_process.doubleSizeImage2(imMat);
#define TEMPMAT(ROW,COL) ((float *)(tempMat->data.fl + tempMat->step/sizeof(float) * (ROW)))[(COL)]

        dst = cvCreateMat(tempMat->rows, tempMat->cols, CV_32FC1);
        preblur_sigma = 1.0;//sqrt(2 - 4*INITSIGMA*INITSIGMA);
        image_process.BlurImage(tempMat, dst, preblur_sigma);

        // The initial blurring for the first image of the first octave of the pyramid.
        sigma = sqrt((4 * INITSIGMA*INITSIGMA) + preblur_sigma * preblur_sigma);
        //  sigma = sqrt(SIGMA * SIGMA - INITSIGMA * INITSIGMA * 4);
        //printf("Init Sigma: %f/n", sigma);
        image_process.BlurImage(dst, tempMat, sigma);
        cvReleaseMat(&dst);
        return tempMat;
    }
    else
    {
        dst = cvCreateMat(im->rows, im->cols, CV_32FC1);
        //sigma = sqrt(SIGMA * SIGMA - INITSIGMA * INITSIGMA);
        preblur_sigma = 1.0;//sqrt(2 - 4*INITSIGMA*INITSIGMA);
        sigma = sqrt((4 * INITSIGMA*INITSIGMA) + preblur_sigma * preblur_sigma);
        //printf("Init Sigma: %f/n", sigma);
        image_process.BlurImage(imMat, dst, sigma);
        return dst;
    }
}

ImageOctaves* SiftProcess::BuildGaussianOctaves(CvMat * image)
{

    ImageOctaves *octaves;
    CvMat *tempMat;
    CvMat *dst;
    CvMat *temp;

    int i, j;
    double k = pow(2, 1.0 / ((float)SCALESPEROCTAVE));
    float preblur_sigma, initial_sigma, sigma1, sigma2, sigma, absolute_sigma, sigma_f;


    int dim = min(image->rows, image->cols);
    int numoctaves = (int)(log((double)dim) / log(2.0)) - 2;

    numoctaves = min(numoctaves, MAXOCTAVES);
    octaves = (ImageOctaves*)malloc(numoctaves * sizeof(ImageOctaves));
    DOGoctaves = (ImageOctaves*)malloc(numoctaves * sizeof(ImageOctaves));

    printf("BuildGaussianOctaves(): Base image dimension is %dx%d/n", (int)(0.5*(image->cols)), (int)(0.5*(image->rows)));
    printf("BuildGaussianOctaves(): Building %d octaves/n", numoctaves);

    // start with initial source image
    tempMat = cvCloneMat(image);
    // preblur_sigma = 1.0;//sqrt(2 - 4*INITSIGMA*INITSIGMA);
    initial_sigma = sqrt(2);//sqrt( (4*INITSIGMA*INITSIGMA) + preblur_sigma * preblur_sigma );
    //   initial_sigma = sqrt(SIGMA * SIGMA - INITSIGMA * INITSIGMA * 4);


    for (i = 0; i < numoctaves; i++)
    {


        printf("Building octave %d of dimesion (%d, %d)/n", i, tempMat->cols, tempMat->rows);


        octaves[i].Octave = (ImageLevels*)(malloc((SCALESPEROCTAVE + 3) * sizeof(ImageLevels)));

        DOGoctaves[i].Octave = (ImageLevels*)malloc((SCALESPEROCTAVE + 2) * sizeof(ImageLevels));

        (octaves[i].Octave)[0].Level = tempMat;

        octaves[i].col = tempMat->cols;
        octaves[i].row = tempMat->rows;
        DOGoctaves[i].col = tempMat->cols;
        DOGoctaves[i].row = tempMat->rows;
        if (DOUBLE_BASE_IMAGE_SIZE)
            octaves[i].subsample = pow(2, i)*0.5;
        else
            octaves[i].subsample = pow(2, i);

        if (i == 0)
        {
            (octaves[0].Octave)[0].levelsigma = initial_sigma;
            (octaves[0].Octave)[0].absolute_sigma = initial_sigma;
            printf("0 scale and blur sigma : %f /n", (octaves[0].subsample) * ((octaves[0].Octave)[0].absolute_sigma));
        }
        else
        {
            (octaves[i].Octave)[0].levelsigma = (octaves[i - 1].Octave)[SCALESPEROCTAVE].levelsigma;
            (octaves[i].Octave)[0].absolute_sigma = (octaves[i - 1].Octave)[SCALESPEROCTAVE].absolute_sigma;
            printf("0 scale and blur sigma : %f /n", ((octaves[i].Octave)[0].absolute_sigma));
        }
        sigma = initial_sigma;

        for (j = 1; j < SCALESPEROCTAVE + 3; j++)
        {
            dst = cvCreateMat(tempMat->rows, tempMat->cols, CV_32FC1);
            temp = cvCreateMat(tempMat->rows, tempMat->cols, CV_32FC1);
            // 2 passes of 1D on original
            //   if(i!=0)
            //   {
            //       sigma1 = pow(k, j - 1) * ((octaves[i-1].Octave)[j-1].levelsigma);
            //          sigma2 = pow(k, j) * ((octaves[i].Octave)[j-1].levelsigma);
            //       sigma = sqrt(sigma2*sigma2 - sigma1*sigma1);
            sigma_f = sqrt(k*k - 1)*sigma;
            //   }
            //   else
            //   {
            //       sigma = sqrt(SIGMA * SIGMA - INITSIGMA * INITSIGMA * 4)*pow(k,j);
            //   }
            sigma = k * sigma;
            absolute_sigma = sigma * (octaves[i].subsample);
            printf("%d scale and Blur sigma: %f  /n", j, absolute_sigma);

            (octaves[i].Octave)[j].levelsigma = sigma;
            (octaves[i].Octave)[j].absolute_sigma = absolute_sigma;

            int length = image_process.BlurImage((octaves[i].Octave)[j - 1].Level, dst, sigma_f);//相应尺度
            (octaves[i].Octave)[j].levelsigmalength = length;
            (octaves[i].Octave)[j].Level = dst;

            cvSub(((octaves[i].Octave)[j]).Level, ((octaves[i].Octave)[j - 1]).Level, temp, 0);
            //         cvAbsDiff( ((octaves[i].Octave)[j]).Level, ((octaves[i].Octave)[j-1]).Level, temp );
            ((DOGoctaves[i].Octave)[j - 1]).Level = temp;
        }
        // halve the image size for next iteration
        tempMat = image_process.halfSizeImage(((octaves[i].Octave)[SCALESPEROCTAVE].Level));
    }
    return octaves;
}

int SiftProcess::DetectKeypoint(int numoctaves, ImageOctaves *GaussianPyr)
{

    double curvature_threshold;
    curvature_threshold = ((CURVATURE_THRESHOLD + 1)*(CURVATURE_THRESHOLD + 1)) / CURVATURE_THRESHOLD;
#define ImLevels(OCTAVE,LEVEL,ROW,COL) ((float *)(DOGoctaves[(OCTAVE)].Octave[(LEVEL)].Level->data.fl + DOGoctaves[(OCTAVE)].Octave[(LEVEL)].Level->step/sizeof(float) *(ROW)))[(COL)]

    int   keypoint_count = 0;
    for (int i = 0; i < numoctaves; i++)
    {
        for (int j = 1; j < SCALESPEROCTAVE + 1; j++)
        {

            //float sigma=(GaussianPyr[i].Octave)[j].levelsigma;
            //int dim = (int) (max(3.0f, 2.0*GAUSSKERN *sigma + 1.0f)*0.5);
            int dim = (int)(0.5*((GaussianPyr[i].Octave)[j].levelsigmalength) + 0.5);

            for (int m = dim; m < ((DOGoctaves[i].row) - dim); m++)
                for (int n = dim; n < ((DOGoctaves[i].col) - dim); n++)
                {
                    if (fabs(ImLevels(i, j, m, n)) >= CONTRAST_THRESHOLD)
                    {

                        if (ImLevels(i, j, m, n) != 0.0)
                        {
                            float inf_val = ImLevels(i, j, m, n);
                            if (((inf_val <= ImLevels(i, j - 1, m - 1, n - 1)) &&
                                 (inf_val <= ImLevels(i, j - 1, m, n - 1)) &&
                                 (inf_val <= ImLevels(i, j - 1, m + 1, n - 1)) &&
                                 (inf_val <= ImLevels(i, j - 1, m - 1, n)) &&
                                 (inf_val <= ImLevels(i, j - 1, m, n)) &&
                                 (inf_val <= ImLevels(i, j - 1, m + 1, n)) &&
                                 (inf_val <= ImLevels(i, j - 1, m - 1, n + 1)) &&
                                 (inf_val <= ImLevels(i, j - 1, m, n + 1)) &&
                                 (inf_val <= ImLevels(i, j - 1, m + 1, n + 1)) &&

                                 (inf_val <= ImLevels(i, j, m - 1, n - 1)) &&
                                 (inf_val <= ImLevels(i, j, m, n - 1)) &&
                                 (inf_val <= ImLevels(i, j, m + 1, n - 1)) &&
                                 (inf_val <= ImLevels(i, j, m - 1, n)) &&
                                 (inf_val <= ImLevels(i, j, m + 1, n)) &&
                                 (inf_val <= ImLevels(i, j, m - 1, n + 1)) &&
                                 (inf_val <= ImLevels(i, j, m, n + 1)) &&
                                 (inf_val <= ImLevels(i, j, m + 1, n + 1)) &&

                                 (inf_val <= ImLevels(i, j + 1, m - 1, n - 1)) &&
                                 (inf_val <= ImLevels(i, j + 1, m, n - 1)) &&
                                 (inf_val <= ImLevels(i, j + 1, m + 1, n - 1)) &&
                                 (inf_val <= ImLevels(i, j + 1, m - 1, n)) &&
                                 (inf_val <= ImLevels(i, j + 1, m, n)) &&
                                 (inf_val <= ImLevels(i, j + 1, m + 1, n)) &&
                                 (inf_val <= ImLevels(i, j + 1, m - 1, n + 1)) &&
                                 (inf_val <= ImLevels(i, j + 1, m, n + 1)) &&
                                 (inf_val <= ImLevels(i, j + 1, m + 1, n + 1))
                                 ) ||
                                    ((inf_val >= ImLevels(i, j - 1, m - 1, n - 1)) &&
                                     (inf_val >= ImLevels(i, j - 1, m, n - 1)) &&
                                     (inf_val >= ImLevels(i, j - 1, m + 1, n - 1)) &&
                                     (inf_val >= ImLevels(i, j - 1, m - 1, n)) &&
                                     (inf_val >= ImLevels(i, j - 1, m, n)) &&
                                     (inf_val >= ImLevels(i, j - 1, m + 1, n)) &&
                                     (inf_val >= ImLevels(i, j - 1, m - 1, n + 1)) &&
                                     (inf_val >= ImLevels(i, j - 1, m, n + 1)) &&
                                     (inf_val >= ImLevels(i, j - 1, m + 1, n + 1)) &&

                                     (inf_val >= ImLevels(i, j, m - 1, n - 1)) &&
                                     (inf_val >= ImLevels(i, j, m, n - 1)) &&
                                     (inf_val >= ImLevels(i, j, m + 1, n - 1)) &&
                                     (inf_val >= ImLevels(i, j, m - 1, n)) &&
                                     (inf_val >= ImLevels(i, j, m + 1, n)) &&
                                     (inf_val >= ImLevels(i, j, m - 1, n + 1)) &&
                                     (inf_val >= ImLevels(i, j, m, n + 1)) &&
                                     (inf_val >= ImLevels(i, j, m + 1, n + 1)) &&

                                     (inf_val >= ImLevels(i, j + 1, m - 1, n - 1)) &&
                                     (inf_val >= ImLevels(i, j + 1, m, n - 1)) &&
                                     (inf_val >= ImLevels(i, j + 1, m + 1, n - 1)) &&
                                     (inf_val >= ImLevels(i, j + 1, m - 1, n)) &&
                                     (inf_val >= ImLevels(i, j + 1, m, n)) &&
                                     (inf_val >= ImLevels(i, j + 1, m + 1, n)) &&
                                     (inf_val >= ImLevels(i, j + 1, m - 1, n + 1)) &&
                                     (inf_val >= ImLevels(i, j + 1, m, n + 1)) &&
                                     (inf_val >= ImLevels(i, j + 1, m + 1, n + 1))
                                     ))
                            {
                                if (fabs(ImLevels(i, j, m, n)) >= CONTRAST_THRESHOLD)
                                {

                                    // Compute the entries of the Hessian matrix at the extrema location.
                                    /*
                                    1   0   -1
                                    0   0   0
                                    -1   0   1         *0.25
                                    */
                                    // Compute the trace and the determinant of the Hessian.
                                    //Tr_H = Dxx + Dyy;
                                    //Det_H = Dxx*Dyy - Dxy^2;
                                    float Dxx, Dyy, Dxy, Tr_H, Det_H, curvature_ratio;
                                    Dxx = ImLevels(i, j, m, n - 1) + ImLevels(i, j, m, n + 1) - 2.0*ImLevels(i, j, m, n);
                                    Dyy = ImLevels(i, j, m - 1, n) + ImLevels(i, j, m + 1, n) - 2.0*ImLevels(i, j, m, n);
                                    Dxy = ImLevels(i, j, m - 1, n - 1) + ImLevels(i, j, m + 1, n + 1) - ImLevels(i, j, m + 1, n - 1) - ImLevels(i, j, m - 1, n + 1);
                                    Tr_H = Dxx + Dyy;
                                    Det_H = Dxx * Dyy - Dxy * Dxy;
                                    // Compute the ratio of the principal curvatures.
                                    curvature_ratio = (1.0*Tr_H*Tr_H) / Det_H;
                                    if ((Det_H >= 0.0) && (curvature_ratio <= curvature_threshold))
                                    {

                                        keypoint_count++;
                                        Keypoint k;
                                        /* Allocate memory for the keypoint. */
                                        k = (Keypoint)malloc(sizeof(struct KeypointSt));
                                        k->next = keypoints;
                                        keypoints = k;
                                        k->row = m * (GaussianPyr[i].subsample);
                                        k->col = n * (GaussianPyr[i].subsample);
                                        k->sy = m;
                                        k->sx = n;
                                        k->octave = i;
                                        k->level = j;
                                        k->scale = (GaussianPyr[i].Octave)[j].absolute_sigma;
                                    }//if >curvature_thresh
                                }//if >contrast
                            }//if inf value
                        }//if non zero
                    }//if >contrast
                }  //for concrete image level col
        }//for levels
    }//for octaves
    return keypoint_count;
}

void SiftProcess::ComputeGrad_DirecandMag(int numoctaves, ImageOctaves *GaussianPyr)
{   // Compute the gradient direction and magnitude of the gaussian pyramid images

    // ImageOctaves *mag_thresh ;
    mag_pyr = (ImageOctaves*)malloc(numoctaves * sizeof(ImageOctaves));
    grad_pyr = (ImageOctaves*)malloc(numoctaves * sizeof(ImageOctaves));
    // float sigma=( (GaussianPyr[0].Octave)[SCALESPEROCTAVE+2].absolute_sigma ) / GaussianPyr[0].subsample;
    // int dim = (int) (max(3.0f, 2 * GAUSSKERN *sigma + 1.0f)*0.5+0.5);
#define ImLevels(OCTAVE,LEVEL,ROW,COL) ((float *)(GaussianPyr[(OCTAVE)].Octave[(LEVEL)].Level->data.fl + GaussianPyr[(OCTAVE)].Octave[(LEVEL)].Level->step/sizeof(float) *(ROW)))[(COL)]
    for (int i = 0; i < numoctaves; i++)
    {
        mag_pyr[i].Octave = (ImageLevels*)malloc((SCALESPEROCTAVE) * sizeof(ImageLevels));
        grad_pyr[i].Octave = (ImageLevels*)malloc((SCALESPEROCTAVE) * sizeof(ImageLevels));
        for (int j = 1; j < SCALESPEROCTAVE + 1; j++)
        {
            CvMat *Mag = cvCreateMat(GaussianPyr[i].row, GaussianPyr[i].col, CV_32FC1);
            CvMat *Ori = cvCreateMat(GaussianPyr[i].row, GaussianPyr[i].col, CV_32FC1);
            CvMat *tempMat1 = cvCreateMat(GaussianPyr[i].row, GaussianPyr[i].col, CV_32FC1);
            CvMat *tempMat2 = cvCreateMat(GaussianPyr[i].row, GaussianPyr[i].col, CV_32FC1);
            cvZero(Mag);
            cvZero(Ori);
            cvZero(tempMat1);
            cvZero(tempMat2);
#define MAG(ROW,COL) ((float *)(Mag->data.fl + Mag->step/sizeof(float) *(ROW)))[(COL)]
#define ORI(ROW,COL) ((float *)(Ori->data.fl + Ori->step/sizeof(float) *(ROW)))[(COL)]
#define TEMPMAT1(ROW,COL) ((float *)(tempMat1->data.fl + tempMat1->step/sizeof(float) *(ROW)))[(COL)]
#define TEMPMAT2(ROW,COL) ((float *)(tempMat2->data.fl + tempMat2->step/sizeof(float) *(ROW)))[(COL)]
            for (int m = 1; m < (GaussianPyr[i].row - 1); m++)
                for (int n = 1; n < (GaussianPyr[i].col - 1); n++)
                {

                    TEMPMAT1(m, n) = 0.5*(ImLevels(i, j, m, n + 1) - ImLevels(i, j, m, n - 1));  //dx
                    TEMPMAT2(m, n) = 0.5*(ImLevels(i, j, m + 1, n) - ImLevels(i, j, m - 1, n));  //dy
                    MAG(m, n) = sqrt(TEMPMAT1(m, n)*TEMPMAT1(m, n) + TEMPMAT2(m, n)*TEMPMAT2(m, n));  //mag

                    ORI(m, n) = atan(TEMPMAT2(m, n) / TEMPMAT1(m, n));
                    if (ORI(m, n) == CV_PI)
                        ORI(m, n) = -CV_PI;
                }
            ((mag_pyr[i].Octave)[j - 1]).Level = Mag;
            ((grad_pyr[i].Octave)[j - 1]).Level = Ori;
            cvReleaseMat(&tempMat1);
            cvReleaseMat(&tempMat2);
        }//for levels
    }//for octaves
}

int SiftProcess::FindClosestRotationBin(int binCount, float angle)
{
    angle += CV_PI;
    angle /= 2.0 * CV_PI;
    // calculate the aligned bin
    angle *= binCount;
    int idx = (int)angle;
    if (idx == binCount)
        idx = 0;
    return (idx);
}

// Average the content of the direction bins.
void SiftProcess::AverageWeakBins(double* hist, int binCount)
{
    // TODO: make some tests what number of passes is the best. (its clear
    // one is not enough, as we may have something like
    // ( 0.4, 0.4, 0.3, 0.4, 0.4 ))
    for (int sn = 0; sn < 2; ++sn)
    {
        double firstE = hist[0];
        double last = hist[binCount - 1];
        for (int sw = 0; sw < binCount; ++sw)
        {
            double cur = hist[sw];
            double next = (sw == (binCount - 1)) ? firstE : hist[(sw + 1) % binCount];
            hist[sw] = (last + cur + next) / 3.0;
            last = cur;
        }
    }
}

bool SiftProcess::InterpolateOrientation(double left, double middle, double right, double *degreeCorrection, double *peakValue)
{
    // Fit a parabol to the three points (-1.0 ; left), (0.0 ; middle) and
    // (1.0 ; right).
    // Formulas:
    // f(x) = a (x - c)^2 + b
    // c is the peak offset (where f'(x) is zero), b is the peak value.
    // In case there is an error false is returned, otherwise a correction
    // value between [-1 ; 1] is returned in 'degreeCorrection', where -1
    // means the peak is located completely at the left vector, and -0.5 just
    // in the middle between left and middle and > 0 to the right side. In
    // 'peakValue' the maximum estimated peak value is stored.
    double a = ((left + right) - 2.0 * middle) / 2.0;
    // degreeCorrection = peakValue = Double.NaN;

    // Not a parabol
    if (a == 0.0)
        return false;
    double c = (((left - middle) / a) - 1.0) / 2.0;
    double b = middle - c * c * a;
    if (c < -0.5 || c > 0.5)
        return false;
    *degreeCorrection = c;
    *peakValue = b;
    return true;
}

void SiftProcess::AssignTheMainOrientation(int numoctaves, ImageOctaves *GaussianPyr, ImageOctaves *mag_pyr, ImageOctaves *grad_pyr)
{
    // Set up the histogram bin centers for a 36 bin histogram.
    int num_bins = 36;
    float hist_step = 2.0*PI / num_bins;
    float hist_orient[36];
    for (int i = 0; i < 36; i++)
        hist_orient[i] = -PI + i * hist_step;
    float sigma1 = (((GaussianPyr[0].Octave)[SCALESPEROCTAVE].absolute_sigma)) / (GaussianPyr[0].subsample);//SCALESPEROCTAVE+2
    int zero_pad = (int)(max(3.0, 2 * GAUSSKERN *sigma1 + 1.0)*0.5 + 0.5);
    //Assign orientations to the keypoints.
#define ImLevels(OCTAVES,LEVELS,ROW,COL) ((float *)((GaussianPyr[(OCTAVES)].Octave[(LEVELS)].Level)->data.fl + (GaussianPyr[(OCTAVES)].Octave[(LEVELS)].Level)->step/sizeof(float) *(ROW)))[(COL)]

    int keypoint_count = 0;
    Keypoint p = keypoints;
    while (p)
    {
        int i = p->octave;
        int j = p->level;
        int m = p->sy;
        int n = p->sx;
        if ((m >= zero_pad) && (m < GaussianPyr[i].row - zero_pad) &&
                (n >= zero_pad) && (n < GaussianPyr[i].col - zero_pad))
        {
            float sigma = (((GaussianPyr[i].Octave)[j].absolute_sigma)) / (GaussianPyr[i].subsample);

            CvMat* mat = image_process.GaussianKernel2D(sigma);
            int dim = (int)(0.5 * (mat->rows));

#define MAT(ROW,COL) ((float *)(mat->data.fl + mat->step/sizeof(float) *(ROW)))[(COL)]

            double* orienthist = (double *)malloc(36 * sizeof(double));
//            double* orienthist = new double;
            for (int sw = 0; sw < 36; ++sw)
            {
                orienthist[sw] = 0.0;
            }

            for (int x = m - dim, mm = 0; x <= (m + dim); x++, mm++)
                for (int y = n - dim, nn = 0; y <= (n + dim); y++, nn++)
                {

                    double dx = 0.5*(ImLevels(i, j, x, y + 1) - ImLevels(i, j, x, y - 1));  //dx
                    double dy = 0.5*(ImLevels(i, j, x + 1, y) - ImLevels(i, j, x - 1, y));  //dy
                    double mag = sqrt(dx*dx + dy * dy);  //mag

                    double Ori = atan(1.0*dy / dx);
                    int binIdx = FindClosestRotationBin(36, Ori);
                    orienthist[binIdx] = orienthist[binIdx] + 1.0* mag * MAT(mm, nn);
                }
            // Find peaks in the orientation histogram using nonmax suppression.
            AverageWeakBins(orienthist, 36);
            // find the maximum peak in gradient orientation
            double maxGrad = 0.0;
            int maxBin = 0;
            for (int b = 0; b < 36; ++b)
            {
                if (orienthist[b] > maxGrad)
                {
                    maxGrad = orienthist[b];
                    maxBin = b;
                }
            }
            // First determine the real interpolated peak high at the maximum bin
            // position, which is guaranteed to be an absolute peak.
            double maxPeakValue = 0.0;
            double maxDegreeCorrection = 0.0;
            if ((InterpolateOrientation(orienthist[maxBin == 0 ? (36 - 1) : (maxBin - 1)],
                                        orienthist[maxBin], orienthist[(maxBin + 1) % 36],
                                        &maxDegreeCorrection, &maxPeakValue)) == false)
                printf("BUG: Parabola fitting broken");

            // Now that we know the maximum peak value, we can find other keypoint
            // orientations, which have to fulfill two criterias:
            //
            //  1. They must be a local peak themselves. Else we might add a very
            //     similar keypoint orientation twice (imagine for example the
            //     values: 0.4 1.0 0.8, if 1.0 is maximum peak, 0.8 is still added
            //     with the default threshhold, but the maximum peak orientation
            //     was already added).
            //  2. They must have at least peakRelThresh times the maximum peak
            //     value.
            bool binIsKeypoint[36];
            for (int b = 0; b < 36; ++b)
            {
                binIsKeypoint[b] = false;
                // The maximum peak of course is
                if (b == maxBin)
                {
                    binIsKeypoint[b] = true;
                    continue;
                }
                // Local peaks are, too, in case they fulfill the threshhold
                if (orienthist[b] < (peakRelThresh * maxPeakValue))
                    continue;
                int leftI = (b == 0) ? (36 - 1) : (b - 1);
                int rightI = (b + 1) % 36;
                if (orienthist[b] <= orienthist[leftI] || orienthist[b] <= orienthist[rightI])
                    continue; // no local peak
                binIsKeypoint[b] = true;
            }
            // find other possible locations
            double oneBinRad = (2.0 * PI) / 36;
            for (int b = 0; b < 36; ++b)
            {
                if (binIsKeypoint[b] == false)
                    continue;
                int bLeft = (b == 0) ? (36 - 1) : (b - 1);
                int bRight = (b + 1) % 36;
                // Get an interpolated peak direction and value guess.
                double peakValue;
                double degreeCorrection;

                double maxPeakValue, maxDegreeCorrection;
                if (InterpolateOrientation(orienthist[maxBin == 0 ? (36 - 1) : (maxBin - 1)],
                                           orienthist[maxBin], orienthist[(maxBin + 1) % 36],
                                           &degreeCorrection, &peakValue) == false)
                {
                    printf("BUG: Parabola fitting broken");
                }

                double degree = (b + degreeCorrection) * oneBinRad - PI;
                if (degree < -PI)
                    degree += 2.0 * PI;
                else if (degree > PI)
                    degree -= 2.0 * PI;

                Keypoint k;
                /* Allocate memory for the keypoint Descriptor. */
                k = (Keypoint)malloc(sizeof(struct KeypointSt));
                k->next = keyDescriptors;
                keyDescriptors = k;
                k->descrip = (float*)malloc(LEN * sizeof(float));
//                k->descrip = new float;
                k->row = p->row;
                k->col = p->col;
                k->sy = p->sy;
                k->sx = p->sx;
                k->octave = p->octave;
                k->level = p->level;
                k->scale = p->scale;
                k->ori = degree;
                k->mag = peakValue;
            }//for
            delete(orienthist);
        }
        p = p->next;

    }

}

void SiftProcess::DisplayOrientation(IplImage* image, ImageOctaves *GaussianPyr)
{
    Keypoint p = keyDescriptors;
    while (p)
    {
        float scale = (GaussianPyr[p->octave].Octave)[p->level].absolute_sigma;
        float autoscale = 3.0;
        float uu = autoscale * scale*cos(p->ori);
        float vv = autoscale * scale*sin(p->ori);
        float x = (p->col) + uu;
        float y = (p->row) + vv;
        cvLine(image, cvPoint((int)(p->col), (int)(p->row)),
               cvPoint((int)x, (int)y), CV_RGB(255, 255, 0),
               1, 8, 0);
        // Arrow head parameters
        float alpha = 0.33; // Size of arrow head relative to the length of the vector
        float beta = 0.33;  // Width of the base of the arrow head relative to the length

        float xx0 = (p->col) + uu - alpha * (uu + beta * vv);
        float yy0 = (p->row) + vv - alpha * (vv - beta * uu);
        float xx1 = (p->col) + uu - alpha * (uu - beta * vv);
        float yy1 = (p->row) + vv - alpha * (vv + beta * uu);
        cvLine(image, cvPoint((int)xx0, (int)yy0),
               cvPoint((int)x, (int)y), CV_RGB(255, 255, 0),
               1, 8, 0);
        cvLine(image, cvPoint((int)xx1, (int)yy1),
               cvPoint((int)x, (int)y), CV_RGB(255, 255, 0),
               1, 8, 0);
        p = p->next;
    }
}

int SiftProcess::ExtractFeatureDescriptors(int numoctaves, ImageOctaves *GaussianPyr, Keypoint p)
{
    int count = 0;
    // The orientation histograms have 8 bins
    float orient_bin_spacing = (float)PI / 4;
    float orient_angles[8] = { (float)-PI,(float)-PI + orient_bin_spacing,(float)-PI * 0.5, -orient_bin_spacing,
                               (float)0.0, (float)orient_bin_spacing, (float)PI*0.5,  (float)PI + orient_bin_spacing };

    float *feat_grid = (float *)malloc(2 * 16 * sizeof(float));
    for (int i = 0; i < GridSpacing; i++)
    {
        for (int j = 0; j < 2 * GridSpacing; ++j, ++j)
        {
            feat_grid[i * 2 * GridSpacing + j] = -6.0 + i * GridSpacing;
            feat_grid[i * 2 * GridSpacing + j + 1] = -6.0 + 0.5*j*GridSpacing;
        }
    }

    float *feat_samples = (float *)malloc(2 * 256 * sizeof(float));
    for (int i = 0; i < 4 * GridSpacing; i++)
    {
        for (int j = 0; j < 8 * GridSpacing; j += 2)
        {
            feat_samples[i * 8 * GridSpacing + j] = -(2 * GridSpacing - 0.5) + i;
            feat_samples[i * 8 * GridSpacing + j + 1] = -(2 * GridSpacing - 0.5) + 0.5*j;
        }
    }
    float feat_window = 2 * GridSpacing;
    //Keypoint p = keyDescriptors;

    while (p)
    {
        float scale = (GaussianPyr[p->octave].Octave)[p->level].absolute_sigma;

        float sine = sin(p->ori);
        float cosine = cos(p->ori);

        float *featcenter = (float *)malloc(2 * 16 * sizeof(float));
        for (int i = 0; i < GridSpacing; i++)
        {
            for (int j = 0; j < 2 * GridSpacing; j += 2)
            {
                float x = feat_grid[i * 2 * GridSpacing + j];
                float y = feat_grid[i * 2 * GridSpacing + j + 1];
                featcenter[i * 2 * GridSpacing + j] = ((cosine * x + sine * y) + p->sx);
                featcenter[i * 2 * GridSpacing + j + 1] = ((-sine * x + cosine * y) + p->sy);
            }
        }
        // calculate sample window coordinates (rotated along keypoint)
        float *feat = (float *)malloc(2 * 256 * sizeof(float));
        for (int i = 0; i < 2 * 64 * GridSpacing; i++, i++)
        {
            float x = feat_samples[i];
            float y = feat_samples[i + 1];
            feat[i] = ((cosine * x + sine * y) + p->sx);
            feat[i + 1] = ((-sine * x + cosine * y) + p->sy);
        }
        //Initialize the feature descriptor.
        float *feat_desc = (float *)malloc(128 * sizeof(float));
        for (int i = 0; i < 128; i++)
        {
            feat_desc[i] = 0.0;
            //printf("%f  ",feat_desc[i]);
        }
        //printf("/n");
        for (int i = 0; i < 512; ++i, ++i)
        {
            float x_sample = feat[i];
            float y_sample = feat[i + 1];
            // Interpolate the gradient at the sample position
            /*
            0   1   0
            1   *   1
            0   1   0
            */
            float sample12 = image_process.getPixelBI(((GaussianPyr[p->octave].Octave)[p->level]).Level, x_sample, y_sample - 1);
            float sample21 = image_process.getPixelBI(((GaussianPyr[p->octave].Octave)[p->level]).Level, x_sample - 1, y_sample);
            float sample22 = image_process.getPixelBI(((GaussianPyr[p->octave].Octave)[p->level]).Level, x_sample, y_sample);
            float sample23 = image_process.getPixelBI(((GaussianPyr[p->octave].Octave)[p->level]).Level, x_sample + 1, y_sample);
            float sample32 = image_process.getPixelBI(((GaussianPyr[p->octave].Octave)[p->level]).Level, x_sample, y_sample + 1);
            //float diff_x = 0.5*(sample23 - sample21);
            //float diff_y = 0.5*(sample32 - sample12);
            float diff_x = sample23 - sample21;
            float diff_y = sample32 - sample12;
            float mag_sample = sqrt(diff_x*diff_x + diff_y * diff_y);
            float grad_sample = atan(diff_y / diff_x);
            if (grad_sample == CV_PI)
                grad_sample = -CV_PI;
            // Compute the weighting for the x and y dimensions.
            float *x_wght = (float *)malloc(GridSpacing * GridSpacing * sizeof(float));
            float *y_wght = (float *)malloc(GridSpacing * GridSpacing * sizeof(float));
            float *pos_wght = (float *)malloc(8 * GridSpacing * GridSpacing * sizeof(float));;
            for (int m = 0; m < 32; ++m, ++m)
            {
                float x = featcenter[m];
                float y = featcenter[m + 1];
                x_wght[m / 2] = max(1 - (fabs(x - x_sample)*1.0 / GridSpacing), 0.0);
                y_wght[m / 2] = max(1 - (fabs(y - y_sample)*1.0 / GridSpacing), 0.0);

            }
            for (int m = 0; m < 16; ++m)
                for (int n = 0; n < 8; ++n)
                    pos_wght[m * 8 + n] = x_wght[m] * y_wght[m];
            free(x_wght);
            free(y_wght);

            float diff[8], orient_wght[128];
            for (int m = 0; m < 8; ++m)
            {
                float angle = grad_sample - (p->ori) - orient_angles[m] + CV_PI;
                float temp = angle / (2.0 * CV_PI);
                angle -= (int)(temp) * (2.0 * CV_PI);
                diff[m] = angle - CV_PI;
            }
            // Compute the gaussian weighting.
            float x = p->sx;
            float y = p->sy;
            float g = exp(-((x_sample - x)*(x_sample - x) + (y_sample - y)*(y_sample - y)) / (2 * feat_window*feat_window)) / (2 * CV_PI*feat_window*feat_window);

            for (int m = 0; m < 128; ++m)
            {
                orient_wght[m] = max((1.0 - 1.0*fabs(diff[m % 8]) / orient_bin_spacing), 0.0);
                feat_desc[m] = feat_desc[m] + orient_wght[m] * pos_wght[m] * g*mag_sample;
            }
            //printf("%f  ", feat_desc[1]);
            free(pos_wght);
        }
        free(feat);
        free(featcenter);
        //printf("%f  ", feat_desc[1]);
        float norm = image_process.GetVecNorm(feat_desc, 128);
        //printf("%f  ", norm);
        for (int m = 0; m < 128; m++)
        {
            feat_desc[m] /= norm;
            if (feat_desc[m] > 0.2)
                feat_desc[m] = 0.2;
        }
        norm = image_process.GetVecNorm(feat_desc, 128);
        for (int m = 0; m < 128; m++)
        {
            feat_desc[m] /= norm;
            /*	printf("%f  ", feat_desc[m]);*/
            /*cout << typeid(feat_desc[m]).name() << " ";*/
        }
        //printf("/n");
        //cout << endl;
        p->descrip = feat_desc;
        p = p->next;
        count++;
    }
    cout << count << endl;
    free(feat_grid);
    free(feat_samples);
    return count;
}

