#include "include_total.h"

using namespace std;

#define nx 300 //400px
#define ny 300 //300px
#define ns 100 //抗锯齿化的采样次数

//origin_hit_sohere_fuction
bool hit_sphere(const vec3 &center, float radius, const ray &r) //(中心， 半径， 光线)
{
    //圆面  (p(t)-center)(p(t)-center) = R*R
    //展开  (origin + t*direction - center)(origin + t*direction - center) = R*R
    //展开  t*t*direction*direction + 2*t*direction*(oringin-center) + (oringin-center)*(oringin-center) - R*R = 0
    //a = direction*direction;  b = direction*(oringin-center); c = (oringin-center)*(oringin-center) - R*R
    //寻找是否有 t ，使光线延长后与圆存在交点；
    vec3 oc = r.origin() - center; //矢量圆心到 r.origin
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float delta_discriminant = b * b - 4 * a * c; //判断光线与圆的交点数
    return (delta_discriminant > 0);              //delta > 0 即存在交点
}

//origin_color
vec3 color(const ray &r)
{
    float t = hit_sphere(vec3(0, 0, -1), 0.5, r);
    /*
    if(hit_sphere(vec3(0, 0, -1), 0.5, r))  //若存在交点，则染色
        return vec3(1,0,0); //rgb: 255, 0, 0
    */
    if (t > 0.0)
    {
        vec3 N = unit_vector(r.poin_at_parameter(t) - vec3(0, 0, -1)); //(r+t*B - center) / length
        return 0.5 * vec3(N.x() + 1, N.y() + 1, N.z() + 1);            //xyz转 rgb
    }
    vec3 unit_direction = unit_vector(r.direction()); //单位向量
    //float t = 0.5 * (unit_direction.y() + 1.0);
    t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0); //vec3(r/255, g/255, b/255);;
    //线性混合函数 blended_value = (1-t)*start_value + t*end_value;
    //或线性插值 “lerp”
}

vec3 color(const ray &r, hitable *world, int depth)
{
    hit_record rec;
    rec.mat_ptr = NULL;
    //找到最近的撞击物体，并返回坐标，方向。
    if (world->hit(r, 0.001, FLT_MAX, rec)) //0.001,消除随机产生的极小小数形成的暗影斑点问题
    {
        ray scattered;
        vec3 attenuation;
        float text;
        //判断深度，并并判断是否击中物体，若击中，则返回颜色和更新后的反射射线ray
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered, text))
        {
            //    if (rec.mat_ptr->ligh_judge())    、//光源判断函数
            //        return attenuation;
            return attenuation * color(scattered, world, depth + 1); //递归，合并颜色
        }
        else
        {
            return vec3(0, 0, 0); //找未击中物体，返回黑色
        }
    }
    else //background
    {
        //return vec3(0,0,0);
        vec3 unit_direction = unit_vector(r.direction()); //单位向量
        float t = 0.5 * (unit_direction.y()) + 1.0;
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
        //线性混合函数 blended_value = (1-t)*start_value + t*end_value;
    }
}

vec3 color(const ray &r, bvh_node *world, int depth)
{
    hit_record rec;
    //找到最近的撞击物体，并返回坐标，方向。
    if (world->hit(r, 0.001, FLT_MAX, rec)) //0.001,消除随机产生的极小小数形成的暗影斑点问题
    {
        ray scattered;
        vec3 attenuation;
        vec3 light_ = rec.mat_ptr->emitted(scattered, rec, rec.hit_u, rec.hit_v, rec.p); //获取光亮，若没有光，返回黑色
        float pdf;
        vec3 albedo;
        //判断深度，并并判断是否击中物体，若击中，则返回颜色和更新后的反射射线ray
        if (depth < 30 && rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf))
        {
            //    if (rec.mat_ptr->ligh_judge())    //光源判断函数
            //        return attenuation;
            vec3 save = color(scattered, world, depth + 1);
            //cout << "[(" <<light_ << ")\n+\n(" << albedo << ")\n*\n(" << save << ")\n*\n";
            return light_ + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered) * save / pdf; //递归，合并颜色
        }
        else
        {
            //cout << "[(" <<light_<<")\n\n"<<endl;
            return light_; //若未击中物体，返回光的颜色
        }
    }
    else //background
    {
        return vec3(0, 0, 0);
        vec3 unit_direction = unit_vector(r.direction()); //单位向量
        float t = 0.5 * (unit_direction.y()) + 1.0;
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
        //线性混合函数 blended_value = (1-t)*start_value + t*end_value;
    }
}

