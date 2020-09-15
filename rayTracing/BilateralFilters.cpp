
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include "opencv2/opencv.hpp"

float avg(float *a) //����ƽ����
{
    float temp = 0.0;
    for (int m = 0; m < 9; m++)
    {
        temp += a[m];
    }
    return temp;
}

float var(float *b, float c) //���㷽��
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

    //�߽���չ
    int z = 0;
    while (z < 30) //����30��ѭ��������һ��˫���˲�Ч������
    {
        //IplImage *extensionimage = cvCreateImage(cvSize(inputimage->width + 2, inputimage->height + 2), IPL_DEPTH_8U, inputimage->nChannels);
        //IplImage *smoothimage = cvCreateImage(cvSize(inputimage->width, inputimage->height), IPL_DEPTH_8U, inputimage->nChannels);
        cvCopyMakeBorder(inputimage, extensionimage, cvPoint(1, 1), IPL_BORDER_REPLICATE); //���б߽���չ��ԭͼ������ͼ��ģ�1��1��λ�ã�IPL_BORDER_REPLICATE��ʾ��չΪ�߽�����

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
                    average = avg(shuzu);           //�����ֵ
                    variance = var(shuzu, average); //���㷽��
                    float root = sqrt(variance);
                    if (variance < 1) //��ֹ�������ؼ�ķ����С����ɴ���
                    {
                        smoothimage->imageData[i * smoothimage->widthStep + j * smoothimage->nChannels + k] = (unsigned char)extensionimage->imageData[(i + 1) * extensionimage->widthStep + (j + 1) * extensionimage->nChannels + k];
                        //printf("%f\n", variance);
                    }
                    else
                    {
                        float temp = 0.0;
                        float w = 0.0;

                        float num1[9] = {1.414, 1, 1.414, 1, 0, 1, 1.414, 1, 1.414}; //λ����Ϣ
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
                                num2 = x < y ? (y - x) : (x - y); //������Ϣת��Ϊ�޷����ַ���0-255֮�䣬��֤��ֵΪ����
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
