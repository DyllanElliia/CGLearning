#ifndef colorMode
#define colorMode

#include "vec3.h"

namespace colorMode
{
namespace colorMath
{
inline float find_max_vec3(vec3 &a)
{
    return a[0] > a[1] ? (a[0] > a[2] ? a[0] : a[2]) : (a[1] > a[2] ? a[1] : a[2]);
}

inline vec3 for_save(vec3 &a)
{
    if (a[0] > 1 || a[1] > 1 || a[2] > 1)
        return a / find_max_vec3(a);
    else
        return a;
}

vec3 de_nan(vec3 a)
{
    if (a[0] != a[0])
        a[0] = 0;
    if (a[1] != a[1])
        a[1] = 0;
    if (a[2] != a[2])
        a[2] = 0;
    return a;
}
vec3 de_nan_forSave(vec3 a)
{
    if (a[0] != a[0])
        a[0] = 0;
    if (a[1] != a[1])
        a[1] = 0;
    if (a[2] != a[2])
        a[2] = 0;
    return for_save(a);
}
} // namespace colorMath
#define limitVec 0
#define freeVec  1
typedef vec3 (*colorSolution)(vec3 a);
void settingColorMode(colorSolution &math, int mode)
{
    switch (mode)
    {
    case 0:
        math = colorMath::de_nan;
        break;
    case 1:
        math = colorMath::de_nan_forSave;
        break;
    default:
        math = colorMath::de_nan_forSave;
        break;
    }
}
} // namespace colorMode
#endif