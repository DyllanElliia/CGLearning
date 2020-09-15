#ifndef DRAND48
#define DRAND48

#include <math.h>
#include <stdlib.h>
#include "vec3.h"

#define M_PI 3.14159265358979323846

inline double drand48()
{
    return double(rand()) / double(RAND_MAX);
}

//É¢Éä
vec3 random_in_unit_sphere()
{
    vec3 p;
    do
    {
        p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0);
    return p;
}
//same for object
vec3 random_in_unit_object()
{
    vec3 p;
    do
    {
        p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0);
    return p;
}

inline vec3 random_cosine_direction()
{
    double r1 = drand48();
    double r2 = drand48();
    double z = sqrt(1 - r2);
    double theta = 2 * M_PI * r1;
    double x = cos(theta) * 2 * sqrt(r2);
    double y = sin(theta) * 2 * sqrt(r2);
    return vec3(x, y, z);
}

inline vec3 random_to_sphere(float radius, float distance_squared)
{
    float r1 = drand48();
    float r2 = drand48();
    float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
    float phi = 2 * M_PI * r1;
    float x = cos(phi) * sqrt(1 - z * z);
    float y = sin(phi) * sqrt(1 - z * z);
    return vec3(x, y, z);
}

#endif