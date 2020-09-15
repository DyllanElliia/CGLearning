#ifndef WATERH
#define WATERH

#include "hitable.h"
#include "BVH.h"
#include "texture.h"
#include "drand48.h"
#include "box.h"
#include "vec3.h"
#include "polygon.h"
//#include <iostream>
#include <float.h>

class water : public hitable
{
    private:
        bvh_node *redWaterFace;
        bvh_node *blueWaterFace;
    public:
        water() {}
        water(vec3 highest, vec3 lowest, int faceNumber, float level);
        water(float x, float xx, float z, float zz, float y, float yy, int faceNumber, float level);
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec)
            const
        {
            return redWaterFace->hit(r,t_min,t_max,rec)?true:false;//blueWaterFace->hit(r,t_min,t_max,rec);
        }
        virtual bool bounding_box(float t0, float t1, aabb &box)
            const
        {
            aabb box1, box2;
            redWaterFace->bounding_box(t0,t1,box1);
            blueWaterFace->bounding_box(t0,t1,box2);
            box = aabb(vec3(box1.min()[0] < box2.min()[0] ? box1.min()[0] : box2.min()[0],
                            box1.min()[1] < box2.min()[1] ? box1.min()[1] : box2.min()[1],
                            box1.min()[2] < box2.min()[2] ? box1.min()[2] : box2.min()[2]),
                       vec3(box1.max()[0] > box2.max()[0] ? box1.max()[0] : box2.max()[0],
                            box1.max()[1] > box2.max()[1] ? box1.max()[1] : box2.max()[1],
                            box1.max()[2] > box2.max()[2] ? box1.max()[2] : box2.max()[2]));
            return true;
        }
};
water::water(vec3 highest, vec3 lowest, int faceNumber, float level)
{
    perlin noise;
    faceNumber = int(sqrt(faceNumber));
    float **hightData = new float*[faceNumber+1];//[faceNumber+1][faceNumber+1];
    for(int i = 0; i < faceNumber+1; hightData[i++] = new float[faceNumber+1]);
    float high = highest[1]-lowest[1];
    for(int u = 0; u <= faceNumber; u++)
        for(int v = 0; v <= faceNumber; v++)
            hightData[u][v] = high*(fabsf(noise.noise(level*vec3(float(u)/float(faceNumber),0,float(u)/float(faceNumber)))));
    hitable **rlist = new hitable*[(faceNumber)*(faceNumber)];
    hitable **blist = new hitable*[(faceNumber)*(faceNumber)];
    vec3 vertex[3];
    vec3 vertex_xz[2];
    vertex_xz[0] = lowest;
    vertex_xz[1] = {highest[0]-lowest[0],0,highest[2]-lowest[2]};
    float delta = 1/float(faceNumber);
    material *glass_ = new dielectric(new constant_texture(vec3(1,1,1)),1.33);
    int count_traingle = 0;
    for(int u = 0; u < faceNumber;u++)
        for(int v = 0; v < faceNumber; v++)
        {
            float uu = float(u)/float(faceNumber);
            float vv = float(v)/float(faceNumber);
            vertex[0]= {vertex_xz[0][0]+uu*         vertex_xz[1][0],
                        vertex_xz[0][1]+            hightData[u][v],
                        vertex_xz[0][2]+vv*         vertex_xz[1][2]};
            vertex[1]= {vertex_xz[0][0]+uu*         vertex_xz[1][0],
                        vertex_xz[0][1]+            hightData[u][v+1],
                        vertex_xz[0][2]+(vv+delta)* vertex_xz[1][2]};
            vertex[2]= {vertex_xz[0][0]+(uu+delta)* vertex_xz[1][0],
                        vertex_xz[0][1]+            hightData[u+1][v+1],
                        vertex_xz[0][2]+(vv+delta)* vertex_xz[1][2]};

            rlist[count_traingle++] = new triangle(vertex,new lambertian(new constant_texture(vec3(1,0,0))));

            vertex[0]= {vertex_xz[0][0]+uu*         vertex_xz[1][0],
                        vertex_xz[0][1]+            hightData[u][v],
                        vertex_xz[0][2]+vv*         vertex_xz[1][2]};
            vertex[1]= {vertex_xz[0][0]+(uu+delta)* vertex_xz[1][0],
                        vertex_xz[0][1]+            hightData[u+1][v+1],
                        vertex_xz[0][2]+(vv+delta)* vertex_xz[1][2]};
            vertex[2]= {vertex_xz[0][0]+(uu+delta)* vertex_xz[1][0],
                        vertex_xz[0][1]+            hightData[u+1][v],
                        vertex_xz[0][2]+vv*         vertex_xz[1][2]};

            blist[count_traingle++] = new triangle(vertex,new lambertian(new constant_texture(vec3(1,0,0))));
            //cout << vertex[0] << endl;
            //cout << vertex[1] << endl;
            //cout << vertex[2] << endl;
        }
    redWaterFace = new bvh_node(rlist, count_traingle,0,FLT_MAX);
    blueWaterFace = new bvh_node(blist, count_traingle, 0, FLT_MAX);
}

