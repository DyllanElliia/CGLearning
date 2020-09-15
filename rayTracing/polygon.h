#ifndef POLYGON
#define POLYGON

#include "hitable.h"
#include "aabb.h"
#include "drand48.h"

inline float vec3_float(vec3 p)
{
    return p[0]+p[1]+p[2];
}

inline void get_triangle_uv(const vec3 (&vertex)[3], vec3 &origin, const vec3 &normal, float &u, float &v)
{
    vec3 center = vec3( (vertex[0].x()+vertex[1].x()+vertex[2].x())/3.0,
                        (vertex[0].y()+vertex[1].y()+vertex[2].y())/3.0,
                        (vertex[0].z()+vertex[1].z()+vertex[2].z())/3.0);
    vec3 to_center[3];
    to_center[0] = center-vertex[0];
    to_center[1] = center-vertex[1];
    to_center[2] = center-vertex[2];
    v = acos(vec3_float(unit_vector(origin-center)*unit_vector(to_center[0])))/(2.0*M_PI);
    u =  0;//´ý²¹³ä
    /*int num = int(3.*drand48());
    for(int i = 0; i < 3; i++)
    {

    }*/
}

inline bool judge_triangle(const vec3 A, const vec3 B, const vec3 C, const vec3 P)
{
    vec3 AB = B - A;
    vec3 AC = C - A;
    vec3 AP = P - A;
    vec3 v1 = cross(AB,AC);
    vec3 v2 = cross(AB,AP);
    return dot(v1,v2) >= 0;
}

inline float count_area(const vec3 (&vertex)[3])
{
    float a = (vertex[0]-vertex[1]).length();
    float b = (vertex[1]-vertex[2]).length();
    float c = (vertex[0]-vertex[2]).length();
    float p = (a+b+c)/2;
    return sqrt(p*(p-a)*(p-b)*(p-c));
}

class triangle : public hitable
{
private:
    vec3 vertex[3];
    vec3 normal;
    material *mat_ptr;
    //float ver_number;
    float area_;

public:
    triangle() {}
    //sphere(vec3 cen, float r) : center(cen), radius(r) {}
    triangle(vec3 ver[3], material *mat_ptr);
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
    virtual bool bounding_box(float t0, float t1, aabb &box) const;
    virtual float pdf_value(const vec3 &o, const vec3 &v) const;
    virtual vec3 random(const vec3 &o) const;
    //virtual material *return_material(){ return mat_ptr; }
};

float triangle::pdf_value(const vec3 &o, const vec3 &v)
    const
{
    hit_record rec;
    if (this->hit(ray(o, v), 1e-3, FLT_MAX, rec))
    {
        float area = area_;
        float distance_squared = rec.t * rec.t * v.squared_length();
        float cosine = fabs(dot(v, rec.normal) / v.length());
        return distance_squared / (cosine * area);
    }
    else
        return 0.;
}

vec3 triangle::triangle::random(const vec3 &o)
    const
{
    float r1 = drand48();
    float r2 = drand48();
    return ((1-sqrt(r1))*vertex[0]+sqrt(r1)*(1-r2)*vertex[1]+sqrt(r1)*r2*vertex[2])-o;
}

triangle::triangle(vec3 ver[3], material *mat_ptr)
    : mat_ptr(mat_ptr)
{
    vertex[0] = ver[0];
    vertex[1] = ver[1];
    vertex[2] = ver[2];
    //ver_number = vec3_float(vertex[0]) + vec3_float(vertex[1]) + vec3_float(vertex[2]);
    area_ = count_area(vertex);
    vec3 a = vertex[0]-vertex[1];
    vec3 b = vertex[0]-vertex[2];
    normal = vec3(  a[1]*b[2]-a[2]*b[1],
                    a[2]*b[0]-a[0]*b[2],
                    a[0]*b[1]-a[1]*b[0]);
    normal.make_unit_vector();
}

bool triangle::bounding_box(float t0, float t1, aabb &box)
    const
{
    box = aabb(vec3(vertex[0].x()<vertex[1].x()?(vertex[0].x()<vertex[2].x()?vertex[0].x()-10:vertex[2].x()-10):(vertex[1].x()<vertex[2].x()?vertex[1].x()-10:vertex[2].x()-10),
                    vertex[0].y()<vertex[1].y()?(vertex[0].y()<vertex[2].y()?vertex[0].y()-10:vertex[2].y()-10):(vertex[1].y()<vertex[2].y()?vertex[1].y()-10:vertex[2].y()-10),
                    vertex[0].z()<vertex[1].z()?(vertex[0].z()<vertex[2].z()?vertex[0].z()-10:vertex[2].z()-10):(vertex[1].z()<vertex[2].z()?vertex[1].x()-10:vertex[2].z()-10)),
            vec3(   vertex[0].x()>vertex[1].x()?(vertex[0].x()>vertex[2].x()?vertex[0].x()+10:vertex[2].x()+10):(vertex[1].x()>vertex[2].x()?vertex[1].x()+10:vertex[2].x()+10),
                    vertex[0].y()>vertex[1].y()?(vertex[0].y()>vertex[2].y()?vertex[0].y()+10:vertex[2].y()+10):(vertex[1].y()>vertex[2].y()?vertex[1].y()+10:vertex[2].y()+10),
                    vertex[0].z()>vertex[1].z()?(vertex[0].z()>vertex[2].z()?vertex[0].z()+10:vertex[2].z()+10):(vertex[1].z()>vertex[2].z()?vertex[1].x()+10:vertex[2].z()+10)));
    //for(int i = 0; i < 3; i++)
    //    if((box.max[i]-box.min[i])<5)
    //    { box.max[i]+=5;box.min[i]-=5;}
    return true;
}

bool triangle::hit(const ray &r_in, float t_min, float t_max, hit_record &rec)
    const
{
    if(dot(r_in.direction(),normal)==0)
        return false;
    float a = dot(normal, vertex[0]);
    float b = dot(normal, r_in.origin());
    float c = dot(normal, r_in.direction());
    float new_t = (a-b)/c;
    if(new_t > t_max || new_t < t_min)
        return false;

    vec3 new_origin = r_in.poin_at_parameter(new_t);
    if( judge_triangle(vertex[0],vertex[1],vertex[2],new_origin)&&
        judge_triangle(vertex[1],vertex[2],vertex[0],new_origin)&&
        judge_triangle(vertex[2],vertex[0],vertex[1],new_origin))
    {
        rec.t = new_t;
        rec.p = new_origin;
        rec.normal = dot(r_in.direction(),normal)<0?normal:-normal;
        rec.mat_ptr = mat_ptr;
        get_triangle_uv(vertex,new_origin,normal,rec.hit_u,rec.hit_v);
        return true;
    }
    return false;
}

#endif