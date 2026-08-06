#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class hit_record {
    public:
        point3 p; // point where it hits
        vec3 normal; // surface normal at point
        double t; // value t when it hits
        bool front_face; // the front face

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // sets the normal vector and front_face.
            // outward_normal assumed to be unit length

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable {
    public:
        virtual ~hittable() = default;

        /*
            Given center point, radius, and a ray,
            determine whether the ray r hits the object
        */
        virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif