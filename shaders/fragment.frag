#version 330 core
out vec4 FragColor;

const float EPSILON = 0.001;

const int MAX_SPHERES = 100;
const int MAX_BOXES = 100;
const int MAX_PLANES = 10;

const int MATERIAL_DIFFUSE = 0;
const int MATERIAL_METAL = 1;
const int MATERIAL_DIELECTRIC = 2;
const int MATERIAL_EMISSIVE = 3;

struct Sphere {
    vec4 positionRadius;
    vec4 albedoEmission;
    vec4 materialProperties;
};

layout(std140) uniform SphereBuffer
{
    Sphere spheres[MAX_SPHERES];
};

uniform int uSphereCount;
uniform int uBoxCount;
uniform int uPlaneCount;

uniform vec2 uResolution;
uniform int uFrame;

uniform vec3 uCameraPosition;
uniform vec3 uCameraForward;
uniform vec3 uCameraRight;
uniform vec3 uCameraUp;
uniform float uCameraFov;

uniform vec3 uSkyColor;
uniform vec3 uSkyHorizon;
uniform float uSkyIntensity;

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

struct Ray{
    vec3 origin;
    vec3 direction;
};

struct Plane{
    vec4 point;
    vec4 normal;
    vec4 albedoEmission;
    vec4 materialProperties;
};

struct HitInfo
{
    float t;

    vec3 position;
    vec3 normal;

    vec3 albedo;
    float emission;

    vec4 materialProperties;

    bool emissive;
    bool hit;
};

vec3 sampleSphere(Sphere light, inout uint rng)
{
    float z = random(rng) * 2.0 - 1.0;
    float phi = random(rng) * 2.0 * PI;

    float r = sqrt(1.0 - z*z);

    vec3 dir = vec3(r*cos(phi), r*sin(phi), z);

    return light.positionRadius.xyz + dir * light.positionRadius.w;
}

bool intersectSphere(Ray ray, Sphere sphere, out HitInfo hit) {
    vec3 oc = ray.origin - sphere.positionRadius.xyz;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.positionRadius.w * sphere.positionRadius.w;

    float delta = b*b - 4.0*a*c;

    if(delta < 0.0)
    return false;

    float sqrtDelta = sqrt(delta);

    float t0 = (-b - sqrtDelta) / (2.0 * a);
    float t1 = (-b + sqrtDelta) / (2.0 * a);

    float t = t0;

    if (t < 0.001)
    t = t1;

    if (t < 0.001)
    return false;

    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + ray.direction * t;
    hit.normal = normalize(hit.position - sphere.positionRadius.xyz);

    hit.albedo = sphere.albedoEmission.xyz;
    hit.emission = sphere.albedoEmission.w;
    hit.materialProperties = sphere.materialProperties;
    hit.emissive = sphere.albedoEmission.w > 0.0;

    return true;
}

bool intersectPlane(Ray ray, Plane plane, out HitInfo hit) {
    float denom = dot(ray.direction, plane.normal.xyz);

    if(abs(denom) < 0.0001)
            return false;

    float t = dot(plane.point.xyz - ray.origin, plane.normal.xyz) / denom;

    if(t < 0.001)
            return false;

    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + ray.direction * t;
    hit.normal = normalize(plane.normal.xyz);

    float tileSize = 1.0;

    float checker = mod(floor(hit.position.x / tileSize) + floor(hit.position.z / tileSize), 2.0);

    hit.albedo = mix(vec3(0.20), vec3(0.15), checker);
    hit.emission = 0.0;
    hit.materialProperties = vec4(MATERIAL_DIFFUSE, 0.0, 1.5, 0.0);
    hit.emissive = false;

    return true;
}

struct Box {
    vec4 min;
    vec4 max;
    vec4 albedoEmission;
    vec4 materialProperties;
};

layout(std140) uniform PlaneBuffer
{
    Plane planes[MAX_PLANES];
};

