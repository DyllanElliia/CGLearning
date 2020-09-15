#include "vec3.h"
#include <iostream>
#include "drand48.h"
#include "svpng.h"
#include <cstdio>
#define nx 800
#define ny 800

/*inline vec3 random()
{
    vec3 size = vec3(drand48(),drand48(),drand48());
    size /= size[0]+size[1]+size[2];
    return size;
}*/

int main()
{
    unsigned char p[nx*ny*3];
    char name[] = "triangle.png";
    for(int i = 0;i < nx; i++)
        for(int j = 0;j < ny; j++)
            for(int size = 0; size < 3; size++)
                p[i*ny*3+j*3+size] = 0;
    vec3 to[3] = {vec3(0,350,0),vec3(-350,-350,0),vec3(350,-350,0)};
    //vec3 size_;
    for(int i = 0; i < 10000; i++)
    {
        float r1 = drand48();
        float r2 = drand48();
        vec3 now_ = ((1-sqrt(r1))*to[0]+sqrt(r1)*(1-r2)*to[1]+sqrt(r1)*r2*to[2]);
        p[int(now_.x()+400)*ny*3+int(now_.y()+400)*3+1] = 255;
    }
    /*for(int i = 0; i < 10000; i++)
    {
        size_ = random();
        now_ = vec3()+size_.x()*to[0]+size_.y()*to[1]+size_.z()*to[2];
        p[int(now_.x()+400)*ny*3+int(now_.y()+400)*3+1] = 255;
    }*/
    FILE *fp = fopen(name, "wb");
    svpng(fp, nx, ny, p, 0);
    fclose(fp);
}