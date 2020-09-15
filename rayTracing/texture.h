#ifndef TEXTURE
#define TEXTURE

#include "vec3.h"
#include "drand48.h"
#include "noise.h"
//#include "hitable.h"


class texture
{
    public:
        virtual vec3 value(float u, float v,const vec3 &p)const = 0;
};

//该贴图为一个纯色的贴图
class constant_texture : public texture
{
    public:
        vec3 color;
        constant_texture(){}
        constant_texture(vec3 c): color(c) {}
        virtual vec3 value(float u, float v, const vec3 &p)
            const
        {
            return color;
        }
};

//一个和位置相关的斑格图
class checker_texture : public texture
{
    public:
        texture *even;  //贴图1
        texture *odd;   //贴图2
        checker_texture(){}
        checker_texture(texture *t0, texture *t1): even(t0), odd(t1) {}
        virtual vec3 value(float u, float v, const vec3 &p)
            const
        {
            //位置向量u，v；球向量p
            float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
            if(sines < 0)
                return odd->value(u,v,p);
            else
                return even->value(u,v,p);
        }
};

class noise_sea_texture : public texture
{
    public:
        perlin noise;   //set a noise texture
        float scale;    //频率
        noise_sea_texture() {}
        noise_sea_texture(float sc) : scale(sc) {}
        virtual vec3 value(float u, float v, const vec3 &p) const
        { return vec3(1, 1, 1) * (1 - fabsf(noise.noise(scale * p))); }
};

class noise_marble1_texture : public texture
{
    public:
        perlin noise;   //set a noise texture
        float scale;    //频率
        noise_marble1_texture() {}
        noise_marble1_texture(float sc) : scale(sc) {}
        virtual vec3 value(float u, float v, const vec3 &p) const
        { return vec3(1, 1, 1) * 0.5*(1+noise._turb(scale * p)); }
};

class noise_marble2_texture : public texture
{
    public:
        perlin noise;   //set a noise texture
        float scale;    //频率
        noise_marble2_texture() {}
        noise_marble2_texture(float sc) : scale(sc) {}
        virtual vec3 value(float u, float v, const vec3 &p) const
        { return vec3(1, 1, 1) * 0.5*(1+sin(scale * p.z() + 10 * noise._turb(p))); }
};

class noise_texture : public texture
{
    public:
        perlin noise;   //set a noise texture
        float scale;    //频率
        noise_texture() {}
        noise_texture(float sc) : scale(sc) {}
        virtual vec3 value(float u, float v, const vec3 &p) const
        { return vec3(1, 1, 1) * 0.5*(1+noise.noise(scale * p)); }
};

class image_texture : public texture
{
    private:
        unsigned char *data;
        int nx;
        int ny;
    public:
        image_texture() {}
        image_texture(unsigned char *pixels, int a, int b) : data(pixels), nx(a), ny(b) {}
        virtual vec3 value(float u, float v, const vec3 &p)
            const
        {
            int i = (  u)*nx;
            int j = (1-v)*ny - 0.001;
            if(i<0)i = 0;
            if(j<0)j = 0;
            if(i>nx-1)i = nx-1;
            if(j>ny-1)j = ny-1;
            float r = int(data[3*i + 3*nx*j    ])/255.0;
            float g = int(data[3*i + 3*nx*j + 1])/255.0;
            float b = int(data[3*i + 3*nx*j + 2])/255.0;
            return vec3(r,g,b);
        }
};

#endif