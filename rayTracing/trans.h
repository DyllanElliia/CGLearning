#ifndef TRANSH
#define TRSNSH

#include "hitable_list.h"
#include "aabb.h"

class translate : public hitable
{
    private:
        hitable *ptr;
        vec3 offset;
    public:
        translate(hitable *p, const vec3 &displacement) : ptr(p), offset(displacement) {}
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
        virtual bool bounding_box(float t0, float t1, aabb &box) const;
        virtual float pdf_value(const vec3 &o, const vec3 &v) const;
        virtual vec3 random(const vec3 &o) const;
};

float translate::pdf_value(const vec3 &o, const vec3 &v)
    const
{
    vec3 move_o = o-offset;
    return ptr->pdf_value(move_o,v);
}

vec3 translate::random(const vec3 &o)
    const
{
    vec3 move_o = o-offset;
    return ptr->random(move_o);
}

bool translate::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if(ptr->hit(moved_r, t_min, t_max, rec))
    {
        rec.p += offset;
        return true;
    }else
        return false;
}

bool translate::bounding_box(float t0, float t1, aabb &box)
    const
{
    if(ptr->bounding_box(t0, t1, box))
    {
        box = aabb(box.min() + offset, box.max() + offset);
        return true;
    }else
        return false;
}

class rotate_x : public hitable
{
    private:
        hitable *ptr;
        float sin_theta;
        float cos_theta;
        bool hasbox;
        aabb bbox;
    public:
        rotate_x(hitable *p, float angle);
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
        virtual bool bounding_box(float t0, float t1, aabb &box) const
        { box = bbox; return hasbox;}
        virtual float pdf_value(const vec3 &o, const vec3 &v) const;
        virtual vec3 random(const vec3 &o) const;
};

float rotate_x::pdf_value(const vec3 &o, const vec3 &v)
    const
{
    vec3 move_o = o;
    vec3 move_v = v;
    move_o[1] = cos_theta*o[1] + sin_theta*o[2];
    move_o[2] = -sin_theta*o[1] + cos_theta*o[2];
    move_v[1] = cos_theta*v[1] + sin_theta*v[2];
    move_v[2] = -sin_theta*v[1] + cos_theta*v[2];
    return ptr->pdf_value(move_o, move_v);
}

vec3 rotate_x::random(const vec3 &o)
    const
{
    vec3 move_o = o;
    move_o[1] = cos_theta*o[1] + sin_theta*o[2];
    move_o[2] = -sin_theta*o[1] + cos_theta*o[2];
    vec3 random1 = ptr->random(move_o);
    move_o = random1;
    move_o[1] = cos_theta*random1[1] - sin_theta*random1[2];
    move_o[2] = sin_theta*random1[1] + cos_theta*random1[2];
    return move_o;
}

rotate_x::rotate_x(hitable *p, float angle)
    :ptr(p)
{
    float radians = (M_PI / 180.) * angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);
    vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 max = -min;
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            for(int k = 0; k < 2; k++)
            {
                float x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                float y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                float z = k * bbox.max().z() + (1 - k) * bbox.min().z();
                float newy = cos_theta * y - sin_theta * z;
                float newz = sin_theta * y + cos_theta * z;
                vec3 tester(x, newy, newz);
                for(int c = 0; c < 3; c++)
                {
                    if (tester[c] > max[c])
                        max[c] = tester[c];
                    if (tester[c] < min[c])
                        min[c] = tester[c];
                }
            }
    bbox = aabb(min,max);
}

bool rotate_x::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[1] = cos_theta*r.origin()[1] + sin_theta*r.origin()[2];
    origin[2] = -sin_theta*r.origin()[1] + cos_theta*r.origin()[2];
    direction[1] = cos_theta*r.direction()[1] + sin_theta*r.direction()[2];
    direction[2] = -sin_theta*r.direction()[1] + cos_theta*r.direction()[2];
    ray rotate_ray(origin, direction, r.time());
    if(ptr->hit(rotate_ray, t_min, t_max, rec))
    {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[1] = cos_theta*rec.p[1] - sin_theta*rec.p[2];
        p[2] = sin_theta*rec.p[1] + cos_theta*rec.p[2];
        normal[1] = cos_theta*rec.normal[1] - sin_theta*rec.normal[2];
        normal[2] = sin_theta*rec.normal[1] + cos_theta*rec.normal[2];
        rec.p = p;
        rec.normal = normal;
        return true;
    }else
        return false;
}

class rotate_y : public hitable
{
    private:
        hitable *ptr;
        float sin_theta;
        float cos_theta;
        bool hasbox;
        aabb bbox;
    public:
        rotate_y(hitable *p, float angle);
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
        virtual bool bounding_box(float t0, float t1, aabb &box) const
        { box = bbox; return hasbox;}
        virtual float pdf_value(const vec3 &o, const vec3 &v) const;
        virtual vec3 random(const vec3 &o) const;
};

float rotate_y::pdf_value(const vec3 &o, const vec3 &v)
    const
{
    vec3 move_o = o;
    vec3 move_v = v;
    move_o[0] = cos_theta*o[0] - sin_theta*o[2];
    move_o[2] = sin_theta*o[0] + cos_theta*o[2];
    move_v[0] = cos_theta*v[0] - sin_theta*v[2];
    move_v[2] = sin_theta*v[0] + cos_theta*v[2];
    return ptr->pdf_value(move_o, move_v);
}

