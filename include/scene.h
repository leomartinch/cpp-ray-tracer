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
		closest_hit.face_id = -1;
        double closest_time = std::numeric_limits<double>::max();

        for (const auto& mesh : meshes) {
		//for (const auto& bounding_box : bounding_boxes) {

		    // add bounding box hierarchy!!!
			//BoundHit bound_hit = mesh->bound_hit(render_ray);
			//if (bound_hit.is_hit) {
			//	continue;
			//}

            RayHit temp_hit = mesh->hit(render_ray);
            if (temp_hit.hit_time > 0 && temp_hit.hit_time < closest_time) {
               closest_time = temp_hit.hit_time;
		       temp_hit.hit_object = mesh.get(); // pointer to the object that was hit
               closest_hit = temp_hit;
            }
        }
        return closest_hit;
    }


// have to clean up the names etc (error correction from chatgpt (only one line was wrong but he still changed many names)
color trace_path(ray render_ray, const int& samples, const int& max_bounces) const {
    color final_color(0, 0, 0);

	// first object hit
    RayHit primary_hit;
    primary_hit.hit_time = -1;  // no hit yet
    double closest_time = std::numeric_limits<double>::max();
    for (const auto& mesh : meshes) {
        if (!mesh->bound_hit(render_ray)) {
            continue;
        }
        RayHit temp_hit = mesh->hit(render_ray);
        if (temp_hit.hit_time > 0.0001 && temp_hit.hit_time < closest_time) {
            closest_time = temp_hit.hit_time;
            temp_hit.hit_object = mesh.get(); // record which object was hit
            primary_hit = temp_hit;
        }
    }

    if (primary_hit.hit_time <= 0.0001) {
        return final_color;
    }

    Mesh* primary_mesh = dynamic_cast<Mesh*>(primary_hit.hit_object);
    point3 primary_hit_point = render_ray.at(primary_hit.hit_time);
    vec3 primary_normal = primary_mesh->get_normal_vector(primary_hit.face_id, render_ray);

    float primary_roughness = primary_mesh->get_roughness();
    color primary_emission = primary_mesh->get_emission();
    color primary_diffuse = primary_mesh->get_color();

	// subsequent bounce hits
    for (int i = 0; i < samples; i++) {
        color throughput(1, 1, 1);
        color sample_color(0, 0, 0);

        sample_color += throughput * primary_emission;
        throughput = throughput * primary_diffuse;

        vec3 primary_specular = primary_mesh->get_specular_direction(render_ray, primary_normal);
        vec3 primary_diffuse_direction  = primary_mesh->get_diffuse_direction(primary_normal);
        vec3 reflection_direction = lerp(primary_diffuse_direction, primary_specular, primary_roughness);
        ray current_ray(primary_hit_point, reflection_direction);

        for (int j = 1; j < max_bounces; j++) {
            RayHit hit;
            hit.hit_time = -1;  // no hit
            double closest_hit = std::numeric_limits<double>::max();

            for (const auto& mesh : meshes) {
                if (!mesh->bound_hit(current_ray)) {
                    continue;
                }
                RayHit temp_hit = mesh->hit(current_ray);
                if (temp_hit.hit_time > 0.0001 && temp_hit.hit_time < closest_hit) {
                    closest_hit = temp_hit.hit_time;
                    temp_hit.hit_object = mesh.get();
                    hit = temp_hit;
                }
            }

            // if ray hits object, update the ray and throughput
            if (hit.hit_time > 0.0001) {
                Mesh* hit_mesh = dynamic_cast<Mesh*>(hit.hit_object);
                point3 hit_point = current_ray.at(hit.hit_time);
                vec3 normal = hit_mesh->get_normal_vector(hit.face_id, current_ray);
                color emission = hit_mesh->get_emission();
                color diffuse = hit_mesh->get_color();
                float roughness = hit_mesh->get_roughness();

                sample_color += throughput * emission;
                throughput = throughput * diffuse;

                // compute new reflection vector
                vec3 specular_direction = hit_mesh->get_specular_direction(current_ray, normal);
                vec3 diffuse_direction = hit_mesh->get_diffuse_direction(normal);
                vec3 reflection_direction = lerp(diffuse_direction, specular_direction, roughness);
                current_ray = ray(hit_point, reflection_direction);
            }
            else {
                break; // no more hits 
            }
        }
        final_color += sample_color;
    }
    return final_color / samples;
}


private:
    std::vector<std::shared_ptr<Mesh>> meshes;
};

