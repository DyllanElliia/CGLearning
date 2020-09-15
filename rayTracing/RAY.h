#ifndef RAY
#define RAY
#include "vec3.h"

class ray
{
    public:
        vec3 A;         //������
        vec3 B;         //����ʸ��
        float t_time;   //save the time of t

        ray() {}
        ray(const vec3 &a, const vec3 &b) {A = a; B = b;}
        ray(const vec3 &a, const vec3 &b, const float time):A(a),B(b),t_time(time) {}
        inline vec3 origin()   const    { return A; }
        inline vec3 direction()const    { return B; }
        inline vec3 poin_at_parameter(float t) const   { return A + t * B; }
        //�� A ������ �ƶ� t �����룬����Ϊ B
        inline float time()const        {return t_time;}
};

#endif