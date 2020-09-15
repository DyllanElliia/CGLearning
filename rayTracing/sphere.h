#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "aabb.h"
#include "drand48.h"

inline void get_sphere_uv(const vec3 &p, float &u, float &v)
{
    float pHi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1 - (pHi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}

class sphere: public hitable
{
    public:
        vec3 center;
        float radius;
        material *mat_ptr;
        sphere() {}
        //sphere(vec3 cen, float r) : center(cen), radius(r) {}
        sphere(vec3 cen, float r, material *mat_ptr) : center(cen), radius(r), mat_ptr(mat_ptr) {}
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec)const;
        virtual bool bounding_box(float t0, float t1, aabb &box) const;
        virtual float pdf_value(const vec3 &o, const vec3 &v) const;
        virtual vec3 random(const vec3 &o) const;
        //virtual material *return_material(){ return mat_ptr; }
};

float sphere::pdf_value(const vec3 &o, const vec3 &v) const
{
    hit_record rec;
    if (this->hit(ray(o, v), 0.001, FLT_MAX, rec))
    {
        float cos_theta_max = sqrt(1 - radius * radius / (center - o).squared_length());
        float solid_angle = 2 * M_PI * (1 - cos_theta_max);
        return 1 / solid_angle;
    }else
        return 0;
}

vec3 sphere::random(const vec3 &o) const
{
    vec3 direction = center - o;
    float distance_squared = direction.squared_length();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
}

bool sphere::bounding_box(float t0, float t1, aabb &box)
    const
{
    //创立一个盒子，一个一球为内切球和盒子，返回后左下角坐标和前左上角坐标
    box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}

//hit( in_ray, t_min, t_max, save_first_hit_t)
bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    //圆面  (p(t)-center)(p(t)-center) = R*R
    //展开  (origin + t*direction - center)(origin + t*direction - center) = R*R
    //展开  t*t*direction*direction + 2*t*direction*(oringin-center) + (oringin-center)*(oringin-center) - R*R = 0
    //a = direction*direction;  b = direction*(oringin-center); c = (oringin-center)*(oringin-center) - R*R
    //寻找是否有 t ，使光线延长后与圆存在交点；
    vec3 oc = r.origin() - center;                      //原点到光点的向量
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - a*c;
    if(discriminant > 0)
    {                                                   //由近到远
        float temp = (-b - sqrt(discriminant)) / a;     //最小 t
        if(temp < t_max && temp > t_min)
        {
            rec.t = temp;                               //保存时间
            rec.p = r.poin_at_parameter(rec.t);         //保存移动后的位置
            rec.normal = (rec.p - center) / radius;     //球面上的法向量
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p-center)/radius,rec.hit_u,rec.hit_v);
            return true;                                //保存成功
        }
        temp = (-b + sqrt(discriminant)) / a;           //若最短 t不满足，判断最大 t
        if (temp < t_max && temp > t_min)               //the same
        {
            rec.t = temp;
            rec.p = r.poin_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p - center) / radius, rec.hit_u, rec.hit_v);
            return true;
        }
    }
    return false;   //未找到交点
}

class moving_sphere : public hitable
{
    public:
        vec3 center_0, center_1;
        float time_0, time_1;
        float radius;
        material *mat_ptr;
        moving_sphere() {}
        //moving_sphere(vec3 cen, float r) : center(cen), radius(r) {}
        moving_sphere(vec3 cen_0, vec3 cen_1, float tiem_0, float time_1, float r, material *mat_ptr):
        center_0(cen_0), center_1(cen_1), time_0(tiem_0), time_1(time_1),radius(r), mat_ptr(mat_ptr) {}

        inline vec3 center(float time)const;
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec)const;
        virtual bool bounding_box(float t0, float t1, aabb &box) const;
        //virtual material *return_material(){ return mat_ptr; }
};

inline vec3 moving_sphere::center(float time)
    const
{
    //返回经过时间time后的球体中心的坐标
    return (center_0 + ((time-time_0) / (time_1 - time_0))*(center_1 - center_0));
}

bool moving_sphere::bounding_box(float t0, float t1, aabb &box)
    const
{
    box = aabb( vec3(   center_0.x()<center_1.x()?center_0.x():center_1.x(),
                        center_0.y()<center_1.y()?center_0.y():center_1.y(),
                        center_0.z()<center_1.z()?center_0.z():center_1.z())
              - vec3(radius, radius, radius),
                vec3(   center_0.x()>center_1.x()?center_0.x():center_1.x(),
                        center_0.y()>center_1.y()?center_0.y():center_1.y(),
                        center_0.z()>center_1.z()?center_0.z():center_1.z())
              + vec3(radius, radius, radius));
    return true;
}

bool moving_sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    //圆面  (p(t)-center)(p(t)-center) = R*R
    //展开  (origin + t*direction - center)(origin + t*direction - center) = R*R
    //展开  t*t*direction*direction + 2*t*direction*(oringin-center) + (oringin-center)*(oringin-center) - R*R = 0
    //a = direction*direction;  b = direction*(oringin-center); c = (oringin-center)*(oringin-center) - R*R
    //寻找是否有 t ，使光线延长后与圆存在交点；
    vec3 oc = r.origin() - center(r.time()); //原点到光点的向量
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant > 0)
    {                                                //由近到远
        float temp = (-b - sqrt(discriminant)) / a;  //最小 t
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;                           //保存时间
            rec.p = r.poin_at_parameter(rec.t);     //保存移动后的位置
            rec.normal = (rec.p - center(r.time())) / radius; //球面上的法向量
            rec.mat_ptr = mat_ptr;                  //存储材质
            get_sphere_uv((rec.p - center(r.time())) / radius, rec.hit_u, rec.hit_v);
            return true; //保存成功
        }
        temp = (-b + sqrt(discriminant)) / a;       //若最短 t不满足，判断最大 t，同上
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = r.poin_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p - center(r.time())) / radius, rec.hit_u, rec.hit_v);
            return true;
        }
    }
    return false; //未找到交点
}

#endif