vec3 first_lerp(const ray &r, bvh_node *world, int depth)
{
    hit_record rec;
    //找到最近的撞击物体，并返回坐标，方向。
    if (world->hit(r, 0.001, FLT_MAX, rec)) //0.001,消除随机产生的极小小数形成的暗影斑点问题
    {
        ray scattered;
        vec3 attenuation;
        vec3 light_; //获取光亮，若没有光，返回黑色
        float pdf;
        vec3 albedo;
        //判断深度，并并判断是否击中物体，若击中，则返回颜色和更新后的反射射线ray
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf))
        {
            light_ = rec.mat_ptr->emitted(scattered, rec, rec.hit_u, rec.hit_v, rec.p);
            vec3 on_light = vec3(213 + drand48() * (343 - 213), 554, 227 + drand48() * (332 - 227));
            vec3 to_light = on_light - rec.p;
            float squared_distance_to_light = to_light.squared_length();
            to_light.make_unit_vector();
            if (dot(to_light, rec.normal) < 0)
                return light_;
            float light_area = (343 - 213) * (332 - 227);
            float light_cosine = fabs(to_light.y());
            if (light_cosine < 0.000001)
                return light_;
            pdf = squared_distance_to_light / (light_cosine * light_area);
            scattered = ray(rec.p, to_light, r.time());
            return light_ + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered) * first_lerp(scattered, world, depth + 1) / pdf; //递归，合并颜色
        }
        else
        {
            light_ = rec.mat_ptr->emitted(scattered, rec, rec.hit_u, rec.hit_v, rec.p);
            //cout << "[(" <<light_<<")\n\n"<<endl;
            return light_; //若未击中物体，返回光的颜色
        }
    }
    else //background
    {
        return vec3(0, 0, 0);
        vec3 unit_direction = unit_vector(r.direction()); //单位向量
        float t = 0.5 * (unit_direction.y()) + 1.0;
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

vec3 wtf_lerp(const ray &r, bvh_node *world, hitable *light_shape, int depth)
{
    hit_record rec;
    //找到最近的撞击物体，并返回坐标，方向。
    if (world->hit(r, 0.001, FLT_MAX, rec)) //0.001,消除随机产生的极小小数形成的暗影斑点问题
    {
        scatter_record srec;
        vec3 light_ = rec.mat_ptr->emitted(r, rec, rec.hit_u, rec.hit_v, rec.p); //获取光亮，若没有光，返回黑色
        //判断深度，并并判断是否击中物体，若击中，则返回颜色和更新后的反射射线ray
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, srec))
        {
            if (srec.is_specular)
                return srec.attenuation * wtf_lerp(srec.specular_ray, world, light_shape, depth + 1);
            hit_pdf p0(light_shape, rec.p);
            mixture_pdf p(&p0, srec.pdf_ptr);
            ray scattered = ray(rec.p, p.generate(), r.time());
            float pdf_val = p.value(scattered.direction());
            delete srec.pdf_ptr;
            //cout << srec.attenuation <<endl;
            return light_ + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * wtf_lerp(scattered, world, light_shape, depth + 1) / pdf_val; //递归，合并颜色
        }
        else
        {
            //cout << "[(" <<light_<<")\n\n"<<endl;
            return light_; //若未击中物体，返回光的颜色
        }
    }
    else //background
    {
        return vec3(0, 0, 0);
        vec3 unit_direction = unit_vector(r.direction()); //单位向量
        float t = 0.5 * (unit_direction.y()) + 1.0;
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

vec3 lerp(const ray &r, bvh_node *world, hitable *light_shape, int depth)
{
    //cout << depth <<" ";
    hit_record rec;
    rec.mat_ptr = NULL;
    //找到最近的撞击物体，并返回坐标，方向。
    if (world->hit(r, 0.001, FLT_MAX, rec)) //0.001,消除随机产生的极小小数形成的暗影斑点问题
    {
        ray scattered;
        float pdf_val;
        vec3 albedo;
        //if(rec.mat_ptr->ligh_judge())
        //    cout <<"hit light"<<endl;
        vec3 light_ = rec.mat_ptr->emitted(r, rec, rec.hit_u, rec.hit_v, rec.p); //获取光亮，若没有光，返回黑色
        //判断深度，并并判断是否击中物体，若击中，则返回颜色和更新后的反射射线ray
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf_val))
        {
            //hitable *shape1 = free_translate(new box(vec3(), vec3(130, 50, 105), nullptr), "", vec3(), vec3(213, 554, 227));
            //hitable *shape = new xz_rect(213,343,227,332,554,nullptr);
            hit_pdf p0(light_shape, rec.p);
            cosine_pdf p1(rec.normal);
            mixture_pdf p(&p0, &p1);
            scattered = ray(rec.p, p.generate(), r.time());
            pdf_val = p.value(scattered.direction());
            float pdf = rec.mat_ptr->scattering_pdf(r, rec, scattered);
            vec3 save = lerp(scattered, world, light_shape, depth + 1);
            //cout << depth<<endl;
            //cout << rec.p << endl;
            //cout << light_ <<endl;
            //cout << albedo << endl;
            //cout <<pdf<<endl;
            //cout <<"---------------"<<endl;
            //cout << "[(" << light_ << ")\n+\n(" << albedo << ")\n*\n(" << save << ")\n/\n(" << pdf_val << ")\n*\n";
            return light_ + albedo * pdf * save / pdf_val; //递归，合并颜色
        }
        else
        {
            //cout <<depth<<endl;
            //cout << "---------------" << endl;
            //cout << rec.p <<endl;
            //cout << "[(" <<light_<<")\n\n"<<endl;
            return light_; //若未击中物体，返回光的颜色
        }
    }
    else //background
    {
        //cout << "---------------" << endl;
        return vec3(0, 0, 0);
        vec3 unit_direction = unit_vector(r.direction()); //单位向量
        float t = 0.5 * (unit_direction.y()) + 1.0;
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

bvh_node *text_light()
{
    texture *perlintex = new noise_texture(6.3);
    material *redlight = new light(new constant_texture(vec3(0.98, 0.1, 0.08)));
    material *bluelight = new light(new constant_texture(vec3(0.05, 0.05, 1.)));

    hitable **list = new hitable *[7];
    list[0] = new sphere(vec3(-2, 3, -3), 1.5, redlight);
    list[1] = new sphere(vec3(-2.2, 3.2, 2.8), 1.5, bluelight);
    list[2] = new sphere(vec3(0, 0, 2.2), 1, new metal(new constant_texture(vec3(1, 1, 1)), 0));
    list[3] = new sphere(vec3(), 1, new lambertian(new constant_texture(vec3(1, 1, 1))));
    list[4] = new sphere(vec3(0, 0, -2), 1, new dielectric(new constant_texture(vec3(1, 1, 1)), 1.5));
    list[5] = new sphere(vec3(0, 0, -2), -0.88, new dielectric(new constant_texture(vec3(1, 1, 1)), 1.5));
    list[6] = new sphere(vec3(0, -1000, 0), 999,
                         new lambertian(new constant_texture(vec3(1, 1, 1))));
    return new bvh_node(list, 7, 0, FLT_MAX);
}

bvh_node *finalSence(hitable **&light_s, int &count_light)
{
    count_light = 2;
    light_s = new hitable *[count_light];
    int nb = 20;
    hitable **list = new hitable *[40];
    hitable **boxlist = new hitable *[2000];
    hitable **boxlist2 = new hitable *[2000];

    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));

    int b = 0;
    for (int i = 0; i < nb; ++i)
        for (int j = 0; j < nb; ++j)
        {
            float w = 100;
            float x0 = -1000 + i * w;
            float z0 = -1000 + j * w;
            float y0 = 0;
            float x1 = x0 + w;
            float y1 = 100 * (drand48() + 0.01);
            float z1 = z0 + w;
            boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
        }

    int l = 0;
    list[l++] = new bvh_node(boxlist, b, 0, 1);
    material *light_ = new light(new constant_texture(vec3(20, 20, 20)));
    light_s[0] = list[l++] = new xz_rect(123, 423, 147, 412, 554, light_);
    vec3 center(400, 400, 200);
    //list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
    light_s[1] = list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(new constant_texture(vec3(1, 1, 1)), 1.5));
    list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(new constant_texture(vec3(0.8, 0.8, 0.9)), 10.0));
    texture *noise = new noise_sea_texture(2);
    hitable *boundary = new sphere(vec3(360, 150, 145), 69, new metal(noise, 0.1));
    //light_s[2] = list[l++] = boundary;
    list[l++] = boundary;
    boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(noise, 1.5));
    list[l++] = boundary;
    //list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
    boundary = new sphere(vec3(), 5000, new lambertian(new constant_texture(vec3(1, 1, 1))));
    //list[l++] = new constant_medium(boundary, 0.0005, new constant_texture(vec3(1, 1, 1)));

    int x, y, n;
    unsigned char *tex = stbi_load("picture/earth_map.jpg", &x, &y, &n, 0);
    material *emat = new lambertian(new image_texture(tex, x, y));
    list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
    texture *pertext = new noise_marble1_texture(0.2);
    list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));
    int ns_ = 1000;
    for (int j = 0; j < ns_; ++j)
        boxlist2[j] = new sphere(vec3(165 * drand48(), 165 * drand48(), 165 * drand48()), 10, white);

    list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns_, 0, 1), 15), vec3(-100, 270, 395));

    return new bvh_node(list, l, 0, FLT_MAX);
}

