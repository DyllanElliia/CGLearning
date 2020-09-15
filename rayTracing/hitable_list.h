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
    for(int i = 0; i < list_size; i++)//��һ�������壬�ж��Ƿ����
        if (list[i]->hit(r, tmin, closest_so_far, temp_rec))
        {   //��������� hit���ж��Ƿ���У������������� t
            //�����л򳤶ȳ���� t�����ж��¸������Ƿ񱻻���
            hit_anything = true;//��������
            closest_so_far = temp_rec.t;//����� t����ȷ�����е�����ӽ�������
            rec = temp_rec;
        }
    return hit_anything;
}
bool hitable_list::bounding_box(float t0, float t1, aabb &box)
    const
{
    if(list_size < 1)//������û������
        return false;
    aabb temp_box;
    bool first_true = list[0]->bounding_box(t0,t1,temp_box);//�Ե�һ������Ϊ��һ������
    if(!first_true)//���Ӵ���ʧ�ܣ���list�������
        return false;
    else
        box = temp_box;     //�洢��һ��box��Ϊ������ı��ϲ���box
    for(int i = 1; i < list_size; i++)
    {
        if(list[i]->bounding_box(t0,t1,temp_box))   //������i�������box
            box = surrounding_box(box,temp_box);    //�ϲ�box��temp_box
        else
            return false;
    }
    return true;
}
#endif