#include "leo-raytracer.h"

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <chrono>

// RAY-TRACER //
// Leo Martin (2025) //


// render image
int main() {
	const int image_width = 480;
	const int image_height = 480;
	
	const float start_x = -2;
	const float start_y = 2;

	const int samples = 3;
	const int max_bounces = 3;

    const float pixel_size = -2 * start_x / image_width;

	const float center_pixel = pixel_size / 2;
	const int height = image_height / pixel_size;
    const int width = image_width / pixel_size;

	// initiate scene (populate with meshes)
	MeshScene scene;

    scene.add(make_shared<Mesh>("objects/top-wall.obj"));
    scene.add(make_shared<Mesh>("objects/bottom-wall.obj"));
    scene.add(make_shared<Mesh>("objects/left-wall.obj"));
    scene.add(make_shared<Mesh>("objects/right-wall.obj"));
    scene.add(make_shared<Mesh>("objects/back-wall.obj"));
    scene.add(make_shared<Mesh>("objects/reflector.obj"));
    scene.add(make_shared<Mesh>("objects/monke.obj"));
 	
	auto render_start = std::chrono::high_resolution_clock::now();
	// render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n"; // PPM header 
    color pixel_color;
    float x_pos, y_pos;
    //auto row_start = chrono::high_resolution_clock::now();


	for (int j = 0; j < image_height; j++) { // row
		//auto row_start = chrono::high_resolution_clock::now();
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush; // progress meter
		//auto row_start = chrono::high_resolution_clock::now();

		y_pos = start_y - (pixel_size * j) - center_pixel;
		for (int i = 0; i < image_width; i++) { // column
		    x_pos = start_x + (pixel_size * i) + center_pixel;
			point3 camera_start(0,0,5);
			vec3 cell_center(x_pos,y_pos,2); // cell start has to not clip through the object
			vec3 ray_direction = cell_center - camera_start;
		    ray_direction = normalize(ray_direction);
            ray render_ray(cell_center, ray_direction);
			

			color path_color = scene.trace_path(render_ray, samples, max_bounces);
		    write_color(std::cout, path_color);

		    //chrono::duration<double> elapsed_time = render_end - render_start;

		}
    }
	auto render_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_time = render_end - render_start;
	std::clog << "\rRender Done in: " << elapsed_time.count() << "sec\n";
}
				



