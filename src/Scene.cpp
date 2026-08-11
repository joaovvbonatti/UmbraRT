#include "Scene.h"
#include <random>

#include "glm/detail/func_geometric.inl"

Scene randomScene(int seed) {
    Scene scene;

    std::mt19937 rng(seed);

    auto randomFloat = [&](float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    };

    auto randomVec3 = [&](float min, float max)
    {
        return glm::vec3(
            randomFloat(min, max),
            randomFloat(min, max),
            randomFloat(min, max)
        );
    };


    for (int i = -6; i < 6; i++) {
        for (int j = -6; j < 6; j++) {

            glm::vec3 position = randomVec3(-50.0, 50.0);
            position.y = randomFloat(0.0, 50.0);

            float radius = randomFloat(1.0, 3.0);

            float materialChance = randomFloat(0.0, 1.0);
            int materialType;
            if (materialChance > 0.5)
                materialType = Material::METAL;
            else
                materialType = Material::DIFFUSE;

            glm::vec3 albedo = randomVec3(0.5, 1.0);

            float emission = randomFloat(0.0, 1.0);
            if (emission < 2)
                emission = 0.0;
            Material material = Material(materialType, albedo, 0.1, emission, 0.0);

            Sphere randomSphere = Sphere(position, radius, material);

            scene.spheres.emplace_back(randomSphere);
        }
    }

    for (int i = -3; i < 3; i++) {
        for (int j = -3; j < 3; j++) {

            glm::vec3 position = randomVec3(-50.0, 50.0);
            position.y = randomFloat(0.0, 50.0);

            float materialChance = randomFloat(0.0, 1.0);
            int materialType;
            if (materialChance > 0.5)
                materialType = Material::METAL;
            else
                materialType = Material::DIFFUSE;

            glm::vec3 albedo = randomVec3(0.5, 1.0);
            glm::vec3 size = glm::vec3(randomFloat(1.0, 3.0));

            float emission = randomFloat(0.0, 1.0);
            if (emission < 2)
                emission = 0.0;
            Material material = Material(materialType, albedo, 0.1, emission, 0.0);

            Box randomBox = Box(position, size, material);

            scene.boxes.emplace_back(randomBox);
        }
    }


    glm::vec3 position = glm::vec3(30.0, 4.0, 0.0);
    float radius = 5.0;
    glm::vec3 albedo = glm::vec3(1.0, 0.0, 0.0);
    Material material = Material(Material::DIFFUSE, albedo, 0.0, 5.0, 0.0);
    scene.spheres.emplace_back(position, radius, material);

    return scene;
}
