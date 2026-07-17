#version 330 core
out vec4 FragColor;

uniform vec2 uResolution;

uniform vec3 uSpherePosition;

uniform vec3 uCameraPosition;
uniform vec3 uCameraForward;
uniform vec3 uCameraRight;
uniform vec3 uCameraUp;
uniform float uCameraFov;

struct Camera{
    vec3 position;
    vec3 forward;
    vec3 up;
    vec3 right;
    float fov;
};

struct Ray{
    vec3 origin;
    vec3 direction;
};

struct Sphere{
    vec3 center;
    float radius;
    vec3 color;
};

struct Plane{
    vec3 point;
    vec3 normal;
};

struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
    float emission;
};

struct HitInfo{
    float t;
    vec3 position;
    vec3 normal;
    bool hit;
    Material material;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

bool intersectSphere(Ray ray, Sphere sphere, out HitInfo hit) {
    vec3 oc = ray.origin - sphere.center;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;

    float delta = b*b - 4.0*a*c;

    if(delta < 0.0)
    return false;

    float t = (-b - sqrt(delta)) / (2.0 * a);

    if(t < 0.001)
    return false;

    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + ray.direction * t;
    hit.normal = normalize(hit.position - sphere.center);
    hit.material.albedo = sphere.color;

    return true;
}

bool intersectPlane(Ray ray, Plane plane, out HitInfo hit) {
    float denom = dot(ray.direction, plane.normal);

    if(abs(denom) < 0.0001)
            return false;

    float t = dot(plane.point - ray.origin, plane.normal) / denom;

    if(t < 0.001)
            return false;

    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + ray.direction * t;
    hit.normal = normalize(plane.normal);

    float tileSize = 1.0;

    float checker = mod(floor(hit.position.x / tileSize) + floor(hit.position.z / tileSize), 2.0);

    hit.material.albedo = mix(vec3(0.20), vec3(0.15), checker);

    return true;
}

bool traceScene(Ray ray, Sphere sphere, Plane plane, out HitInfo hit) {
    hit.hit = false;
    hit.t = 1e30;

    HitInfo temp;

    if(intersectSphere(ray, sphere, temp)) {
        if(temp.t < hit.t)
                hit = temp;
    }

    if(intersectPlane(ray, plane, temp)) {
        if(temp.t < hit.t)
                hit = temp;
    }

    return hit.hit;
}

vec3 computeLighting(HitInfo hit, PointLight light) {
    vec3 L = light.position - hit.position;
    float distance = length(L);
    L = normalize(L);
    float NdotL = max(dot(hit.normal, L), 0.0);
    float attenuation = 1.0 / (distance * distance);

    return hit.material.albedo * light.color * light.intensity * attenuation * NdotL;
}

void main() {
    vec2 uv = gl_FragCoord.xy / uResolution;

    uv = uv * 2.0 - 1.0;
    uv.x *= uResolution.x / uResolution.y;


    float scale = tan(radians(uCameraFov * 0.5));

    vec3 rayDir =
    normalize(uCameraForward + uv.x * scale * uCameraRight + uv.y * scale * uCameraUp);

    Sphere sphere;
    sphere.center = uSpherePosition;
    sphere.radius = 1.0;
    sphere.color = vec3(0.8, 0.2, 0.2);

    Plane plane;
    plane.point = vec3(0.0, -1.0, 0.0);
    plane.normal = vec3(0.0, 1.0, 0.0);

    PointLight light;
    light.position = vec3(0.0, 5.0, 0.0);
    light.color = vec3(0.0, 1.0, 1.0);
    light.intensity = 50.0;

    HitInfo hit;

    Ray ray;
    ray.origin = uCameraPosition;
    ray.direction = rayDir;

    vec3 color;

    if(traceScene(ray, sphere, plane, hit)) {
        vec3 ambient = vec3(0.1) * hit.material.albedo;
        color = ambient;

        color += computeLighting(hit, light);

        Ray shadowRay;
        shadowRay.origin = hit.position + hit.normal * 0.001;
        shadowRay.direction = normalize(light.position - hit.position);

        HitInfo shadowHit;

        if (traceScene(shadowRay, sphere, plane, shadowHit)) {
            float lightDistance =
            length(light.position - hit.position);

            if (shadowHit.t < lightDistance)
                color = ambient;

        }
    }

    FragColor = vec4(color, 1.0);
}