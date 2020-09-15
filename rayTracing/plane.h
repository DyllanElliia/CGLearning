#ifndef PLANE
#define PLANE

//it just mt test
#include "hitable.h"
#include "aabb.h"

class xy_rect :public hitable
{
    public:
        float x0;
        float x1;
        float y0;
        float y1;
        float k_;
        material *mp;

        xy_rect() {}
        xy_rect(float _x0, float _x1, float _y0, float _y1, float _k_, material *mat):
            x0(_x0<_x1?_x0:_x1),
            x1(_x0>_x1?_x0:_x1),
            y0(_y0<_y1?_y0:_y1),
            y1(_y0>_y1?_y0:_y1),
            k_(_k_),
            mp(mat) {}
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
        virtual bool bounding_box(float t0, float t1, aabb &box)
            const
        {
            box = aabb(vec3(x0, y0, k_ - 0.0001), vec3(x1, y1, k_ + 0.0001));
            return true;
        }
        virtual float pdf_value(const vec3 &o, const vec3 &v)
            const
        {
            hit_record rec;
            if (this->hit(ray(o, v), 1e-3, FLT_MAX, rec))
            {
                float area = (x1 - x0) * (y1 - y0);
                float distance_squared = rec.t * rec.t * v.squared_length();
                float cosine = fabs(dot(v, rec.normal) / v.length());
                return distance_squared / (cosine * area);
            }
            else
                return 0.;
        }
        virtual vec3 random(const vec3 &o)
            const
        {
            vec3 random_point = vec3(x0 + drand48() * (x1 - x0), y0 + drand48() * (y1 - y0), k_);
            return random_point - o;
        }
};

bool xy_rect::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    float t = (k_-r.origin().z())/(r.direction().z());
    if(t < t_min || t > t_max)
        return false;
    float x = r.origin().x() + t*r.direction().x();
    float y = r.origin().y() + t*r.direction().y();
    if(x < x0 || x > x1 || y < y0 || y > y1)
        return false;
    rec.hit_u = (x-x0)/(x1-x0);
    rec.hit_v = (y-y0)/(y1-y0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.poin_at_parameter(t);
    //rec.normal = dot(r.direction(),vec3(0,0,1))>0? vec3(0,0,-1):vec3(0,0,1);
    rec.normal = vec3(0,0,1);
    return true;
}

class xz_rect :public hitable
{
    public:
        float x0;
        float x1;
        float z0;
        float z1;
        float k_;
        material *mp;

        xz_rect() {}
        xz_rect(float _x0, float _x1, float _z0, float _z1, float _k_, material *mat):
            x0(_x0<_x1?_x0:_x1),
            x1(_x0>_x1?_x0:_x1),
            z0(_z0<_z1?_z0:_z1),
            z1(_z0>_z1?_z0:_z1),
            k_(_k_),
            mp(mat) {}
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
        virtual bool bounding_box(float t0, float t1, aabb &box)
            const
        {
            box = aabb(vec3(x0, k_ - 0.0001, z0), vec3(x1, k_ + 0.0001, z1));
            return true;
        }
        virtual float pdf_value(const vec3 &o, const vec3 &v)
            const
        {
            hit_record rec;
            if (this->hit(ray(o, v), 1e-3, FLT_MAX, rec))
            {
                float area = (x1 - x0) * (z1 - z0);
                float distance_squared = rec.t * rec.t * v.squared_length();
                float cosine = fabs(dot(v, rec.normal) / v.length());
                return distance_squared / (cosine * area);
            }
            else
                return 0.;
        }
        virtual vec3 random(const vec3 &o)
            const
        {
            vec3 random_point = vec3(x0 + drand48() * (x1 - x0), k_, z0 + drand48() * (z1 - z0));
            return random_point - o;
        }
};

