#include "vec3.h"
#include "color.h"
#include "ray.h"

#include <iostream>

// RAY-TRACER //
// Leo Martin (2025) //


bool get_ray_mesh_intersection(ray render_ray, point3 vertices[3]) {
    vec3 edge_1 = vertices[1] - vertices[0];
    vec3 edge_2 = vertices[2] - vertices[0];

    vec3 p_vector = cross(render_ray.direction(), edge_2);
    double determinant = dot(edge_1, p_vector);

    const float kepsilon = 1e-8f;
    if (std::fabs(determinant) < kepsilon) {
        return false; // ray is parallel or invalid
    }

    float inverse_determinant = 1 / determinant;
    vec3 ray_to_vertice_0 = render_ray.origin() - vertices[0];

    double u = dot(ray_to_vertice_0, p_vector) * inverse_determinant;
    if (u < 0 || u > 1) {
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

    return false; // ray intersects triangle behind camera
}



int main() {
    // image
	const int image_width = 256;
	const int image_height = 256;
	
	const float start_x = -2;
	const float start_y = 1.5;

    const float pixel_size = -2 * start_x / image_width;

	const float center_pixel = pixel_size / 2;
	const int height = image_height / pixel_size;
    const int width = image_width / pixel_size;

    point3 vertices[3] = {point3(-2.0, 0.0, -3.0),point3(0.0, 2.0, -3.0),point3(2.0, 0.0, -3.0)}; // mesh triangle

	float x_pos, y_pos;
	// render	
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n"; // PPM header
color pixel_color;
	for (int j = 0; j < image_height; j++) {
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
		y_pos = start_y - (pixel_size * j) - center_pixel;
		for (int i = 0; i < image_height; i++) {
		    x_pos = start_x + (pixel_size * i) + center_pixel;
			
			vec3 cell_center(x_pos,y_pos,0);
			vec3 ray_direction(0, 0, -1);

		    ray_direction = unit_vector(ray_direction);
            ray render_ray(cell_center, ray_direction);

		    bool hit = get_ray_mesh_intersection(render_ray, vertices);

		    
			if (hit) {
				    pixel_color = color(1, 0, 0);
			} else {
					pixel_color = color(0, 0, 0);
			}	

            write_color(std::cout, pixel_color);
		}
    }
	
	std::clog << "\rDone.                            \n";
}
				