vec3 rotate_y::random(const vec3 &o)
    const
{
    vec3 move_o = o;
    move_o[0] = cos_theta*o[0] - sin_theta*o[2];
    move_o[2] = sin_theta*o[0] + cos_theta*o[2];
    vec3 random1 = ptr->random(move_o);
    move_o = random1;
    move_o[0] = cos_theta*random1[0] + sin_theta*random1[2];
    move_o[2] = -sin_theta*random1[0] + cos_theta*random1[2];
    return move_o;
}

rotate_y::rotate_y(hitable *p, float angle)
    :ptr(p)
{
    float radians = (M_PI / 180.) * angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);
    vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 max = -min;
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            for(int k = 0; k < 2; k++)
            {
                float x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                float y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                float z = k * bbox.max().z() + (1 - k) * bbox.min().z();
                float newx = cos_theta * x + sin_theta * z;
                float newz = -sin_theta * x + cos_theta * z;
                vec3 tester(newx, y, newz);
                for(int c = 0; c < 3; c++)
                {
                    if (tester[c] > max[c])
                        max[c] = tester[c];
                    if (tester[c] < min[c])
                        min[c] = tester[c];
                }
            }
    bbox = aabb(min,max);
}

bool rotate_y::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];
    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];
    ray rotate_ray(origin, direction, r.time());
    if(ptr->hit(rotate_ray, t_min, t_max, rec))
    {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[0] = cos_theta*rec.p[0] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];
        normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];
        rec.p = p;
        rec.normal = normal;
        return true;
    }else
        return false;
}

class rotate_z : public hitable
{
    private:
        hitable *ptr;
        float sin_theta;
        float cos_theta;
        bool hasbox;
        aabb bbox;
    public:
        rotate_z(hitable *p, float angle);
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
        virtual bool bounding_box(float t0, float t1, aabb &box) const
        { box = bbox; return hasbox;}
        virtual float pdf_value(const vec3 &o, const vec3 &v) const;
        virtual vec3 random(const vec3 &o) const;
};

float rotate_z::pdf_value(const vec3 &o, const vec3 &v)
    const
{
    vec3 move_o = o;
    vec3 move_v = v;
    move_o[0] = cos_theta*o[0] + sin_theta*o[1];
    move_o[1] = -sin_theta*o[0] + cos_theta*o[1];
    move_v[0] = cos_theta*v[0] + sin_theta*v[1];
    move_v[1] = -sin_theta*v[0] + cos_theta*v[1];
    return ptr->pdf_value(move_o, move_v);
}

vec3 rotate_z::random(const vec3 &o)
    const
{
    vec3 move_o = o;
    move_o[0] = cos_theta*o[0] + sin_theta*o[1];
    move_o[1] = -sin_theta*o[0] + cos_theta*o[1];
    vec3 random1 = ptr->random(move_o);
    move_o = random1;
    move_o[0] = cos_theta*random1[0] - sin_theta*random1[1];
    move_o[1] = sin_theta*random1[0] + cos_theta*random1[1];
    return move_o;
}

rotate_z::rotate_z(hitable *p, float angle)
    :ptr(p)
{
    float radians = (M_PI / 180.) * angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);
    vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 max = -min;
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            for(int k = 0; k < 2; k++)
            {
                float x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                float y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                float z = k * bbox.max().z() + (1 - k) * bbox.min().z();
                float newx = cos_theta * x - sin_theta * y;
                float newy = sin_theta * x + cos_theta * y;
                vec3 tester(newx, newy, z);
                for(int c = 0; c < 3; c++)
                {
                    if (tester[c] > max[c])
                        max[c] = tester[c];
                    if (tester[c] < min[c])
                        min[c] = tester[c];
                }
            }
    bbox = aabb(min,max);
}

bool rotate_z::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[0] = cos_theta*r.origin()[0] + sin_theta*r.origin()[1];
    origin[1] = -sin_theta*r.origin()[0] + cos_theta*r.origin()[1];
    direction[0] = cos_theta*r.direction()[0] + sin_theta*r.direction()[1];
    direction[1] = -sin_theta*r.direction()[0] + cos_theta*r.direction()[1];
    ray rotate_ray(origin, direction, r.time());
    if(ptr->hit(rotate_ray, t_min, t_max, rec))
    {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[0] = cos_theta*rec.p[0] - sin_theta*rec.p[1];
        p[1] = sin_theta*rec.p[0] + cos_theta*rec.p[1];
        normal[0] = cos_theta*rec.normal[0] - sin_theta*rec.normal[1];
        normal[1] = sin_theta*rec.normal[0] + cos_theta*rec.normal[1];
        rec.p = p;
        rec.normal = normal;
        return true;
    }else
        return false;
}

hitable *free_translate(hitable *ptr, string number,vec3 angle, vec3 trans)
{
    short count = number.length()-1;
    while(count >= 0 )
        switch(number[count--])
        {
            case 'x':
            case 'X':
                if(angle.x() != 0)
                    ptr = new rotate_x(ptr, angle.x());
                break;
            case 'y':
            case 'Y':
                if(angle.y() != 0)
                    ptr = new rotate_y(ptr, angle.y());
                break;
            case 'z':
            case 'Z':
                if(angle.z() != 0)
                    ptr = new rotate_z(ptr, angle.z());
                break;
            default:
                cout << count + 1<<number[count+1];
                cout << endl;
                break;
        }
    if(trans.x()!=0||trans.y()!=0||trans.z()!=0)
        ptr = new translate(ptr, trans);
    return ptr;
}

#endif