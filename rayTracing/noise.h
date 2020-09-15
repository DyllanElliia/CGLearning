#ifndef NOISE
#define NOISE

#include "vec3.h"
#include "drand48.h"

inline float trilinear_interp(float c[2][2][2], float u, float v, float w)
{
    float accum = 0;
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            for(int k = 0; k < 2; k++)
                accum +=    (i*u + (1-i)*(1-u))*
                            (j*v + (1-j)*(1-v))*
                            (k*w + (1-k)*(1-w))*c[i][j][k];
    return accum;
}

inline float perlin_interp(vec3 c[2][2][2], float u, float v, float w)
{
    float accum = 0;
    float uu = u*u*(3-2*u);
    float vv = v*v*(3-2*v);
    float ww = w*w*(3-2*w);
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            for(int k = 0; k < 2; k++)
            {
                vec3 weight_v(u-i,v-j, w-k);
                accum +=    (i*uu + (1-i)*(1-uu))*
                            (j*vv + (1-j)*(1-vv))*
                            (k*ww + (1-k)*(1-ww))*dot(c[i][j][k], weight_v);
            }
    return accum;
}

class perlin
{
    private:
        static vec3 *ranvec;
        static int *perm_x;
        static int *perm_y;
        static int *perm_z;
    public:
        inline float noise(const vec3 &p)//输入光在球上的坐标
            const
        {
            //网格化，使其uvw均为整数，用于插值
            float u = p.x() - floor(p.x());
            float v = p.y() - floor(p.y());
            float w = p.z() - floor(p.z());
            //直接使用uvw会存在较明显的网格状
            /*u = u*u*(3-2*u);
            v = v*v*(3-2*v);
            w = w*w*(3-2*w); */

            // 取p向量的小数部分
            int i = floor(p.x());
            int j = floor(p.y());
            int k = floor(p.z());
            vec3 c[2][2][2];    //
            for(int di = 0; di < 2; di++)
                for(int dj = 0; dj < 2; dj++)
                    for(int dk = 0; dk < 2; dk++)
                        c[di][dj][dk] = ranvec[perm_x[(i+di) & 255] ^ perm_y[(j+dj) & 255] ^ perm_z[(k+dk) & 255]];
                        //与255，防止数据溢出；然后异或，得到 ranvec坐标，赋予随机的单位向量
            return perlin_interp(c, u, v, w);
        }
        inline float _turb(const vec3 &p, int depth = 7)
            const
        {
            float accum = 0;
            float weight = 1.0;
            vec3 t = p;
            for(int i = 0; i < depth; i++)
            {
                accum +=weight * noise(t);
                weight *=0.5;
                t *= 2;
            }
            return abs(accum);
        }
};

inline static float* perlin_generate_old()
{
    float *p = new float[256];
    for(int i = 0; i < 256; i++)
        p[i] = drand48();
    return p;
}

inline static vec3 *perlin_generate()
{
    vec3 *p = new vec3[256];    //亮度
    for (int i = 0; i < 256; i++)
        p[i] = unit_vector(vec3(-1+2*drand48(),-1+2*drand48(),-1+2*drand48()));
        //在空间内产生256组随机单位向量
    return p;
}

inline void permute(int *p, int n)
{
    for(int i = n-1; i > 0; i--)
    {
        //随机和一个低位的进行交换
        int target = int(drand48()*(i+1));
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
}

inline static int* perlin_generate_perm()
{
    int *p = new int[256];
    for(int i = 0; i < 256; i++)
        p[i] = i;
    permute(p, 256);    //低位打乱
    return p;
}

vec3 *perlin::ranvec = perlin_generate();
int *perlin::perm_x = perlin_generate_perm();
int *perlin::perm_y = perlin_generate_perm();
int *perlin::perm_z = perlin_generate_perm();

#endif