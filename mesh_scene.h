#include <vector>
#include <memory>
#include "mesh.h"
#include "ray.h"

class MeshScene {
public:
    // add mesh to the scene
    void add(const std::shared_ptr<Mesh>& mesh) {
        meshes.push_back(mesh);
    }

    // cast a ray and return the closest hit among all meshes in the scene
    RayHit hit(const ray& render_ray) const {
        RayHit closest_hit;
        closest_hit.hit_time = -1;  // no hit
        double closest_time = std::numeric_limits<double>::max();

        for (const auto& mesh : meshes) {
		    // add bounding box hierarchy!!!

            RayHit temp_hit = mesh->hit(render_ray);
            if (temp_hit.hit_time > 0 && temp_hit.hit_time < closest_time) {
                closest_time = temp_hit.hit_time;
				temp_hit.hit_object = mesh.get(); // pointer to the object that was hit
                closest_hit = temp_hit;
            }
        }
        return closest_hit;
    }

	color trace_path(ray render_ray, const int& samples, const int& max_bounces) const {
		color final_color = color(0,0,0);

		for (int i = 0; i < samples; i++) {
		    color throughput = color(1,1,1);
		    ray current_ray = render_ray;

			for (int j = 0; j < max_bounces; j++) {
				RayHit hit;
				hit.hit_time = -1;  // no hit
				double closest_time = std::numeric_limits<double>::max();

				for (const auto& mesh : meshes) {
				    // add bounding box hierarchy!!!

				    RayHit temp_hit = mesh->hit(current_ray);
				    if (temp_hit.hit_time > 0 && temp_hit.hit_time < closest_time) {
						closest_time = temp_hit.hit_time;
						temp_hit.hit_object = mesh.get(); // pointer to the object that was hit
						hit = temp_hit;
				    }
				}

				if (hit.hit_time > 0.01) {
					Mesh* hit_mesh = dynamic_cast<Mesh*>(hit.hit_object);

				    point3 intersection_point = current_ray.at(hit.hit_time);
					vec3 normal_vector = hit_mesh->get_normal_vector(hit.face_id, current_ray);
					final_color += throughput * hit_mesh->get_emission();
				    throughput = throughput * hit_mesh->get_color();

					float roughness = hit_mesh->get_roughness();	
					vec3 specular_direction = hit_mesh->get_specular_direction(current_ray, normal_vector);
					vec3 diffuse_direction = hit_mesh->get_diffuse_direction(normal_vector);

					vec3 reflection_direction = lerp(specular_direction, diffuse_direction, roughness);
					current_ray = ray(intersection_point, reflection_direction);
				}
				else {
				    break;
				}
		    }
		}
		return final_color / samples;
	}

private:
    std::vector<std::shared_ptr<Mesh>> meshes;
};