bool intersectBox(Ray ray, Box box, out HitInfo hit) { //slab method
    vec3 invDir = 1.0 / ray.direction;

    vec3 t0 = (box.min.xyz - ray.origin) * invDir;
    vec3 t1 = (box.max.xyz - ray.origin) * invDir;

    vec3 tMin = min(t0, t1);
    vec3 tMax = max(t0, t1);

    float tNear = max(max(tMin.x, tMin.y), tMin.z);
    float tFar = min(min(tMax.x, tMax.y), tMax.z);

    if(tNear > tFar || tFar < 0.0)
    return false;

    float t = tNear > 0.0 ? tNear : tFar;

    hit.t = t;
    hit.position = ray.origin + ray.direction * t;

    vec3 p = hit.position;

    float eps = 0.0001;

    if (abs(p.x - box.min.x) < eps)
        hit.normal = vec3(-1.0, 0.0, 0.0);
    else if (abs(p.x - box.max.x) < eps)
        hit.normal = vec3(1.0, 0.0, 0.0);
    else if (abs(p.y - box.min.y) < eps)
        hit.normal = vec3(0.0, -1.0, 0.0);
    else if (abs(p.y - box.max.y) < eps)
        hit.normal = vec3(0.0, 1.0, 0.0);
    else if (abs(p.z - box.min.z) < eps)
        hit.normal = vec3(0.0, 0.0, -1.0);
    else
        hit.normal = vec3(0.0, 0.0, 1.0);

    hit.albedo = box.albedoEmission.xyz;
    hit.emission = box.albedoEmission.w;

    hit.materialProperties = box.materialProperties;

    hit.hit = true;
    hit.emissive = box.albedoEmission.w > 0.0;

    return true;
}

vec3 sampleBox(Box box, inout uint rng)
{
    vec3 size = box.max.xyz - box.min.xyz;

    float areaXY = size.x * size.y;
    float areaXZ = size.x * size.z;
    float areaYZ = size.y * size.z;

    float totalArea = 2.0 * (areaXY + areaXZ + areaYZ);

    float r = random(rng) * totalArea;

    vec2 uv = vec2(random(rng), random(rng));

    // ±X
    if (r < 2.0 * areaYZ)
    {
        bool positive = r >= areaYZ;

        return vec3(
        positive ? box.max.x : box.min.x,
        mix(box.min.y, box.max.y, uv.x),
        mix(box.min.z, box.max.z, uv.y)
        );
    }

    r -= 2.0 * areaYZ;

    // ±Y
    if (r < 2.0 * areaXZ)
    {
        bool positive = r >= areaXZ;

        return vec3(
        mix(box.min.x, box.max.x, uv.x),
        positive ? box.max.y : box.min.y,
        mix(box.min.z, box.max.z, uv.y)
        );
    }

    r -= 2.0 * areaXZ;

    // ±Z
    bool positive = r >= areaXY;

    return vec3(
    mix(box.min.x, box.max.x, uv.x),
    mix(box.min.y, box.max.y, uv.y),
    positive ? box.max.z : box.min.z
    );
}

vec3 sampleMetal(vec3 direction, vec3 normal, float roughness, inout uint rng) {
    vec3 reflected = reflect(direction, normal);

    if (roughness <= 0.0)
        return reflected;

    vec3 randomDirection = cosineSampleHemisphere(reflected, rng);

    return normalize(mix(reflected, randomDirection, roughness));
}

layout(std140) uniform BoxBuffer
{
    Box boxes[MAX_BOXES];
};

bool traceScene(Ray ray, out HitInfo hit) {
    hit.hit = false;
    hit.t = 1e30;

    HitInfo temp;

    for(int i = 0; i < uSphereCount; i++) {
        if(intersectSphere(ray, spheres[i], temp)) {
            if(temp.t < hit.t) {
                hit = temp;
            }
        }
    }

    for(int i = 0; i < uPlaneCount; i++) {
        if(intersectPlane(ray, planes[i], temp)) {
            if(temp.t < hit.t) {
                hit = temp;
            }
        }
    }

    for(int i = 0; i < uBoxCount; i++) {
        if(intersectBox(ray, boxes[i], temp)) {
            if(temp.t < hit.t) {
                hit = temp;
            }
        }
    }

    return hit.hit;
}

