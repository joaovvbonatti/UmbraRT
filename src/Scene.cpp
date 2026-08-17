#include "Scene.h"
#include <random>


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

Scene cornell() {
    Scene scene;

    Box light = Box(glm::vec3(0.0, 10.0, 0.0), glm::vec3(1.0, 0.01, 1.0), Material(Material::DIFFUSE, glm::vec3(1.0), 0.0, 1.0, 0.0));
    scene.boxes.push_back(light);

    Plane floor = Plane(glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0), Material(Material::DIFFUSE, glm::vec3(1.0), 0.0, 0.0, 0.0));
    scene.planes.push_back(floor);
    Plane ceiling = Plane(glm::vec3(0.0, 10.0, 0.0), glm::vec3(0.0, -1.0, 0.0), Material(Material::DIFFUSE, glm::vec3(1.0), 0.0, 0.0, 0.0));
    scene.planes.push_back(ceiling);
    Plane back = Plane(glm::vec3(10.0, 1.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), Material(Material::DIFFUSE, glm::vec3(1.0), 0.0, 0.0, 0.0));
    scene.planes.push_back(back);
    Plane left = Plane(glm::vec3(0.0, 1.0, -5.0), glm::vec3(0.0, 0.0, 1.0), Material(Material::DIFFUSE, glm::vec3(1.0, 0.0, 0.0), 0.0, 0.0, 0.0));
    scene.planes.push_back(left);
    Plane right = Plane(glm::vec3(0.0, 1.0, 5.0), glm::vec3(0.0, 0.0, -1.0), Material(Material::DIFFUSE, glm::vec3(0.0, 1.0, 0.0), 0.0, 0.0, 0.0));
    scene.planes.push_back(right);
    Plane front = Plane(glm::vec3(-20.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), Material(Material::DIFFUSE, glm::vec3(0.0), 0.0, 0.0, 0.0));
    scene.planes.push_back(front);

    Sphere metal = Sphere(glm::vec3(3.0, 2.0, 2.0), 2.0, Material(Material::METAL, glm::vec3(1.0), 0.1, 0.0, 0.0));
    scene.spheres.push_back(metal);
    Sphere diffuse = Sphere(glm::vec3(2.0, 1.0, -2.0), 1.0, Material(Material::DIFFUSE, glm::vec3(1.0, 1.0, 0.0), 0.0, 0.0, 0.0));
    scene.spheres.push_back(diffuse);
    Box box = Box(glm::vec3(6.0, 3.0, -2.0), glm::vec3(3.0, 7.0, 3.0), Material(Material::DIFFUSE, glm::vec3(0.0, 0.0, 1.0), 0.0, 0.0, 0.0));
    scene.boxes.push_back(box);

    return scene;
}

void Scene::buildTriangles() {
    triangles.clear();

    for (const Model& model : models) {
        for (const Mesh& mesh : model.meshes) {
            for (size_t i = 0; i < mesh.indices.size(); i += 3) {

                uint32_t i0 = mesh.indices[i + 0];
                uint32_t i1 = mesh.indices[i + 1];
                uint32_t i2 = mesh.indices[i + 2];

                Triangle triangle;

                glm::vec3 p0 = mesh.vertices[i0].position;
                glm::vec3 p1 = mesh.vertices[i1].position;
                glm::vec3 p2 = mesh.vertices[i2].position;

                glm::vec3 n0 = mesh.vertices[i0].normal;
                glm::vec3 n1 = mesh.vertices[i1].normal;
                glm::vec3 n2 = mesh.vertices[i2].normal;

                triangle.v0 = glm::vec3(model.transform * glm::vec4(p0, 1.0f));
                triangle.v1 = glm::vec3(model.transform * glm::vec4(p1, 1.0f));
                triangle.v2 = glm::vec3(model.transform * glm::vec4(p2, 1.0f));

                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model.transform)));

                triangle.n0 = glm::normalize(normalMatrix * n0);
                triangle.n1 = glm::normalize(normalMatrix * n1);
                triangle.n2 = glm::normalize(normalMatrix * n2);

                triangles.push_back(triangle);
            }
        }
    }
}