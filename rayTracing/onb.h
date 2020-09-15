#ifndef ONBH
#define ONBH

#include "vec3.h"

class onb
{
public:
    onb() {}

    inline const vec3 &operator[](int index) const { return axis[index]; }

    inline const vec3 &u() const { return axis[0]; }

    inline const vec3 &v() const { return axis[1]; }

    inline const vec3 &w() const { return axis[2]; }

public:
    inline vec3 local(double a, double b, double c) const;

    inline vec3 local(const vec3 &v) const;

    void build_from_w(const vec3 &);

private:
    vec3 axis[3];
};

inline vec3 onb::local(double a, double b, double c) const
{
    return a * u() + b * v() + c * w();
}

inline vec3 onb::local(const vec3 &v) const
{
    return local(v.x(), v.y(), v.z());
}

inline void onb::build_from_w(const vec3 &V)
{
    axis[2] = unit_vector(V);
    vec3 a;
    if (fabs(w().x()) > 0.9)
        a = vec3(0, 1, 0);
    else
        a = vec3(1, 0, 0);
    axis[1] = unit_vector(cross(w(),a));
    axis[0] = cross(w(), v());
}

#endif