water::water(float x, float xx, float z, float zz, float y, float yy, int faceNumber, float level)
{
    perlin noise;
    faceNumber = int(sqrt(faceNumber));
    float **hightData = new float*[faceNumber+1];//[faceNumber+1][faceNumber+1];
    for(int i = 0; i < faceNumber+1; hightData[i++] = new float[faceNumber+1]);
    float high = yy-y;
    for(int u = 0; u <= faceNumber; u++)
        for(int v = 0; v <= faceNumber; v++)
            hightData[u][v] = high*(fabsf(noise.noise(level*vec3(float(u)/float(faceNumber),0,float(u)/float(faceNumber)))));
    hitable **rlist = new hitable*[(faceNumber)*(faceNumber)];
    hitable **blist = new hitable*[(faceNumber)*(faceNumber)];
    vec3 vertex[3];
    float delta = 1/float(faceNumber);
    material *glass_ = new dielectric(new constant_texture(vec3(1,1,1)),1.33);
    material *white_ = new lambertian(new constant_texture(vec3(1,0,0)));
    material *white1 = new lambertian(new constant_texture(vec3(1, 1, 0)));
    int count_traingle = 0;
    float delta_x = xx-x, delta_z = zz-z;
    for(int u = 0; u<faceNumber; u++)
        for(int v = faceNumber-1; v >=0; v--)
        {
            float uu = float(u)/float(faceNumber);
            float vv = float(v)/float(faceNumber);
            vertex[0]= {x+uu*           delta_x,
                        y+              hightData[u][v],
                        z+vv*           delta_z};
            vertex[1]= {x+(uu+delta)*   delta_x,
                        y+              hightData[u+1][v+1],
                        z+(vv+delta)*   delta_z};
            vertex[2]= {x+(uu+delta)*   delta_x,
                        y+              hightData[u+1][v],
                        z+vv*           delta_z};
            cout<<vertex[0]<<endl;
            cout<<vertex[1]<<endl;
            cout<<vertex[2]<<endl<<endl;
            rlist[count_traingle] = new triangle(vertex,white_);

            vertex[0]= {x+uu*           delta_x,
                        y+              hightData[u][v],
                        z+vv*           delta_z};
            vertex[1]= {x+uu*           delta_x,
                        y+              hightData[u][v+1],
                        z+(vv+delta)*   delta_z};
            vertex[2]= {x+(uu+delta)*   delta_x,
                        y+              hightData[u+1][v+1],
                        z+(vv+delta)*   delta_z};
            blist[count_traingle++] = new triangle(vertex,white1);
        }
    redWaterFace = new bvh_node(rlist, count_traingle, 0, FLT_MAX);
    blueWaterFace = new bvh_node(blist, count_traingle, 0, FLT_MAX);
}

#endif