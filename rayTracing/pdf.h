#ifndef PDFH
#define PDFH

#include "vec3.h"
#include "onb.h"
#include "drand48.h"
#include "hitable.h"

class mixture_pdf : public pdf
{
public:
    mixture_pdf(pdf *p1, pdf *p2)
    {
        _p[0] = p1;
        _p[1] = p2;
    }

    /*
    @brief: we get the value of pdf function by this interface
    @param:    the direction of location
    @retur: the value of the pdf function
    */
    virtual float value(const vec3 &direction) const
    {
        return 0.5 * _p[0]->value(direction) + 0.5 * _p[1]->value(direction);
    }

    /*
    @brief: generate a random number with a Probability model
    @param: none
    @retur:    the Three-dimensional random vector
    */
    virtual vec3 generate() const
    {
        if (drand48() < 2.0 / 3.0)
            return _p[0]->generate();
        else
            return _p[1]->generate();
    }

private:
    pdf *_p[2];
};

class cosine_pdf : public pdf
{
public:
    //constructor
    cosine_pdf(const vec3 &w);

    /*
    @brief: we get the value of pdf function by this interface
    @param:    the direction of location
    @retur: the value of the pdf function
    */
    virtual float value(const vec3 &direction) const;

    /*
    @brief: generate a random number with a Probability model
    @param: none
    @retur:    the Three-dimensional random vector
    */
    virtual vec3 generate() const;

private:
    onb _uvw;
};

inline cosine_pdf::cosine_pdf(const vec3 &w)
{
    _uvw.build_from_w(w);
}

float cosine_pdf::value(const vec3 &direction) const
{
    float cosine = dot(unit_vector(direction), _uvw.w());
    if (cosine > 0.)
        return cosine / M_PI;
    else
        return 0.;
}

vec3 cosine_pdf::generate() const
{
    return _uvw.local(random_cosine_direction());
}

class hit_pdf : public pdf
{
public:
    /*
    @param: info -> Geometry information
            origion -> the point of intersection
    */
    hit_pdf(hitable *ptr_, const vec3 &origion)
        : ptr(ptr_), _o(origion)
    {
    }

    /*
    @brief: we get the value of pdf function by this interface
    @param:    the direction of location
    @retur: the value of the pdf function
    */
    virtual float value(const vec3 &direction) const
    {
        return ptr->pdf_value(_o, direction);
    }

    /*
    @brief: generate a random number with a Probability model
    @param: none
    @retur:    the Three-dimensional random vector
    */
    virtual vec3 generate() const
    {
        return ptr->random(_o);
    }

private:
    vec3 _o;

    hitable *ptr;
};

#endif