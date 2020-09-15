#ifndef BVH
#define BVH

#include "hitable.h"
#include "aabb.h"
#include <iostream>
#include <queue>

int box_x_compare(const void *a, const void *b)
{
    aabb box_left, box_right;
    hitable *ah = *(hitable**)a;
    hitable *bh = *(hitable**)b;
    //cout << "a" << endl;
    if(!ah->bounding_box(0,0,box_left)||!bh->bounding_box(0,0,box_right))
        std::cerr<<"no bounding box in bvh_node constructor\n";
    if(box_left.min().x() - box_right.min().x()<0.0)
        return -1;//left小于right
    else
        return 1;
}

int box_y_compare(const void *a, const void *b)
{
    aabb box_left, box_right;
    hitable *ah = *(hitable **)a;
    hitable *bh = *(hitable **)b;
    //cout <<"a"<<endl;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().y() - box_right.min().y() < 0.0)
        return -1;
    else
        return 1;
}

int box_z_compare(const void *a, const void *b)
{
    aabb box_left, box_right;
    hitable *ah = *(hitable **)a;
    hitable *bh = *(hitable **)b;
    //cout << "a" << endl;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().z() - box_right.min().z() < 0.0)
        return -1;
    else
        return 1;
}

class bvh_node : public hitable
{
    private:
        aabb box;
        hitable *left;
        hitable *right;
    public:
        bvh_node() {}
        bvh_node(hitable **l, int n, float time_0, float time_1);
        bvh_node(queue <hitable*> &que, float time_0, float time_1);
        virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
        virtual bool bounding_box(float t0, float t1, aabb &box) const;
        inline void copy(aabb &it_box, hitable *&it_left, hitable *&it_right)
        {
            it_box = box;
            it_left = left;
            it_right = right;
        }
};

bvh_node::bvh_node(queue <hitable*> &que, float time_0, float time_1)
{
    int size = que.size();
    hitable **list = new hitable*[size];
    for(int i = 0; i < size; )
    {
        list[i++] = que.front();
        que.pop();
    }
    bvh_node new_for_copy(list, size, 0, FLT_MAX);
    new_for_copy.copy(box,left,right);
}

bvh_node::bvh_node(hitable **l, int n, float time_0, float time_1)
{
    int axis = int(3*drand48());    //随机化
    if(axis == 0)                   //sort
        qsort(l,n,sizeof(hitable*),box_x_compare);
    else if(axis == 1)
        qsort(l,n,sizeof(hitable*),box_y_compare);
    else
        qsort(l,n,sizeof(hitable*),box_z_compare);//axis == 2
    if(n == 1){
        left = right = l[0];        //若只有一个物体
    }
    else if(n == 2){                //若只有两个物体
        left = l[0];
        right = l[1];
    }
    else {                          //其他则递归生成左子树和右子树
        left = new bvh_node(l,n/2,time_0,time_1);
        right = new bvh_node(l+n/2,n-n/2,time_0,time_1);
    }
    aabb box_left;
    aabb box_right;
    //创建个用于合并的左右儿子节点
    if(!left->bounding_box(time_0,time_1,box_left)||!right->bounding_box(time_0,time_1,box_right))
        std::cerr<<"no bounding box in bvh_node constructor\n";
    box = surrounding_box(box_left,box_right);  //生成所有子集并集的集合
    //cout << box.max() << " " << box.min() << endl;
}

bool bvh_node::bounding_box(float t0, float t1, aabb &b)
    const
{
    b = box;
    return true;
}

bool bvh_node::hit(const ray &r, float t_min, float t_max, hit_record &rec)
    const
{
    if(box.hit(r,t_min,t_max))  //光线击中目的 node节点所在的集合
    {
        hit_record left_record;
        hit_record right_record;
        //cout << t_max <<" "<<t_min<<endl;
        bool hit_left = left->hit(r, t_min, t_max, left_record);
        bool hit_right = right->hit(r,t_min, t_max, right_record);
        if(hit_left && hit_right)
        {
            //record the min ray_time
            rec = left_record.t<right_record.t? left_record:right_record;
            return true;
        }
        if(hit_left || hit_right)
        {
            //record the ray_time which is true
            rec = hit_left? left_record:right_record;
            return true;
        }
    }
    return false;
}

#endif