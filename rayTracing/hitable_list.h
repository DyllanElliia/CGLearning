#ifndef HITABLELIST
#define HITABLELIST

#include "hitable.h"
#include "aabb.h"

class flip_normals : public hitable
{
    public:
        hitable *ptr;
        flip_normals(hitable *p) : ptr(p) {}
        virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec)
            const
        {
            if(ptr->hit(r,tmin,tmax,rec))
            {
                rec.normal = -rec.normal;
                return true;
            }else{
                return false;
            }
        }
        virtual bool bounding_box(float t0, float t1, aabb &box) const
        { return ptr->bounding_box(t0,t1,box); }
        virtual float pdf_value(const vec3 &o, const vec3 &v)
            const
        { return ptr->pdf_value(o,v);}
        virtual vec3 random(const vec3 &o)
            const
        { return ptr->random(o);}
};

class hitable_list:public hitable
{
    public:
        hitable **list;
        int list_size;
        hitable_list() {}
        hitable_list(hitable **l, int n) { list = l; list_size = n; }
        virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
        //virtual material *return_material();
        virtual bool bounding_box(float t0, float t1, aabb &box) const;
        virtual float pdf_value(const vec3 &o, const vec3 &v) const;
        virtual vec3 random(const vec3 &o) const;
};

float hitable_list::pdf_value(const vec3 &o, const vec3 &v) const
{
    float weight = 1.0 / list_size;
    float sum = 0;
    for (int i = 0; i < list_size; i++)
        sum += weight * list[i]->pdf_value(o, v);
    return sum;
}

vec3 hitable_list::random(const vec3 &o) const
{
    int index;
    do
    {
        index = int(drand48() * list_size);
    } while (index >= list_size);
    return list[index]->random(o);
}

bool hitable_list::hit(const ray &r, float tmin, float tmax, hit_record &rec)
    const
{
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = tmax;
    for(int i = 0; i < list_size; i++)//逐一遍历物体，判断是否击中
        if (list[i]->hit(r, tmin, closest_so_far, temp_rec))
        {   //调用物体的 hit，判断是否击中，若击中则更新最长 t
            //若不中或长度超过最长 t，则判断下个物体是否被击中
            hit_anything = true;//击中物体
            closest_so_far = temp_rec.t;//更新最长 t，以确保击中的是最接近的物体
            rec = temp_rec;
        }
    return hit_anything;
}
bool hitable_list::bounding_box(float t0, float t1, aabb &box)
    const
{
    if(list_size < 1)//箱子中没有物体
        return false;
    aabb temp_box;
    bool first_true = list[0]->bounding_box(t0,t1,temp_box);//以第一个物体为第一个盒子
    if(!first_true)//盒子创建失败，即list输入出错
        return false;
    else
        box = temp_box;     //存储第一个box作为最基础的被合并的box
    for(int i = 1; i < list_size; i++)
    {
        if(list[i]->bounding_box(t0,t1,temp_box))   //创建第i个物体的box
            box = surrounding_box(box,temp_box);    //合并box和temp_box
        else
            return false;
    }
    return true;
}
#endif