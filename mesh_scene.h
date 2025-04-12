#include <vector>
#include <memory>
#include "mesh.h"
#include "ray.h"

class MeshScene {
public:
    void add(const std::shared_ptr<Mesh>& mesh) {
        meshes.push_back(mesh);
    }

    // cast a ray and return the closest hit among all meshes.
    RayHit hit(const ray& render_ray) const {
        RayHit closest_hit;
        closest_hit.hit_time = -1;  // no hit
        double closest_time = std::numeric_limits<double>::max();

        for (const auto& mesh : meshes) {
		    // add bounding box hierarchy!!!
            RayHit temp_hit = mesh->hit(render_ray);
            if (temp_hit.hit_time > 0 && temp_hit.hit_time < closest_time) {
                closest_time = temp_hit.hit_time;
                closest_hit = temp_hit;
				temp_hit.hit_object = mesh.get(); // pointer to the object that was hit
            }
        }
        return closest_hit;
    }

private:
    std::vector<std::shared_ptr<Mesh>> meshes;
};

