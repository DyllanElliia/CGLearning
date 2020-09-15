#pragma once

#define stds std::

#define VEC2_OUT

#include "lvgm\lvgm.h" //本人博客: https://www.cnblogs.com/lv-anchoret/category/1367052.html
#include <vector>
#include <algorithm>
using namespace lvgm;

class Mountain
{
public:
    typedef dvec2 valtype;

    typedef double (*_Fun)(const valtype &);

    Mountain() {}

    /*
    p: three position coordinates(in ordered or not)
    f: the function Ptr
    δ: the solution precision
    */
    Mountain(const valtype &p1, const valtype &p2, const valtype &p3, const double δ)
        : _δ(δ)
    {
        _positions.resize(3);
        _positions[0] = p1;
        _positions[1] = p2;
        _positions[2] = p3;
        sort();
    }

    static void setF(_Fun f)
    {
        _f = f;
    }

    void setδ(double delt)
    {
        _δ = delt;
    }

public:
    /*
    origion: the bad position
    vec1: bad position -> min position
    vec2: bad position -> mid position
    */
    valtype reflect(const valtype &origion, const valtype &vec1, const valtype &vec2)
    {
        return origion + (vec1 + vec2);
    }

    /*
    origion: the change position
    vec1: change position -> left position
    vec2: change position -> right position
    */
    valtype shrink(const valtype &origion, const valtype &vec1, const valtype &vec2)
    {
        return origion + (vec1 + vec2) / 4;
    }

    /*
    origion: the origion position
    vec1: origion position -> left position
    vec2: origion position -> right position
    */
    void compression(const valtype &origion, const valtype &vec1, const valtype &vec2)
    {
        _positions[0] = origion + min(vec1, vec2) / 2;
        _positions[1] = origion + max(vec1, vec2) / 2;
    }

    /*
    origion: the change position
    vec1: change position -> left position
    vec2: change position -> right position
    */
    valtype exter(const valtype &origion, const valtype &vec1, const valtype &vec2)
    {
        return origion - (vec1 + vec2) / 2;
    }

    void go()
    {
        double delt = (_positions[2] - _positions[0]).normal();
        static int i = 0;
        while (delt > _δ)
        {
            stds cout << ++i << "次  " << _positions[0] << "\t" << _positions[1] << "\t" << _positions[2] << stds endl;
            valtype t = reflect(_positions[2], _positions[1] - _positions[2], _positions[0] - _positions[2]);
            if (_f(t) < _f(_positions[0]))
            {
                valtype ex = exter(t, _positions[1] - t, _positions[0] - t);
                if (_f(ex) < _f(t))
                    _positions[2] = ex;
                else
                    _positions[2] = t;
            }
            else if (_f(t) > _f(_positions[2]))
            {
                valtype sh = shrink(t, _positions[1] - t, _positions[0] - t);
                if (_f(sh) < _f(_positions[2])) //反射点收缩
                    _positions[2] = sh;
                else //三角内部内缩
                {
                    sh = reflect(sh, _positions[1] - sh, _positions[0] - sh);
                    if (_f(sh) < _f(_positions[2]))
                        _positions[2] = sh;
                    else //针对原始点内缩，针对反射点收缩，都不管用，那么选择压缩
                        compression(_positions[0], _positions[1] - _positions[2], _positions[0] - _positions[2]);
                }
            }
            else
                _positions[2] = t;
            sort();
            delt = (_positions[2] - _positions[0]).normal();
        }
        stds cout << "\n最好点为" << _positions[0] << "\t精度为:" << _δ << stds endl
                  << "函数值为：" << _f(_positions[0]) << stds endl
                  << stds endl;
    }

protected:
    const valtype &min(const valtype &vec1, const valtype &vec2)
    {
        return _f(vec1) < _f(vec2) ? vec1 : vec2;
    }

    const valtype &max(const valtype &vec1, const valtype &vec2)
    {
        return _f(vec1) > _f(vec2) ? vec1 : vec2;
    }

    friend bool cmp(const valtype &pos1, const valtype &pos2)
    {
        return Mountain::_f(pos1) < Mountain::_f(pos2);
    }

    void sort()
    {
        stds sort(_positions.begin(), _positions.end(), cmp);
    }

private:
    stds vector<valtype> _positions; //min, mid, max  or  good, mid, bad

    double _δ;

    static _Fun _f;
};