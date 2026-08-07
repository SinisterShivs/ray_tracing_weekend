#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

class camera {
    public:
    // Image
    double aspect_ratio = 1.0;      // width/height
    int image_width = 400;          // rendered image in pixel count
    int samples_per_pixel = 10;     // count of random samples per pixel
    int max_depth = 10;             // max number of ray bounces before ending
    
    void render(const hittable& world) {
        initialize();

        // Render
        // PPM format header
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int i = 0; i < image_height; i++) {
            std::clog << "\rScanlines reamining: " << (image_height - i) << '\n' << std::flush;
            for (int j = 0; j < image_width; j++) {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    // a random ray within 0.5 of that pixel
                    ray r = get_ray(j, i);
                    // add to buffer
                    pixel_color += ray_color(r, max_depth,world);
                }
                
                // Averaging all the colors that the rays return (anti-aliasing)
                write_color(std::cout, pixel_color * pixel_sample_scale);
            }
        }

        std::clog << "\rDone.   \n";
    }

    private:
        int image_height;           // rendered image height
        point3 camera_center;       // camera center
        point3 pixel00_loc;         // location of pixel 00
        vec3 pixel_delta_v;         // offset to pixel to right
        vec3 pixel_delta_u;         // offset to pixel below
        double pixel_sample_scale;  // for anti-aliasing

        void initialize() {
            // find image height based on width and ratio, ensure at least 1
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            // Anti Aliasing
            pixel_sample_scale = 1.0 / samples_per_pixel;

             // Camera
            auto focal_length = 1.0;
            /* get viewport, In order for our viewport proportions to exactly match our
            image proportions, we use the calculated image aspect ratio to determine 
            our final viewport width. */ 
            auto viewport_height = 2.0;
            auto viewport_width = viewport_height * (double(image_width)/image_height);
            camera_center = point3(0, 0, 0);

            // vectors across viewport
            auto viewport_u = vec3(viewport_width, 0, 0); // viewport's left edge to right edge
            auto viewport_v = vec3(0, -viewport_height, 0); // -y to match how we scan pixels, top to bottom

            // calculate delta vector of distance between each pixel
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // get upper left vector, where we start drawing
            auto viewport_upper_left = camera_center 
                                - vec3(0, 0, focal_length) 
                                - viewport_u/2 
                                - viewport_v/2;

            // inset pixels so that all fits inside screen
            // localtion of pixel (0, 0), insetted so that the edge of pixel lines up with edge of viewport
            pixel00_loc = viewport_upper_left + (0.5 * (pixel_delta_u + pixel_delta_v));
        }

        /*
            * Given point i and j (pixel location),
            * construct a ray from origin, pointing randomly around that location.
        */ 
        ray get_ray(int i, int j) const {
            auto offset = sample_square();
            // added offset's value to i and j for anti-aliasing
            auto pixel_sample = pixel00_loc + 
                                ((i + offset.x()) * pixel_delta_u) + 
                                ((j + offset.y()) * pixel_delta_v);
            
            auto ray_origin = camera_center;
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        // Returns vector to a random point within 0.5 of origin (0, 0)
        vec3 sample_square() const {
            return vec3(random_double() -0.5, random_double() -0.5, 0);
        }

        /*
            * given ray r and the world, determine if the ray hits any objects.
            * If hits, calculate the bounced ray and recurse until <depth> is reached.
        */
        color ray_color(const ray& r, int depth, const hittable& world) {
            // if depth limit reached, no more light is gathered
            if (depth <= 0) {
                return color(0, 0, 0); // black
            }

            hit_record rec;

            // 0.001 to avoid shadow acne
            if (world.hit(r, interval(0.001, infinity), rec)) {
                vec3 direction = random_on_hemisphere(rec.normal);
                // recursive, shoots another ray from rec.p (the contact point with the hittable)
                // the bounced ray is is 0.5 times weaker.
                return 0.5 * ray_color(ray(rec.p, direction), depth-1, world);
            }   

            /* sky gradient */
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
};
#endif