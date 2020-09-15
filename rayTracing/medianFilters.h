#ifndef MFH
#define MFH

#include <vector>
#include <iostream>
#include <algorithm>

namespace medianFilters
{
namespace private_
{
bool cmp(unsigned char a, unsigned char b)
{
    return a > b ? true : false;
}
unsigned char findMid(unsigned char Picture_nine[9])
{
    std::sort(&(Picture_nine[0]),&(Picture_nine[8]),cmp);
    return Picture_nine[4];
}
}

void medianFilters(unsigned char rgbPicture[], int weight, int height)
{
    #define nchannels 3
    #define red 0
    #define gre 1
    #define blu 2
    #define countM weight *height
    std::vector<unsigned char> picture[3];
    for (int i = 0; i < countM*3;)
    {
        picture[red].push_back(rgbPicture[i++]);
        picture[gre].push_back(rgbPicture[i++]);
        picture[blu].push_back(rgbPicture[i++]);
    }
    #define levelMax 3
    unsigned char shuzu[levelMax * levelMax] = {};
    int _weight = weight - 1;
    int _height = weight * height - weight;
    for (int i = 0; i < countM; i++)
    {
        //std::cout << weight << " " << height << std::endl;
        //std::cout << countM << std::endl;
        try
        {
            unsigned char text = 0;
            text += picture[red][i];
            text += picture[blu][i];
            text += picture[gre][i];
            if(!picture[red][i]&&!picture[gre][i]&&!picture[blu][i])
                for (int col = 0; col < nchannels; col++)
                {/*
                    switch (col)
                    {
                    case 0:
                        std::cout << "red ";
                        break;
                    case 1:
                        std::cout << "gre ";
                        break;
                    case 2:
                        std::cout << "blu ";
                        break;
                    default:
                        break;
                    }
                    if(i==2257)
                    system("pause");
                    std::cout << int(picture[col][i]) << ":\n";
                    //if (!picture[col][i] && picture[col][i] != (unsigned char)255)
                    //  continue;
                    std::cout << int(rgbPicture[i * 3 + col]) << "->";
                    //shuzu[4] = r_picture[i];
                    */
                    for (int level_shuzu = -1; level_shuzu < levelMax; level_shuzu++)
                    {
                        for (int j = i + level_shuzu * weight - 1, runJ = 0; runJ < 3; runJ++, j++)
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
                            shuzu[runJ + (level_shuzu + 1) * levelMax] = picture[col][copy];
                        }
                    }
                    rgbPicture[i * 3 + col] = private_::findMid(shuzu);
                    std::cout << int(rgbPicture[i * 3 + col]) << std::endl;
                }
        }
        catch(std::exception& e)
        {
            std::cout << i<<std::endl;
            try
            {
                if(!picture[red][i]);
            }
            catch(...)
            {
                std::cout<<"redWoring"<<std::endl;
                picture[red][i] = 0;
            }
                        try
            {
                if(!picture[gre][i]);
            }
            catch(...)
            {
                std::cout<<"greenWoring"<<std::endl;
                picture[red][i] = 0;
            }
            try
            {
                if(!picture[blu][i]);
            }
            catch(...)
            {
                std::cout<<"blueWoring"<<std::endl;
                picture[red][i] = 0;
            }
            --i;
        }
    }
}
}
#endif