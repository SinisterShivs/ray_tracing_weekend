/*
    * Abstract class for material.
    * A material needs to:
    *   Produce a scattered ray (or absorb the ray)
    *   If scattered, define attenuation (strength) of ray
*/

#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material {
    public:
        virtual ~material() = default;

        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation) const {
            return false;
        }
};

#endif