bvh_node *xyz_trans_smoke_box_()
{
    hitable **list = new hitable *[9];
    material *red__ = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(1, 1, 1)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *blue_ = new lambertian(new constant_texture(vec3(0.12, 0.12, 0.75)));
    material *ligh_ = new light(new constant_texture(vec3(15, 15, 15)));

    list[0] = new yz_rect(0, 555, 0, 555, 555, green);
    list[1] = new yz_rect(0, 555, 0, 555, 0, red__);
    list[2] = new xz_rect(213, 343, 227, 332, 554.5, ligh_);
    list[3] = new xz_rect(0, 555, 0, 555, 555, white);
    list[4] = new xz_rect(0, 555, 0, 555, 0, white);
    list[5] = new xy_rect(0, 555, 0, 555, 555, blue_);
    //list[6] = new xy_rect(0,555,0,555,0,white);
    hitable *box1 = free_translate(new box(vec3(), vec3(165, 165, 165), white), "xy", vec3(-30, -45, 0), vec3(130, 200, 65));
    hitable *box2 = free_translate(new box(vec3(), vec3(165, 330, 165), white), "zy", vec3(0, 30, 20), vec3(265, 60, 295));
    list[6] = new constant_medium(box2, 0.006, new constant_texture(vec3(0.8, 0.58, 0.)));
    list[7] = new constant_medium(box1, 0.008, new constant_texture(vec3(0.9, 0.2, 0.72)));
    return new bvh_node(list, 8, 0, FLT_MAX);
}

