#ifndef BOXH
#define BOXH

#include "hitable.h"
#include "hitable_list.h"
#include "aabb.h"
#include "plane.h"

class box : public hitable
{
    public:
        box() {}
        box(const vec3 &pointmin, const vec3 &pointmax, material *mat);
        virtual bool hit(const ray &sight, float t_min, float t_max, hit_record &info) const override;
        virtual bool bounding_box(float t0, float t1, aabb &box)
            const
        {
            box = aabb(_min,_max);
            return true;
        }
        virtual float pdf_value(const vec3 &o, const vec3 &v)
            const
        {
            return list_ptr->pdf_value(o,v);
        }
        virtual vec3 random(const vec3 &o)
            const
        {
            return list_ptr->random(o);
        }
    private:
        vec3 _min;
        vec3 _max;
        hitable *list_ptr;
};

// the implementation of box class

inline box::box(const vec3 &pointmin, const vec3 &pointmax, material *mat)
    : _min(pointmin), _max(pointmax)
{
    hitable **list = new hitable *[6];
    list[0] = new xy_rect(_min.x(), _max.x(), _min.y(), _max.y(), _max.z(), mat);
    list[1] = new flip_normals(new xy_rect(_min.x(), _max.x(), _min.y(), _max.y(), _min.z(), mat));
    list[2] = new xz_rect(_min.x(), _max.x(), _min.z(), _max.z(), _max.y(), mat);
    list[3] = new flip_normals(new xz_rect(_min.x(), _max.x(), _min.z(), _max.z(), _min.y(), mat));
    list[4] = new yz_rect(_min.y(), _max.y(), _min.z(), _max.z(), _max.x(), mat);
    list[5] = new flip_normals(new yz_rect(_min.y(), _max.y(), _min.z(), _max.z(), _min.x(), mat));
    list_ptr = new hitable_list(list, 6);
}

bool box::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    return list_ptr->hit(r, t_min, t_max, rec);
}

#endif