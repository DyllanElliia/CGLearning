#ifndef BFH
#define BFH
#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>
namespace Filters
{
namespace BilateralFilters
{
float avg(unsigned char a[9]) //计算平均数
{
    float temp = 0.0;
    for (int m = 0; m < 9; m++)
    {
        temp += a[m];
    }
    return temp;
}

float var(unsigned char b[9], float c) //计算方差
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
unsigned char BilateralFilters(unsigned char Picture_nine[9])
{
    #define shuzuMiddle 4
    float temp = 0;
    float average = avg(Picture_nine);
    float variance = var(Picture_nine,average);
    float root = sqrt(variance);
    if(variance<1)
        return Picture_nine[shuzuMiddle];
    float w = 0.0;
    float num1[9] = {1.414, 1, 1.414, 1, 0, 1, 1.414, 1, 1.414}; //位置信息
    int s = 0;
    for(int s = 0; s < 9; s++)
    {
        #define den1 0.182
        float e1 = exp(-num1[s]/den1);
        unsigned char   x = Picture_nine[shuzuMiddle],
                        y = Picture_nine[s];
        #define num2 pow(x<y? (y-x):(x-y),2)
        #define den2 variance
        float e2 = exp(-(num2)/(2*den2));
        float e = e1*e2;
        w += e;
        temp += e*y;
    }
    return temp/w;
}
} // namespace BilateralFilters
namespace medianFilters
{
bool cmp(unsigned char a,unsigned char b)
{
    return a>b?true:false;
}
unsigned char medianFilters(unsigned char Picture_nine[9])
{
    std::sort(&(Picture_nine[0]), &(Picture_nine[8]), cmp);
    return Picture_nine[4];
    /*
    if(!Picture_nine[4])
    //    return;
    if(Picture_nine[4]*2 > Picture_nine[7]&&Picture_nine[4]*2 > Picture_nine[1])
        return;
    unsigned char doubleAverage[4];
    doubleAverage[0] = Picture_nine[0] + Picture_nine[8];
    doubleAverage[1] = Picture_nine[1] + Picture_nine[7];
    doubleAverage[2] = Picture_nine[2] + Picture_nine[6];
    doubleAverage[3] = Picture_nine[3] + Picture_nine[5];
    unsigned char doubleFoue = 2*Picture_nine[4];
    for(int i = 0,count = 0; i < 4; i++)
    {
        if(doubleFoue>doubleAverage[i]-delta && doubleFoue<doubleAverage[i]+delta)
            count++;
        if(count==2)
            return;
    }
    sort(&(doubleAverage[0]),&(doubleAverage[3]),cmp);
    Picture_nine[4] = (doubleAverage[2]+doubleAverage[3])/4;*/
}
}
void Filters(unsigned char rgbPicture[], int weight, int height, int maxCount)
{
    //unsigned char *rgbPicture = *_rgbPicture;
    #define nchannels 3
    #define red 0
    #define gre 1
    #define blu 2
    for (int runTime = 0; runTime < maxCount; runTime++)
    {
        std::vector<unsigned char> picture[3];
        for (int i = 0, count = weight * height * 3; i < count;)
        {
            picture[red].push_back(rgbPicture[i++]);
            picture[gre].push_back(rgbPicture[i++]);
            picture[blu].push_back(rgbPicture[i++]);
        }
        #define levelMax 3
        unsigned char shuzu[levelMax*levelMax] = {};
        int _weight = weight-1;
        int _height = weight*height-weight;
        #define countM weight * height
        for (int i = 0; i < countM;i++)
        {
            //std::cout << weight << " " << height << std::endl;
            //std::cout << countM << std::endl;
            for(int col = 0; col < nchannels; col++)
            {
                //shuzu[4] = r_picture[i];
                for(int level_shuzu = -1; level_shuzu<levelMax; level_shuzu++)
                {
                    for (int j = i+level_shuzu*weight - 1, runJ = 0; runJ < 3; runJ++,j++)
                    {
                        int copy = j;
                        if (i % weight == 0)
                            copy++;
                        if (i % weight == _weight)
                            copy--;
                        if (i < height)
                            copy += weight;
                        if (i > _height)
                            copy -= weight;
                        shuzu[runJ+(level_shuzu+1)*levelMax] = picture[col][copy];
                    }

                }
                //std::cout << int(rgbPicture[i*3+col])<<" ";
                //if(flagSaltPepper)
                  //  SaltPepperSolution::JudgeAndSolveSPN(shuzu,60);
                if(runTime==0/*&&(shuzu[4]==0||shuzu[4]==255)*/)
                    rgbPicture[i*3+col] = medianFilters::medianFilters(shuzu);
                rgbPicture[i*3+col] = BilateralFilters::BilateralFilters(shuzu);
                //std::cout << int(rgbPicture[i * 3 + col]) << std::endl;
            }
        }
    }
}
}

#endif