bvh_node *Bunny4k(hitable **&light_s, int &count_light)
{
    count_light = 1;
    light_s = new hitable *[count_light];
    ply_file bunny("PLYFiles.bak/ply/Bunny4K.ply");
    hitable **list = new hitable *[bunny.returnElementFace() + 15];
    material *red__ = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(1, 1, 1)));
    material *whit2 = new metal(new constant_texture(vec3(1, 1, 1)), 0.0);
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *blue_ = new lambertian(new constant_texture(vec3(0.12, 0.12, 0.75)));
    material *ligh_ = new light(new constant_texture(vec3(15, 15, 15)));
    material *glass = new dielectric(new constant_texture(vec3(1, 1, 1)), 1.5);
    material *glas2 = new dielectric(new constant_texture(vec3(0.8, 0.8, 1)), 1.5);
    int l = 0;
    list[l++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[l++] = new yz_rect(0, 555, 0, 555, 0, red__);

    list[l++] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_), "", vec3(), vec3(213, 545, 227));
    light_s[0] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_), "", vec3(), vec3(213, 545, 227));
    list[l++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[l++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[l++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, blue_));
    for (int i = 0; i < bunny.returnElementFace(); i++)
    {
        float change = 2000;
        vec3 ver[3];
        bunny.getThreeVertex(ver, i);
        ver[0] *= change;
        ver[1] *= change;
        ver[2] *= change;
        //cout<<ver[1]<<endl;
        list[l++] = free_translate(new triangle(ver, glass), "y", vec3(50, 180, 0), vec3(250, 20, 250));
        //list[l++] = new triangle(ver, red__);

        //cout << i<<endl<<endl;
    }
    //cout << "over"<< endl;
    cout << l << endl;
    return new bvh_node(list, l, 0, FLT_MAX);
}

