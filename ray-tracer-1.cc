#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

color ray_color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}


bool get_ray_mesh_intersection(ray render_ray, point3 vertices[3]) {
    vec3 edge_1 = vertices[1] - vertices[0];
    vec3 edge_2 = vertices[2] - vertices[0];

    vec3 p_vector = cross(render_ray.direction(), edge_2);
    double determinant = dot(edge_1, p_vector);

    const float kepsilon = 1e-8f;
    if (std::fabs(determinant) < kepsilon) { //abs(determinant) < 1e-8:
        return false; // ray is parallel or invalid
    }

    float inverse_determinant = 1 / determinant;
    vec3 ray_to_vertice_0 = render_ray.origin() - vertices[0];

    double u = dot(ray_to_vertice_0, p_vector) * inverse_determinant;
    if (u < 0 || u > 1) {// # ...
        return false;
    }

    vec3 q = cross(ray_to_vertice_0, edge_1);
    double v = dot(render_ray.direction(), q) * inverse_determinant;
    if (v < 0 || (u + v) > 1) {
        return false;
	}

    double t = dot(edge_2, q) * inverse_determinant;
    if (t > 0){
        //intersection_point = add(ray_origin, multiply_scalar(ray_direction, t))
        return true; //intersection_point
    }

    return false; //# ray intersects triangle behind camera
}




int main() {
    // image
	int image_width = 256;
	int image_height = 256;

    point3 vertices[3] = {point3(-2.0, 0.0, -3.0),point3(0.0, 2.0, -3.0),point3(2.0, 0.0, -3.0)};

	// render	
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
		/*
	for (int j = 0; j < image_height; j++) {
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
		for (int i = 0; i < image_height; i++) {
		    //auto pixel_color = color(double(i)/(image_width-1), double(j)/(image_height-1), 0);

		    //auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            //auto ray_direction = pixel_center - camera_center;
			vec3 camera_center(0.0,0.0,3);
		    vec3 ray_direction(0.0,0.0,4);
            ray r(camera_center, ray_direction);

		    bool hit = get_ray_mesh_intersection(r, vertices);

            //color pixel_color = ray_color(r);
			color pixel_color;
			if (hit) {
				    color pixel_color = color(1, 0, 0);
			} else {
					color pixel_color = color(0, 0, 0);
			}
			std::cout << j << i << "\n";

            write_color(std::cout, pixel_color);
		}
    }
	*/
		
		vec3 camera_center(0.0,0,0);
		    //vec3 ray_direction(0.0,1,0.2);
			vec3 ray_direction(0.0, 0, -1.0); // now the z component is negative

            ray r(camera_center, ray_direction);

	
bool hit = get_ray_mesh_intersection(r, vertices);

            //color pixel_color = ray_color(r);
			color pixel_color;
			if (hit) {
					std::cout << "is hit\n";
			} else {
					std::cout << "is not hit\n";
			}
			//std::cout << j << i << "\n";


	std::clog << "\rDone.                            \n";
}
				



