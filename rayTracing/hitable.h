#ifndef HITABLE
#define HITABLE

#include "RAY.h"
#include "aabb.h"
#include "drand48.h"
#include "onb.h"
#include "texture.h"
#include "vec3.h"

//反射
inline vec3 reflect(const vec3 &v, const vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

//介电质的折射代码
bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted)
{
    vec3 uv = unit_vector(v); //得到 v的单位向量
    float dt = dot(uv, n);    //矢量相乘
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0)
    {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    else
        return false;
}

inline float schlick(float cosine, float ref_idx)
{
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

class material; //for hit_record

struct hit_record
{
    float t;           //时间计数器
    vec3 p;            //撞击点
    vec3 normal;       //撞击面的法向量
    material *mat_ptr; //材质
    //针对图片设定，设图片纹理大小为（nx，ny）
    //撞击坐标为（i，j）
    //则 u = i/(nx-1)；v = j/(ny-1)；
    float hit_u;
    float hit_v;
};

class hitable
{
public:
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
    //virtual material* return_material()= 0;
    virtual bool bounding_box(float t0, float t1, aabb &box) const = 0;

    virtual float pdf_value(const vec3 &o, const vec3 &v) const { return 1.0; }
    virtual vec3 random(const vec3 &o) const { return vec3(1, 0, 0); }
};

class pdf
{
public:
    /*
    @brief: we get the value of pdf function by this interface
    @param: the direction of location
    @retur: the value of the pdf function
    */
    virtual float value(const vec3 &direction) const = 0;

    /*
    @brief: generate a random number with a Probability model
    @param: none
    @retur: the Three-dimensional random vector
    */
    virtual vec3 generate() const = 0;
};

#include "pdf.h"

struct scatter_record
{
    ray specular_ray;
    bool is_specular;
    vec3 attenuation;
    pdf *pdf_ptr;
};

class material
{
public:
    virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const { return false; };
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &albedo, ray &scattered, float &pdf) const { return false; };
    virtual vec3 emitted(const ray &r_in, const hit_record &rec, float u, float v, const vec3 &p) const { return vec3(0, 0, 0); }
    virtual float scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered)
        const
    {
        return false;
    }
    virtual void print() = 0;
    virtual bool ligh_judge() { return false; };
};

class lambertian : public material
{
private:
    texture *albedo; //颜色/衰减量
public:
    lambertian(texture *a) : albedo(a) {} //颜色/衰减量
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered, float &pdf)
        const
    {
        onb uvw;
        uvw.build_from_w(rec.normal);
        do
        {
            vec3 direction = uvw.local(random_cosine_direction());
            scattered = ray(rec.p, unit_vector(direction), r_in.time());
            pdf = dot(uvw.w(), scattered.direction()) / M_PI;
        } while (pdf == 0.0f);
        attenuation = albedo->value(rec.hit_u, rec.hit_v, rec.p);
        //cout << attenuation<<endl;
        return true;
    }
    virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec)
        const
    {
        srec.specular_ray = r_in;
        srec.is_specular = false;
        srec.attenuation = albedo->value(rec.hit_u, rec.hit_v, rec.p);
        srec.pdf_ptr = new cosine_pdf(rec.normal);
        return true;
        /*
            //new scatter
            onb uvw;
            uvw.build_from_w(rec.normal);
            do
            {
                vec3 direction = uvw.local(random_cosine_direction());
                scattered = ray(rec.p, unit_vector(direction), r_in.time());
                pdf = dot(uvw.w(), scattered.direction()) / M_PI;
            }while(pdf == 0.0f);
            alb = albedo->value(rec.hit_u, rec.hit_v, rec.p);
            return true;



            vec3 direction;
            do{
                direction = random_in_unit_object();
            }while(dot(direction, rec.normal)<0);
            //vec3 target = rec.p + rec.normal + random_in_unit_object();                 //随机反射方向
            scattered = ray(rec.p, unit_vector(direction), r_in.time());           //反射光线的矢量方向
            //scattered = ray(rec.p, unit_vector(target - rec.p), r_in.time());           //反射光线的矢量方向
            alb = albedo->value(rec.hit_u,rec.hit_v,rec.p);                             //颜色/衰减量保存
            pdf = 0.5 / M_PI;
            //pdf = dot(rec.normal, scattered.direction()) / M_PI;
            return true;*/
    }
    virtual float scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered)
        const
    {
        float cosine = dot(rec.normal, unit_vector(scattered.direction()));
        if (cosine < 0)
            cosine = 0;
        return cosine / M_PI;
    }
    virtual void print()
    {
        //cout <<"labertian "<< albedo <<endl;                        //text
    }
    virtual bool ligh_judge() { return false; }
};