bvh_node *xyz_trans_box_(hitable **&light_s, int &count_light)
{
    count_light = 3;
    light_s = new hitable *[count_light];
    hitable **list = new hitable *[11];
    material *red__ = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(1, 1, 1)));
    material *whit2 = new metal(new constant_texture(vec3(1, 1, 1)), 0.0);
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *blue_ = new lambertian(new constant_texture(vec3(0.12, 0.12, 0.75)));
    material *ligh_ = new light(new constant_texture(vec3(15, 15, 15)));
    material *glass = new dielectric(new constant_texture(vec3(1, 1, 1)), 1.5);
    material *glas2 = new dielectric(new constant_texture(vec3(0.8, 0.8, 1)), 1.5);

    list[0] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[1] = new yz_rect(0, 555, 0, 555, 0, red__);
    //list[2] = new xz_rect(213,343,227,332,554,ligh_);
    //list[2] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_),"",vec3(),vec3(213,554,227));
    list[2] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_), "xyz", vec3(20, 20, 20), vec3(90, 90, 90));
    light_s[0] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_), "xyz", vec3(20, 20, 20), vec3(90, 90, 90));
    list[3] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[4] = new xz_rect(0, 555, 0, 555, 0, white);
    list[5] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, blue_));
    //list[6] = new xy_rect(0,555,0,555,0,white);
    list[7] = free_translate(new box(vec3(), vec3(165, 165, 165), glass), "xy", vec3(-30, -45, 0), vec3(130, 200, 65));
    list[6] = free_translate(new box(vec3(), vec3(165, 330, 165), whit2), "zy", vec3(0, 30, 20), vec3(265, 60, 295));
    light_s[1] = list[8] = new sphere(vec3(290, 50, 100), 50, glass);
    light_s[2] = list[9] = new sphere(vec3(180, 40, 100), 40, glas2);
    vec3 vertex[3] = {vec3(480, 50, 100), vec3(450, 200, 70), vec3(280, 180, 260)};
    list[10] = new triangle(vertex, whit2);
    //list[7] = new translate(new rotate_z(new rotate_y(new box(vec3(), vec3(165, 330, 165), white), 30), 20), vec3(265, 60, 295));
    return new bvh_node(list, 11, 0, FLT_MAX);
}

bvh_node *xyz_trans_box_2(hitable **&light_s, int &count_light)
{
    count_light = 3;
    light_s = new hitable *[count_light];
    hitable **list = new hitable *[11];
    material *red__ = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(1, 1, 1)));
    material *whit2 = new metal(new constant_texture(vec3(1, 1, 1)), 0.4);
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *blue_ = new lambertian(new constant_texture(vec3(0.12, 0.12, 0.75)));
    material *ligh_ = new light(new constant_texture(vec3(15, 15, 15)));
    material *glass = new dielectric(new constant_texture(vec3(1, 1, 1)), 1.5);
    material *glas2 = new dielectric(new constant_texture(vec3(0.8, 0.8, 1)), 1.5);

    int nx_earth, ny_earth, nn_earth;
    unsigned char *earth_data = stbi_load("picture/earth_map.jpg", &nx_earth, &ny_earth, &nn_earth, 0);
    texture *P_data = new image_texture(earth_data, nx_earth, ny_earth);

    texture *pertext = new noise_marble2_texture(0.7);

    list[0] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[1] = new yz_rect(0, 555, 0, 555, 0, red__);
    //list[2] = new xz_rect(213,343,227,332,554,ligh_);
    //list[2] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_),"",vec3(),vec3(213,554,227));
    list[2] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_), "xyz", vec3(20, 20, 20), vec3(90, 90, 90));
    light_s[0] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_), "xyz", vec3(20, 20, 20), vec3(90, 90, 90));
    list[3] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[4] = new xz_rect(0, 555, 0, 555, 0, white);
    list[5] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, blue_));
    //list[6] = new xy_rect(0,555,0,555,0,white);
    list[7] = free_translate(new box(vec3(), vec3(165, 165, 165), glass), "xy", vec3(-30, -45, 0), vec3(130, 200, 65));
    list[6] = free_translate(new box(vec3(), vec3(165, 330, 165), whit2), "zy", vec3(0, 30, 20), vec3(265, 60, 295));
    light_s[1] = list[8] = new sphere(vec3(290, 50, 100), 50, new lambertian(pertext));
    light_s[2] = list[9] = new sphere(vec3(180, 40, 100), 40, new dielectric(P_data, 1.5, 0.5));
    vec3 vertex[3] = {vec3(480, 50, 100), vec3(450, 200, 70), vec3(280, 180, 260)};
    list[10] = new triangle(vertex, whit2);
    //list[7] = new translate(new rotate_z(new rotate_y(new box(vec3(), vec3(165, 330, 165), white), 30), 20), vec3(265, 60, 295));
    return new bvh_node(list, 11, 0, FLT_MAX);
}

