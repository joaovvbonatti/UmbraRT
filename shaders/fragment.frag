#version 330 core
out vec4 FragColor;

uniform vec2 uResolution;
uniform int uFrame;

uniform vec3 uSpherePosition;

uniform vec3 uCameraPosition;
uniform vec3 uCameraForward;
uniform vec3 uCameraRight;
uniform vec3 uCameraUp;
uniform float uCameraFov;

const int MATERIAL_DIFFUSE = 0;
const int MATERIAL_EMISSIVE = 1;

uint pcg_hash(uint num)
{
    uint state = num * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float random(inout uint state)
{
    state = pcg_hash(state);
    return float(state) / 4294967295.0;
}

const float PI = 3.14159265359;

vec3 cosineSampleHemisphere(vec3 normal, inout uint rng)
{
    float u1 = random(rng);
    float u2 = random(rng);

    float r = sqrt(u1);
    float theta = 2.0 * PI * u2;

    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(1.0 - u1);

    vec3 tangent;

    if(abs(normal.y) < 0.999)
    tangent = normalize(cross(vec3(0.0,1.0,0.0), normal));
    else
    tangent = normalize(cross(vec3(1.0,0.0,0.0), normal));

    vec3 bitangent = cross(normal, tangent);

    return normalize(
        tangent * x
        + bitangent * y
        + normal * z
    );
}

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
    int material;

    vec3 albedo;
    vec3 emission;
};

struct Plane{
    vec3 point;
    vec3 normal;
};

struct HitInfo
{
    float t;

    vec3 position;
    vec3 normal;

    vec3 albedo;
    vec3 emission;

    int material;

    bool hit;
};


vec3 sampleSphere(Sphere light, inout uint rng)
{
    float z = random(rng) * 2.0 - 1.0;
    float phi = random(rng) * 2.0 * PI;

    float r = sqrt(1.0 - z*z);

    vec3 dir = vec3(r*cos(phi), r*sin(phi), z);

    return light.center + dir * light.radius;
}


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
    hit.albedo = sphere.albedo;
    hit.material = sphere.material;
    hit.emission = sphere.emission;

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

    hit.albedo = mix(vec3(0.20), vec3(0.15), checker);

    return true;
}

bool traceScene(Ray ray, Sphere sphere, Sphere light, Plane plane, out HitInfo hit) {
    hit.hit = false;
    hit.t = 1e30;

    HitInfo temp;

    if(intersectSphere(ray, sphere, temp)) {
        if(temp.t < hit.t)
                hit = temp;
    }

    if(intersectPlane(ray, plane, temp)) {
        if(temp.t < hit.t) {
            hit = temp;
            hit.material = MATERIAL_DIFFUSE;
            hit.emission = vec3(0.0);
        }
    }

    if(intersectSphere(ray, light, temp))
    {
        if(temp.t < hit.t)
            hit = temp;
    }

    return hit.hit;
}

vec3 skyColor = vec3(0.0);

void main() {
    uint rng = uint(gl_FragCoord.x) + uint(gl_FragCoord.y)*4096u + uint(uFrame)*16777619u;

    vec2 uv = gl_FragCoord.xy / uResolution;

    uv = uv * 2.0 - 1.0;
    uv.x *= uResolution.x / uResolution.y;


    float scale = tan(radians(uCameraFov * 0.5));

    vec3 rayDir =
    normalize(uCameraForward + uv.x * scale * uCameraRight + uv.y * scale * uCameraUp);

    Sphere sphere;
    sphere.center = uSpherePosition;
    sphere.radius = 1.0;
    sphere.albedo = vec3(0.8, 0.2, 0.2);
    sphere.material = MATERIAL_DIFFUSE;
    sphere.emission = vec3(0.0);

    Sphere light;
    light.center = vec3(0.0, 5, 0.0);
    light.radius = 0.5;
    light.albedo = vec3(1.0);
    light.material = MATERIAL_EMISSIVE;
    light.emission = vec3(1.0);

    Plane plane;
    plane.point = vec3(0.0, -1.0, 0.0);
    plane.normal = vec3(0.0, 1.0, 0.0);

    HitInfo hit;

    Ray ray;
    ray.origin = uCameraPosition;
    ray.direction = rayDir;

    vec3 color;

    vec3 throughput = vec3(1.0);
    vec3 radiance = vec3(0.0);

    for(int bounce = 0; bounce < 100; bounce++) {
        if(!traceScene(ray, sphere, light, plane, hit)) {
            radiance += throughput * skyColor;
            break;
        }

        ray.origin = hit.position + hit.normal * 0.001;

        if(hit.material == MATERIAL_EMISSIVE)
        {
            radiance += throughput * hit.emission;
            break;
        }

        vec3 lightPoint = sampleSphere(light, rng);
        vec3 toLight = lightPoint - hit.position;
        float lightDistance = length(toLight);
        vec3 lightDir = toLight / lightDistance;
        Ray shadowRay;
        shadowRay.origin =
        hit.position + hit.normal * 0.001;
        shadowRay.direction = lightDir;
        HitInfo shadowHit;

        if(traceScene(shadowRay, sphere, light, plane, shadowHit)) {
            if(shadowHit.material == MATERIAL_EMISSIVE && shadowHit.t < lightDistance + 0.001) {
                float NdotL = max(dot(hit.normal, lightDir),0.0);

                radiance += throughput * hit.albedo * light.emission * NdotL;
            }
        }


        throughput *= hit.albedo;

        ray.direction = cosineSampleHemisphere(hit.normal, rng);
    }

    color = radiance;

    FragColor = vec4(color, 1.0);
}