class metal : public material
{
private:
    texture *albedo; //颜色/衰减量
    float fuzz;

public:
    metal(texture *a, float f) : albedo(a)
    {
        if (f < 1)
            fuzz = f;
        else
            fuzz = 1;
    }
    virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec)
        const
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        srec.specular_ray = ray(rec.p, unit_vector(reflected + fuzz * random_in_unit_object()));
        srec.attenuation = albedo->value(rec.hit_u, rec.hit_v, rec.p);
        srec.is_specular = true;
        srec.pdf_ptr = NULL;
        return true;
    }
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered, float &pdf)
        const
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal); //镜面反射方向
        //scattered = ray(rec.p, reflected);                                            //反射光线的矢量方向
        scattered = ray(rec.p, unit_vector(reflected + fuzz * random_in_unit_object()), r_in.time()); //反射光线的矢量方向，随机模糊方向
        attenuation = albedo->value(rec.hit_u, rec.hit_v, rec.p);                                     //颜色/衰减量保存
        //while(dot(scattered.direction(), rec.normal) < 0)                             //text
        //    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        //return true;
        return (dot(scattered.direction(), rec.normal) > 0); //判断反射矢量和入射矢量之间的角度
    }
    virtual void print()
    {
        //cout << "metal " << albedo << endl;         //text
    }
    virtual bool ligh_judge() { return false; }
};

class dielectric : public material
{
public:
    texture *albedo; //颜色/衰减量
    float ref_idx;
    float fuzz = 0;
    dielectric(texture *a, float ri) : albedo(a), ref_idx(ri) {}
    dielectric(texture *a, float ri, float f) : albedo(a), ref_idx(ri), fuzz(f) {}
    virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec)
        const
    {
        srec.is_specular = true;
        srec.pdf_ptr = NULL;
        srec.attenuation = albedo->value(rec.hit_u, rec.hit_v, rec.p);
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        vec3 refracted;
        float ni_over_nt;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0)
        {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        else
        {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
        {
            reflect_prob = schlick(cosine, ref_idx);
        }
        else
        {
            reflect_prob = 1.0;
        }
        if (drand48() < reflect_prob)
        {
            srec.specular_ray = ray(rec.p, unit_vector(reflected + fuzz * random_in_unit_object()));
        }
        else
        {
            srec.specular_ray = ray(rec.p, unit_vector(reflected + fuzz * random_in_unit_object()));
        }
        return true;
    }
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered, float &pdf)
        const
    {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal); //反射反向
        float ni_over_nt;                                       //光的折射率
        attenuation = albedo->value(rec.hit_u, rec.hit_v, rec.p);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0) //判断夹角
        {                                          //进入
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        else
        { //出去
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
        {
            reflect_prob = schlick(cosine, ref_idx);
            //scattered = ray(rec.p,reflected);
        }
        else
        {
            scattered = ray(rec.p, reflected, r_in.time());
            reflect_prob = 1.0;
        }
        if (drand48() < reflect_prob)
            scattered = ray(rec.p, reflected, r_in.time());
        else
            scattered = ray(rec.p, refracted, r_in.time());
        return true;
    }
    virtual void print()
    {
        //cout << "dielectric " << albedo << endl;                                //text
    }
    virtual bool ligh_judge() { return false; }
};