bvh_node *xyz_water_box_(hitable **&light_s, int &count_light)
{
    count_light = 1;
    light_s = new hitable *[count_light];
    int count_list = 2;
    hitable **list = new hitable *[count_list];
    material *red__ = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(1, 1, 1)));
    material *whit2 = new metal(new constant_texture(vec3(1, 1, 1)), 0.0);
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *blue_ = new lambertian(new constant_texture(vec3(0.12, 0.12, 0.75)));
    material *ligh_ = new light(new constant_texture(vec3(15, 15, 15)));
    material *glass = new dielectric(new constant_texture(vec3(1, 1, 1)), 1.5);
    material *glas2 = new dielectric(new constant_texture(vec3(0.8, 0.8, 1)), 1.5);

    //list[0] = new flip_normals(new yz_rect(0,555,0,555,555,green));
    //list[1] = new yz_rect(0,555,0,555,0,red__);
    light_s[0] = list[0] = free_translate(new box(vec3(), vec3(130, 50, 105), ligh_), "", vec3(), vec3(213, 500, 227));
    //list[3] = new flip_normals(new xz_rect(0,555,0,555,555,white));
    //list[4] = new xz_rect(0,555,0,555,0,white);
    //list[5] = new flip_normals(new xy_rect(0,555,0,555,555,blue_));
    //list[6] = new water(vec3(555,90,555),vec3(0,80,0),1000,10);

    //int count_ = 6;
    list[1] = new water(0, 555, 0, 555, 20, 90, 9, 5);
    //vec3 vertex[3] = {vec3(0,50,0),vec3(0,50,50),vec3(50,50,50)};
    //list[6] = new triangle(vertex,white);

    /*
    int faceNumber = 2;
    vec3 vertex[3];
    float delta = 1 / float(faceNumber);
    material *glass_ = new dielectric(new constant_texture(vec3(1, 1, 1)), 1.33);
    material *white11 = new lambertian(new constant_texture(vec3(1, 0, 0)));
    material *white22 = new lambertian(new constant_texture(vec3(0, 0, 1)));
    float xx = 555,x = 0,zz = 555, z = 0,y = 80;
    int count_traingle = 6;
    float delta_x = xx - x, delta_z = zz - z;
    for (int u = 0; u < faceNumber; u++)
        for (int v = 0; v < faceNumber; v++)
        {
            float uu = float(u) / float(faceNumber);
            float vv = float(v) / float(faceNumber);
            vertex[0] = {x + uu * delta_x,
                         y + 0,
                         z + vv * delta_z};
            vertex[1] = {x + (uu + delta) * delta_x,
                         y + 0,
                         z + (vv + delta) * delta_z};
            vertex[2] = {x + (uu + delta) * delta_x,
                         y + 0,
                         z + vv * delta_z};
            list[count_traingle++] = new triangle(vertex, white11);

            vertex[0] = {x + uu * delta_x,
                         y + 0,
                         z + vv * delta_z};
            vertex[1] = {x + uu * delta_x,
                         y + 0,
                         z + (vv + delta) * delta_z};
            vertex[2] = {x + (uu + delta) * delta_x,
                         y + 0,
                         z + (vv + delta) * delta_z};
            list[count_traingle++] = new triangle(vertex, white11);
        }
*/
    return new bvh_node(list, count_list, 0, FLT_MAX);
}

