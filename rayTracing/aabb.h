#ifndef AABBH
#define AABBH

#include "RAY.h"

inline float ffmin(float a, float b){return a<b?a:b;}
inline float ffmax(float a, float b){return a>b?a:b;}

class aabb
{
    private:
        vec3 _min;//box_min_vector
        vec3 _max;//box_max_vector
    public:
        aabb() {}
        aabb(const vec3 min, const vec3 max):_min(min), _max(max) {}

        inline vec3 min() const {return _min;}
        inline vec3 max() const {return _max;}
        inline bool hit(const ray &r, float tmin, float tmax)const;
};

inline bool aabb::hit(const ray &r, float tmin, float tmax)
    const
{
    //x-y-z, ording to find whether hit box
    //judge the ray whether hit the box
    for(int a = 0; a < 3; a++)
    {
        float invD = 1.0f / r.direction()[a];
        float t0 = (min()[a] - r.origin()[a]) * invD;
        float t1 = (max()[a] - r.origin()[a]) * invD;
        if(invD < 0.0f)
            std::swap(t0, t1);
        tmin = t0 > tmin? t0:tmin;
        tmax = t1 < tmax? t1:tmax;
        if (tmin >= tmax)
            return false;
    }
    return true;
}


inline aabb surrounding_box(aabb &box0, aabb &box1)
{
    //find the minest box
    vec3 small = vec3(ffmin(box0.min().x(), box1.min().x()),
                      ffmin(box0.min().y(), box1.min().y()),
                      ffmin(box0.min().z(), box1.min().z()));
    //find the maxest box
    vec3 max   = vec3(ffmax(box0.max().x(), box1.max().x()),
                      ffmax(box0.max().y(), box1.max().y()),
                      ffmax(box0.max().z(), box1.max().z()));
    return aabb(small, max);
}

#endif