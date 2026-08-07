#ifndef SPHERE_H
#define SPHERE_H

#include "rtweekend.h"
#include "hittable.h"

class sphere : public hittable {
    public:
        sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {};

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            /* 
            * Sphere formula: (C-P) * (C-P) = r^2
            * Sub in P(t) = Q + td and FOIL into quadratic equation
                t^2(d * d) - 2t(d * (C-Q)) + (C - Q)*(C - Q) - r^2 = 0
            * Use quadratic formula to solve for the discriminant
                t = (-b +- sqrt(b^2 - 4ac)) / 2a
                a = d*d
                b = -2d * (C-Q)
                c = (C-Q)*(C-Q) - r^2
            * Simplified variables
                a = d.length_squared(); // eual to d*d
                * since b can be substituted with -2h where h = d*(C-Q)
                * and we solve for h = b/-2
                h = d*(C-Q)
                c = oc.length_squared() - radius^2
            * Discriminant
                if negative, the ray doesnt hit sphere (no t solution)
                if zero, the ray hits the edge of sphere (1 solution)
                if positive, the ray goes thru the sphere (2 solutions)
            */ 

            // C - Q, vector from origin to center of sphere
            vec3 oc = center - r.origin();
            // d, ray direction
            vec3 d = r.direction();

            auto a = d.length_squared();
            auto h = dot(d, oc);
            auto c = oc.length_squared() - (radius*radius);
            auto discriminant = (h * h) - (a*c);

            if (discriminant < 0) {
                return false;
            }
            
            // here, ray hit the object
            auto squared_root = std::sqrt(discriminant);

            // find nearest root that lies in acceptable range of ray_tmin and ray_tmax
            auto root = (h - squared_root) / a;
            // fell outside acceptable range, try larger t
            if (!ray_t.surrounds(root)) {
                root = (h + squared_root) / a;
                // if this value doesnt work either
                if (!ray_t.surrounds(root)) {
                    return false;
                }
            }

            // update hit_record rec
            rec.t = root;
            rec.p = r.at(root);
            vec3 outward_normal = (rec.p - center) / radius; // divide by radius to normalize
            rec.set_face_normal(r, outward_normal);

            return true;
        }

    private:
        point3 center;
        double radius;
};

#endif