bvh_node *xyz_box_()
{
    hitable **list = new hitable *[9];
    material *red__ = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(1, 1, 1)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *blue_ = new lambertian(new constant_texture(vec3(0.12, 0.12, 0.75)));
    material *ligh_ = new light(new constant_texture(vec3(15, 15, 15)));

    list[0] = new yz_rect(0, 555, 0, 555, 555, green);
    list[1] = new yz_rect(0, 555, 0, 555, 0, red__);
    list[2] = new xz_rect(213, 343, 227, 332, 554.5, ligh_);
    list[3] = new xz_rect(0, 555, 0, 555, 555, white);
    list[4] = new xz_rect(0, 555, 0, 555, 0, white);
    list[5] = new xy_rect(0, 555, 0, 555, 555, blue_);
    //list[6] = new xy_rect(0,555,0,555,0,white);

    //list[6] = new box(vec3(130,0,65),vec3(295,165,230),white);
    list[7] = free_translate(new box(vec3(), vec3(165, 165, 165), white), "xy", vec3(-30, -45, 0), vec3(130, 200, 65));
    list[6] = new box(vec3(265, 0, 295), vec3(430, 330, 460), white);
    list[8] = new sphere(vec3(130, 50, 200), 50, new dielectric(new constant_texture(vec3(1, 1, 1)), 1.5));

    return new bvh_node(list, 9, 0, FLT_MAX);
}

bvh_node *earth_map_shpere()
{
    int nx_earth, ny_earth, nn_earth;
    unsigned char *earth_data = stbi_load("picture/earth_map.jpg", &nx_earth, &ny_earth, &nn_earth, 0);
    texture *data = new image_texture(earth_data, nx_earth, ny_earth);
    hitable **list = new hitable *[2];
    list[0] = new sphere(vec3(0, 0, 0), 0.8, new lambertian(data));
    list[1] = new sphere(vec3(0, -1000, 0), 999.2, new metal(new constant_texture(vec3(0.2, 0.2, 0.2)), 0.));
    return new bvh_node(list, 2, 0, FLT_MAX);
}

bvh_node *two_perlin_sphere()
{
    texture *pertext = new noise_marble2_texture(3);
    hitable **list = new hitable *[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    return new bvh_node(list, 2, 0, FLT_MAX);
}

//随机生成许多球体，学习时学习用的
bvh_node *random_scene()
{
    int n = 485;
    hitable **list = new hitable *[n + 1];
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for (int a = -11; a < 11; a++)     //坐标 x
        for (int b = -11; b < 11; b++) //坐标 z
        {
            float choose_mat = drand48();
            vec3 center(float(a) + 0.9 * drand48(), 0.24, float(b) + 0.9 * drand48());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9)
            {
                if (choose_mat < 0.8) //lambertian
                    list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * drand48(), 0), 0.0, 1.0, 0.24, new lambertian(new constant_texture(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48()))));
                else if (choose_mat < 0.95) //metal
                    list[i++] = new sphere(center, 0.24, new metal(new constant_texture(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48()))), 0.5 * (1 + drand48())));
                else //dielectric
                    list[i++] = new sphere(center, 0.24, new dielectric(new constant_texture(vec3(1.0, 1.0, 1.0)), 1.5));
            }
        }
    list[i++] = new sphere(vec3(0, 1.5, 0), 1.5, new dielectric(new constant_texture(vec3(1.0, 1.0, 1.0)), 1.5));
    list[i++] = new sphere(vec3(-4, 1.5, 0), 1.5, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.2))));
    list[i++] = new sphere(vec3(4, 1.5, 0), 1.5, new metal(new constant_texture(vec3(0.7, 0.6, 0.6)), 0.0));

    return new bvh_node(list, n, 0, FLT_MAX);
}

bvh_node *nothing()
{
    hitable **list = new hitable *[1];
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    return new bvh_node(list, 1, 0, FLT_MAX);
}

camera cam1()
{
    vec3 lookform(0, 2.5, -9);
    vec3 lookat(0, 1.7, 0);
    float dist_to_focus = 10.0; //(lookform - lookat).length();
    float aperture = 0.0;
    return camera(lookform, lookat, vec3(0, 1, 0), 40, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
}

camera cam2()
{
    vec3 lookform(13, 2, 3);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0; //(lookform - lookat).length();
    float aperture = 0.0;
    return camera(lookform, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
}

camera cam3()
{
    vec3 lookform(278, 278, -700);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = 10.0; //(lookform - lookat).length();
    float aperture = 0.0;
    return camera(lookform, lookat, vec3(0, 1, 0), 40, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
}

camera cam4()
{
    vec3 lookform(13, 3, 2);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0; //(lookform - lookat).length();
    float aperture = 0.0;
    return camera(lookform, lookat, vec3(0, 1, 0), 30, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
}

camera cam5()
{
    vec3 lookform(278, 278, -520);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = 10.0; //(lookform - lookat).length();
    float aperture = 0.0;
    return camera(lookform, lookat, vec3(0, 1, 0), 40, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
}

#include "colorMode.h"

void ray_tracing()
{
    char png_name[] = "output_ue.png";
    srand((unsigned)time_t(0));
    unsigned char rgb[nx * ny * 3], *prgb = rgb;
    //FILE *fp = fopen(png_name, "wb");
    cout << png_name << "\nsize:\n"
         << nx << "px " << ny << "px\n";

    //ply_file text("PLYFiles.bak/ply/Bunny4K.ply");

    int count_light = 0;
    hitable **light_s;
    bvh_node *world = xyz_trans_box_2(light_s, count_light);
    hitable_list light_(light_s, count_light);
    //hitable *world = new hitable_list(list,5);
    //调用随机生成函数生成世界
    //hitable *world = random_scene();

    camera cam = cam3();
    cam.SettingCamModeJitterMaxCount(CAM_MODE_Jitter, ns);

    colorMode::colorSolution color_;
    colorMode::settingColorMode(color_, freeVec);

    printf("    %\b\b\b\b");
    for (int j = ny - 1; j >= 0; j--)
    {
        printf("%2d", int(double((ny - j) * 100) / double(ny)));
        printf("\b\b");
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++)
            {
                //float u = float(i) / float(nx);     //x轴方向相对于 nx 的百分比
                //float v = float(j) / float(ny);     //y轴方向相对于 ny 的百分比
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                //ray r(origin, lower_left_corner + u*horizontal + v*vertical);   //原点origin到图像某个坐标的光线
                ray r = cam.get_ray(u, v);
                vec3 p = r.poin_at_parameter(2.0);
                col += color_(wtf_lerp(r, world, &light_, 0));
                //col += first_lerp(r, world, 0);
            }
            //cout << col<<endl;
            col /= ns;
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //亮度增益
            //if(col.x()>1||col.y()>1||col.z()>1)
            //cout << col <<endl;
            //col = colorMode::colorMath::for_save(col);
            *(prgb++) = (unsigned char)(int(255.99 * col[0]));
            *(prgb++) = (unsigned char)(int(255.99 * col[1]));
            *(prgb++) = (unsigned char)(int(255.99 * col[2]));
        }
    }
    stbi_write_jpg(".\\ue_save_origin.jpg", nx, ny, 3, rgb, 0);
    //FILE *fp0 = fopen("ue_save_origin.png", "wb");
    //svpng(fp0, nx, ny, rgb, 0);
    //fclose(fp0);

    //for(int i = 0; i < nx*ny*3;i++)
    //    unsigned char text = rgb[i];

    cout << "\b\b/////////MedianFilter/////////" << endl;
    //medianFilters::medianFilters(rgb,nx,ny);

    //    FILE *fp1 = fopen("ue_save.png","wb");
    //    svpng(fp1, nx, ny, rgb, 0);
    //    fclose(fp1);

    cout << "///////BilateralFilter ///////" << endl;
    Filters::Filters(rgb, nx, ny, 20);

    stbi_write_jpg(".\\ue_work.jpg", nx, ny, 3, rgb, 0);
    //svpng(fp, nx, ny, rgb, 0);
    //fclose(fp);
    printf("Finish!\n");
    system("pause");
}

int main()
{
    ray_tracing();
}