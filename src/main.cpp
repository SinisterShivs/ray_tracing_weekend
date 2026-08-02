#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

/*
    Given center point, radius, and a ray,
    determine whether the ray r hits the sphere,
    returns true if it does.
*/
bool hit_sphere(const point3& center, double radius, const ray& r) {
    /* 
    * Sphere formula: (C-P) * (C-P) = r^2
    * Sub in P(t) = Q + td and FOIL into quadratic equation
        t^2(d * d) - 2t(d * (C-Q)) + (C - Q)*(C - Q) - r^2 = 0
    * Use quadratic formula to solve for the discriminant
        t = (-b +- sqrt(b^2 - 4ac)) / 2a
        a = d*d
        b = -2d * (C-Q)
        c = (C-Q)*(C-Q) - r^2
    * Discriminant
        if negative, the ray doesnt hit sphere (no t solution)
        if zero, the ray hits the edge of sphere (1 solution)
        if positive, the ray goes thru the sphere (2 solutions)
    */ 

    // C - Q, vector from origin to center of sphere
    vec3 oc = center - r.origin();
    // d, ray direction
    vec3 d = r.direction();

    auto a = dot(d, d);
    auto b = dot((-2 * d), oc);
    auto c = dot(oc, oc) - (radius * radius);
    auto discriminant = (b * b) - (4 * (a * c));
    
    return (discriminant >= 0);

}

color ray_color(const ray& r) {
    if (hit_sphere(point3(0, 0, -1), 0.5, r)) {
        return color (1, 0, 0);
    }

    color start_value = color(1.0, 1.0, 1.0);
    color end_value = color(0.5, 0.7, 1.0);
    // normalize vector so that we can linear interpolate
    vec3 unit_direction = unit_vector(r.direction());
    // convert [-1, 1] to [0, 1]
    auto a = 0.5 * (unit_direction.y() + 1.0);
    // linear interpolation formula
    auto blendedValue = ((1.0 - a) * start_value) + (a * end_value);

    return color(blendedValue);
}

int main() {
    // Image
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    // find image height based on width and ratio, ensure at least 1
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera
    auto focal_length = 1.0;
    /* get viewport, In order for our viewport proportions to exactly match our
    image proportions, we use the calculated image aspect ratio to determine 
    our final viewport width.*/ 
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width)/image_height);
    auto camera_center = point3(0, 0, 0);

    // vectors across viewport
    auto viewport_u = vec3(viewport_width, 0, 0); // viewport's left edge to right edge
    auto viewport_v = vec3(0, -viewport_height, 0); // -y to match how we scan pixels, top to bottom
    
    // calculate delta vector of distance between each pixel
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // get upper left vector, where we start drawing
    auto viewport_upper_left = camera_center 
                                - vec3(0, 0, focal_length) 
                                - viewport_u/2 
                                - viewport_v/2;

    // inset pixels so that all fits inside screen
    // localtion of pixel (0, 0), insetted so that the edge of pixel lines up with edge of viewport
    auto pixel00_loc = viewport_upper_left + (0.5 * (pixel_delta_u + pixel_delta_v));

    // PPM format header
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int i = 0; i < image_height; i++) {
        std::clog << "\rScanlines reamining: " << (image_height - i) << '\n' << std::flush;
        for (int j = 0; j < image_width; j++) {
            // get the center of each pixel
            auto pixel_center = pixel00_loc + (i * pixel_delta_v) + (j * pixel_delta_u);
            // destination - origin, gives the vector of direction
            auto ray_direction = pixel_center - camera_center;
            // get the ray starting from camera_center and has direction ray_direction
            ray r = ray(camera_center, ray_direction);
            // get color for this ray
            color pixel_color = ray_color(r);
            // display color on screen
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.   \n";

    return 0;
}