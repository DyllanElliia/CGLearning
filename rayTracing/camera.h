#ifndef CAMERAH
#define CAMERAH

#include "RAY.h"

#define M_PI 3.14159265358979323846

vec3 random_in_unit_disk()
{
    vec3 p;
    do
    {
        p = 2.0*vec3(drand48(),drand48(),0)-vec3(1,1,0);
    }while(dot(p,p)>=1.0);
    return p;
}
namespace CamMode{
#define CAM_MODE_Random int(0B0)
#define CAM_MODE_Jitter int(0B1)

vec3 CamModeRandom()
{
    return random_in_unit_disk();
}
int count = -1;
int MaxCount = 10;
float delta = 0.1;
vec3 CamModeJitter()
{
    if (++count >= MaxCount)
        count = 0;
    return  drand48()*drand48() *
            vec3(   cos(float(count) * delta * M_PI),
                    sin(float(count) * delta * M_PI),
                    0);
}
}
//moving_camera
class camera
{
    protected:

    public:
        typedef vec3(*MathPtr)();
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        float lens_radius;
        float time_0;
        float time_1;
        MathPtr camMode;

        //（相机位置，相机镜头朝向的方向， 相机底部指向顶部的方向，视野，长宽比, time_0, time_1）
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist, float time_in_0, float time_in_1)
            : camMode(&CamMode::CamModeRandom)
        {

            //save the object's moving_time
            time_0 = time_in_0;
            time_1 = time_in_1;

            lens_radius = aperture/2;
            float theta = vfov * M_PI / 180;//视角
            float half_height = tan(theta / 2);
            float half_width = aspect * half_height;
            origin = lookfrom;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            lower_left_corner = origin - (half_width * u + half_height * v + w) * focus_dist;
            horizontal = 2 * half_width * u * focus_dist;
            vertical = 2 * half_height * v * focus_dist;
        }

        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist, float time_in_0, float time_in_1, int setCamMode)
            : camMode(  setCamMode == (CAM_MODE_Random)?
                        (CamMode::CamModeRandom):
                        setCamMode == (CAM_MODE_Jitter)?
                        (CamMode::CamModeJitter):(CamMode::CamModeRandom))
        {
            //save the object's moving_time
            time_0 = time_in_0;
            time_1 = time_in_1;

            lens_radius = aperture/2;
            float theta = vfov * M_PI / 180;//视角
            float half_height = tan(theta / 2);
            float half_width = aspect * half_height;
            origin = lookfrom;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            lower_left_corner = origin - (half_width * u + half_height * v + w) * focus_dist;
            horizontal = 2 * half_width * u * focus_dist;
            vertical = 2 * half_height * v * focus_dist;
        }
        void SettingCamModeJitterMaxCount(int settingMax)
        {
            CamMode::MaxCount = settingMax;
            CamMode::delta = 1/float(settingMax);
        }
        void SettingCamModeJitterMaxCount(int setCamMode, int settingMax)
        {
            camMode =   setCamMode == (CAM_MODE_Random)?
                        (CamMode::CamModeRandom):
                        setCamMode == (CAM_MODE_Jitter)?
                        (CamMode::CamModeJitter):(CamMode::CamModeRandom);
            CamMode::MaxCount = settingMax;
            CamMode::delta = 1 / float(settingMax);
        }
        //add time for the ray_save
        inline ray get_ray(float s, float t)
        {
            vec3 rd = lens_radius * camMode();//抗锯齿化
            vec3 offset = u * rd.x() + v * rd.y();//图像上的对应点坐标的相对原点偏移量
            float time = time_0 + drand48()*(time_1 - time_0);  //flash the time
            return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, time);
        }
};


#endif