//毛玻璃素材，此处单独列出，后期将与玻璃类素材合并
class dielectric_rand : public material
{
public:
    vec3 albedo; //颜色/衰减量
    float ref_idx;
    float fuzz; //模糊度
    dielectric_rand(const vec3 &a, float ri, float fuzz) : albedo(a), ref_idx(ri), fuzz(fuzz) {}
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered, float &pdf)
        const
    {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal); //反射方向
        float ni_over_nt;                                       //光的折射率
        attenuation = vec3(albedo);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0) //判断夹角
        {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        else
        {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
        {
            reflect_prob = schlick(cosine, ref_idx);
            //scattered = ray(rec.p,reflected);
        }
        else
        {
            scattered = ray(rec.p, unit_vector(reflected + fuzz * random_in_unit_object()), r_in.time());
            reflect_prob = 1.0;
        }
        if (drand48() < reflect_prob)
            scattered = ray(rec.p, unit_vector(reflected + fuzz * random_in_unit_object()), r_in.time());
        else
            scattered = ray(rec.p, unit_vector(refracted + fuzz * random_in_unit_object()), r_in.time());
        return true;
    }
    virtual void print()
    {
        //cout << "dielectric " << albedo << endl;                                //text
    }
    virtual bool ligh_judge() { return false; }
};

class my_light : public material
{
public:
    texture *emit;
    my_light(texture *a) : emit(a) {}
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered, float &pdf)
        const
    {
        scattered.direction() = vec3(0, 0, 0);
        scattered.origin() = r_in.origin();
        scattered.t_time = r_in.time();
        attenuation = emit->value(rec.hit_u, rec.hit_v, rec.p);
        return true;
    }
    virtual void print()
    {
        //cout << "light " << emit->value << endl; //text
    }
    virtual bool ligh_judge() { return true; }
};

class light : public material
{
public:
    texture *emit;
    light(texture *a) : emit(a) {}
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered, float &pdf)
        const
    {
        return false;
    }
    virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec)
        const
    {
        return false;
    }
    virtual vec3 emitted(const ray &r_in, const hit_record &rec, float u, float v, const vec3 &p)
        const
    {
        if (dot(rec.normal, r_in.direction()) < 0.0)
            return emit->value(u, v, p);
        else
            return vec3();
    }
    virtual void print()
    {
        //cout << "light " << emit->value << endl; //text
    }
    virtual bool ligh_judge() { return true; }
};

class isotropic : public material
{
private:
    texture *albedo;

public:
    isotropic(texture *a) : albedo(a) {}
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered, float &pdf)
        const
    {
        scattered = ray(rec.p, random_in_unit_sphere());
        attenuation = albedo->value(rec.hit_u, rec.hit_v, rec.p);
        return true;
    }
    virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec)
        const
    {
        srec.specular_ray = ray(rec.p, random_in_unit_sphere());
        srec.attenuation = albedo->value(rec.hit_u, rec.hit_v, rec.p);
        srec.is_specular = true;
        srec.pdf_ptr = NULL;
        return true;
    }
    virtual void print()
    {
        //cout << "light " << emit->value << endl; //text
    }
    virtual bool ligh_judge() { return true; }
};

class constant_medium : public hitable
{
private:
    hitable *boundary;
    float density;
    material *phase_function;

public:
    constant_medium(hitable *b, float d, texture *a) : boundary(b),
                                                       density(d)
    {
        phase_function = new isotropic(a);
    }
    virtual bool bounding_box(float t0, float t1, aabb &box)
        const
    {
        return boundary->bounding_box(t0, t1, box);
    }
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
    //virtual float pdf_value(const vec3 &o, const vec3 &v) const { return boundary->pdf_value(o,v); }
    //virtual vec3 random(const vec3 &o) const { return boundary->random(o); }
};

bool constant_medium::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    hit_record rec1, rec2;
    if (boundary->hit(r, -FLT_MAX, FLT_MAX, rec1))
    {
        if (boundary->hit(r, rec1.t + 0.0001, FLT_MAX, rec2))
        {
            if (rec1.t < t_min)
                rec1.t = t_min;
            if (rec2.t > t_max)
                rec2.t = t_max;
            if (rec1.t >= rec2.t)
                return false;
            if (rec1.t < 0)
                rec1.t = 0;
            float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
            float hit_distance = -(1 / density) * log(drand48());
            if (hit_distance < distance_inside_boundary)
            {
                rec.t = rec1.t + hit_distance / r.direction().length();
                rec.p = r.poin_at_parameter(rec.t);
                //rec.normal = vec3(1, 0, 0); // arbitrary
                rec.normal = vec3(drand48(), drand48(), drand48());
                rec.mat_ptr = phase_function;
                return true;
            }
        }
    }
    return false;
}

#endif