bool xz_rect::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    float t = (k_-r.origin().y())/(r.direction().y());
    if(t < t_min || t > t_max)
        return false;
    float x = r.origin().x() + t*r.direction().x();
    float z = r.origin().z() + t*r.direction().z();
    if(x < x0 || x > x1 || z < z0 || z > z1)
        return false;
    rec.hit_u = (x-x0)/(x1-x0);
    rec.hit_v = (z-z0)/(z1-z0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.poin_at_parameter(t);
    //rec.normal = dot(r.direction(),vec3(0,1,0))>0? vec3(0,-1,0):vec3(0,1,0);
    rec.normal = vec3(0,1,0);
    return true;
}

class yz_rect :public hitable
{
    public:
        float y0;
        float y1;
        float z0;
        float z1;
        float k_;
        material *mp;

        yz_rect() {}
        yz_rect(float _y0, float _y1, float _z0, float _z1, float _k_, material *mat):
            y0(_y0<_y1?_y0:_y1),
            y1(_y0>_y1?_y0:_y1),
            z0(_z0<_z1?_z0:_z1),
            z1(_z0>_z1?_z0:_z1),
            k_(_k_),
            mp(mat) {}
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
        virtual bool bounding_box(float t0, float t1, aabb &box)
            const
        {
            box = aabb(vec3(k_ - 0.0001, y0, z0), vec3(k_ + 0.0001, y1, z1));
            return true;
        }
        virtual float pdf_value(const vec3 &o, const vec3 &v)
            const
        {
            hit_record rec;
            if (this->hit(ray(o, v), 1e-3, FLT_MAX, rec))
            {
                float area = (y1 - y0) * (z1 - z0);
                float distance_squared = rec.t * rec.t * v.squared_length();
                float cosine = fabs(dot(v, rec.normal) / v.length());
                return distance_squared / (cosine * area);
            }
            else
                return 0.;
        }
        virtual vec3 random(const vec3 &o)
            const
        {
            vec3 random_point = vec3(k_, y0 + drand48() * (y1 - y0), z0 + drand48() * (z1 - z0));
            return random_point - o;
        }
};

bool yz_rect::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    float t = (k_-r.origin().x())/(r.direction().x());
    if(t < t_min || t > t_max)
        return false;
    float y = r.origin().y() + t*r.direction().y();
    float z = r.origin().z() + t*r.direction().z();
    if(y < y0 || y > y1 || z < z0 || z > z1)
        return false;
    rec.hit_u = (y-y0)/(y1-y0);
    rec.hit_v = (z-z0)/(z1-z0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.poin_at_parameter(t);
    //rec.normal = dot(r.direction(),vec3(1,0,0))>0? vec3(-1,0,0):vec3(1,0,0);
    rec.normal = vec3(1,0,0);
    return true;
}


//时间问题，为完成box的创建功能，且这仅能生成一个无限大的平面（这是个不合法的平面）
class plane : public hitable
{
    public:
        vec3 center;
        vec3 face_to;
        material *mat_ptr;

        plane(vec3 center_, vec3 face_to_, material *mat_ptr_) : center(center_), face_to(face_to_), mat_ptr(mat_ptr_) {}
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
};

bool plane :: hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    /*
    center = (a,b,c)
    face_to = (a1, b1, c1)
    ray = (x,y,z)
    ray_face = (x1, y1, z1)
    a1(x-a)+b1(y-b)+c1(z-c) = 0
    ->face_to(ray+t*ray_face_to)=face_to*center
    ->t = (dot(face_to,center)-dot(face_to,ray))/dot(face_to,ray_face_to)
     */
    //this function is true. it can creat a limitless plane
    if(dot(r.direction(),face_to)==0)
        return false;
    float a = dot(face_to, center);
    float b = dot(face_to, r.origin());
    float c = dot(face_to, r.direction());

    float temp = (a - b) / c;
    if( temp > t_min && temp < t_max )
    {
        vec3 count = r.poin_at_parameter(temp);
        if(dot(count,count)>400)//i don't want it to be a limitless plane
            return false;
        rec.t = temp;
        rec.p = count;
        rec.normal = face_to;
        rec.mat_ptr = mat_ptr;
        return true;
    }
    return false;
}

#endif