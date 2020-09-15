
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include "opencv2/opencv.hpp"

float avg(float *a) //计算平均数
{
    float temp = 0.0;
    for (int m = 0; m < 9; m++)
    {
        temp += a[m];
    }
    return temp;
}

float var(float *b, float c) //计算方差
{
    float temp = 0.0;
    for (int m = 0; m < 9; m++)
    {
        float a = 0;
        a = b[m] > c ? (b[m] - c) : (c - b[m]);
        temp = temp + a * a;
    }
    return temp / 9.0;
}

void BilateralFilters(const char &**picture, int height, int width)
{
    // bilateral filter
    const char *filename = "building.bmp";
    IplImage *inputimage = cvLoadImage(filename, -1);
    IplImage *smoothimage = cvCreateImage(cvSize(inputimage->width, inputimage->height), IPL_DEPTH_8U, inputimage->nChannels);
    IplImage *extensionimage = cvCreateImage(cvSize(inputimage->width + 2, inputimage->height + 2), IPL_DEPTH_8U, inputimage->nChannels);

    //边界扩展
    int z = 0;
    while (z < 30) //进行30次循环，进行一次双边滤波效果不大
    {
        //IplImage *extensionimage = cvCreateImage(cvSize(inputimage->width + 2, inputimage->height + 2), IPL_DEPTH_8U, inputimage->nChannels);
        //IplImage *smoothimage = cvCreateImage(cvSize(inputimage->width, inputimage->height), IPL_DEPTH_8U, inputimage->nChannels);
        cvCopyMakeBorder(inputimage, extensionimage, cvPoint(1, 1), IPL_BORDER_REPLICATE); //进行边界扩展，原图像在新图像的（1，1）位置，IPL_BORDER_REPLICATE表示扩展为边界像素

        for (int i = 0; i < smoothimage->height; i++)
        {
            for (int j = 0; j < smoothimage->width; j++)
            {
                for (int k = 0; k < smoothimage->nChannels; k++)
                {
                    float average, variance = 0;
                    float shuzu[9] = {};
                    int pos = 0;
                    for (int m = -1; m < 2; m++)
                    {
                        for (int n = -1; n < 2; n++)
                        {
                            shuzu[pos++] = (unsigned char)extensionimage->imageData[(i + 1 + m) * extensionimage->widthStep + (j + 1 + n) * extensionimage->nChannels + k] / 9;
                        }
                    }
                    average = avg(shuzu);           //计算均值
                    variance = var(shuzu, average); //计算方差
                    float root = sqrt(variance);
                    if (variance < 1) //防止相似像素间的方差过小，造成错误
                    {
                        smoothimage->imageData[i * smoothimage->widthStep + j * smoothimage->nChannels + k] = (unsigned char)extensionimage->imageData[(i + 1) * extensionimage->widthStep + (j + 1) * extensionimage->nChannels + k];
                        //printf("%f\n", variance);
                    }
                    else
                    {
                        float temp = 0.0;
                        float w = 0.0;

                        float num1[9] = {1.414, 1, 1.414, 1, 0, 1, 1.414, 1, 1.414}; //位置信息
                        int s = 0;

                        for (int m = -1; m < 2; m++)
                        {
                            for (int n = -1; n < 2; n++)
                            {
                                float e, e1, e2, den1, num2, den2;
                                //num1 = (m ^ 2 + n ^ 2) ^ (1/2);
                                den1 = 0.182;
                                e1 = exp(-num1[s++] / den1);
                                unsigned char x, y;
                                x = (unsigned char)extensionimage->imageData[(i + 1) * extensionimage->widthStep + (j + 1) * extensionimage->nChannels + k];
                                y = (unsigned char)extensionimage->imageData[(i + 1 + m) * extensionimage->widthStep + (j + 1 + n) * extensionimage->nChannels + k];
                                num2 = x < y ? (y - x) : (x - y); //像素信息转换为无符号字符型0-255之间，保证差值为整数
                                num2 = pow(num2, 2);
                                den2 = variance;
                                e2 = exp(-num2 / (2 * den2));
                                e = e1 * e2;
                                w += e;
                                temp += e * (unsigned char)extensionimage->imageData
                                                [(i + 1 + m) * extensionimage->widthStep + (j + 1 + n) * extensionimage->nChannels + k];
                            }
                        }

                        smoothimage->imageData[i * smoothimage->widthStep + j * smoothimage->nChannels + k] = temp / w;
                    }
                }
            }
        }

        for (int i = 0; i < smoothimage->height; i++)
        {
            for (int j = 0; j < smoothimage->width; j++)
            {
                for (int k = 0; k < smoothimage->nChannels; k++)
                {
                    inputimage->imageData[i * inputimage->widthStep + j * inputimage->nChannels + k] = smoothimage->imageData[i * smoothimage->widthStep + j * smoothimage->nChannels + k];
                }
            }
        }
        z++;
    }
    cvNamedWindow("inputimage", 1);
    cvNamedWindow("smoothimage", 1);

    cvShowImage("inputimage", inputimage);
    cvShowImage("smoothimage", smoothimage);

    cvWaitKey(0);

    cv::Mat smooth = cv::cvarrToMat(smoothimage);
    cv::imwrite("buildingsmooth.bmp", smooth);
    /*const char *file = "G:\\flower_filter.bmp";
	cvSaveImage(file, smoothimage);*/

    cvDestroyWindow("inputimage");
    cvDestroyWindow("smoothimage");

    cvReleaseImage(&inputimage);
    cvReleaseImage(&smoothimage);
}
