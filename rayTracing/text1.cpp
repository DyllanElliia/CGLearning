#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_master/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_master/stb_image_write.h"
//#include"tif_RW.h"
//#include"tif.h"

int main()
{
    int nx = 0,ny=0,nn=0;
    unsigned char *data = stbi_load("picture/earth_map.jpg", &nx, &ny, &nn, 0);
    //std::cout <<data[1]<<std::endl;
    stbi_write_bmp("picture/flower.bmp",nx,ny,nn,data);
    //if(ImgToTIF("flower.tif",data,nx,ny,ns))
    system("pause");
}