vec3 skyColor(vec3 direction)
{
    float height = max(direction.y, 0.0);

    float t = pow(height, 0.5);

    vec3 horizon = uSkyHorizon;
    vec3 zenith  = uSkyColor;

    vec3 color = mix(horizon, zenith, t);

    return color * uSkyIntensity;
}

void main() {
    uint rng = uint(gl_FragCoord.x) + uint(gl_FragCoord.y)*4096u + uint(uFrame)*16777619u;

    vec2 jitter = vec2(random(rng), random(rng)) - 0.5;
    vec2 pixelCoord = gl_FragCoord.xy + jitter;

    vec2 uv = pixelCoord / uResolution;
    uv = uv * 2.0 - 1.0;
    uv.x *= uResolution.x / uResolution.y;
    float scale = tan(radians(uCameraFov * 0.5));
    vec3 rayDir = normalize(uCameraForward + uv.x * scale * uCameraRight + uv.y * scale * uCameraUp);

    Ray ray;
    ray.origin = uCameraPosition;
    ray.direction = rayDir;

    vec3 throughput = vec3(1.0);

    vec3 radiance = vec3(0.0);

    HitInfo hit;


    for (int bounce = 0; bounce < 10; bounce++)
    {
        if (!traceScene(ray, hit)) {
            radiance += throughput * skyColor(ray.direction);
            break;
        }

        // =========================
        // Emissive
        // =========================

        if (hit.emission > 0.0) {
            radiance += throughput * hit.albedo * hit.emission;
            break;
        }

        int materialType = int(hit.materialProperties.x);

        if (materialType == MATERIAL_DIFFUSE)
        {
            // =========================
            // Spheres NEE
            // =========================

            for (int i = 0; i < uSphereCount; i++)
            {
                Sphere light = spheres[i];

                float lightEmission = light.albedoEmission.w;

                if (lightEmission <= 0.0)
                continue;


                vec3 lightPoint = sampleSphere(light, rng);

                vec3 toLight = lightPoint - hit.position;

                float lightDistance = length(toLight);

                vec3 lightDirection = toLight / lightDistance;

                float NdotL = max(dot(hit.normal, lightDirection), 0.0);

                if (NdotL <= 0.0)
                continue;

                Ray visibilityRay;

                visibilityRay.origin = hit.position + hit.normal * 0.001;

                visibilityRay.direction = lightDirection;

                HitInfo lightHit;

                if (traceScene(visibilityRay, lightHit)) {
                    if (lightHit.emission > 0.0) {
                        vec3 lightRadiance = lightHit.albedo * lightHit.emission;

                        radiance += throughput * hit.albedo * lightRadiance * NdotL;
                    }
                }
            }

            // =========================
            // Boxes NEE
            // =========================

            for (int i = 0; i < uBoxCount; i++) {
                Box light = boxes[i];

                float lightEmission = light.albedoEmission.w;

                if (lightEmission <= 0.0)
                continue;

                vec3 lightPoint = sampleBox(light, rng);

                vec3 toLight = lightPoint - hit.position;

                float lightDistance = length(toLight);

                vec3 lightDirection = toLight / lightDistance;

                float NdotL = max(dot(hit.normal, lightDirection), 0.0);

                if (NdotL <= 0.0)
                continue;

                Ray visibilityRay;

                visibilityRay.origin = hit.position + hit.normal * EPSILON;

                visibilityRay.direction = lightDirection;

                HitInfo lightHit;

                if (traceScene(visibilityRay, lightHit)) {
                    if (lightHit.emission > 0.0) {
                        vec3 lightRadiance = lightHit.albedo * lightHit.emission;

                        radiance += throughput * hit.albedo * lightRadiance * NdotL;
                    }
                }
            }

            throughput *= hit.albedo;

            ray.origin = hit.position + hit.normal * EPSILON;

            ray.direction = cosineSampleHemisphere(hit.normal, rng);
        }

        // =========================
        // Material
        // =========================

        else if (materialType == MATERIAL_METAL)
        {
            throughput *= hit.albedo;

            ray.origin = hit.position + hit.normal * EPSILON;

            float roughness = hit.materialProperties.y;

            ray.direction = sampleMetal(ray.direction, hit.normal, roughness, rng);
        }
    }

    FragColor = vec4(radiance, 1.0);
}