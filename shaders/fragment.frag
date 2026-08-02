#version 330 core
out vec4 FragColor;

const int MAX_SPHERES = 10;

struct Sphere {
    vec4 positionRadius;
    vec4 albedoEmission;
};

layout(std140) uniform SphereBuffer
{
    Sphere spheres[MAX_SPHERES];
};

uniform int uSphereCount;

uniform vec2 uResolution;
uniform int uFrame;

uniform vec3 uCameraPosition;
uniform vec3 uCameraForward;
uniform vec3 uCameraRight;
uniform vec3 uCameraUp;
uniform float uCameraFov;

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
    vec3 point;
    vec3 normal;
};

struct HitInfo
{
    float t;

    vec3 position;
    vec3 normal;

    vec3 albedo;
    float emission;

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

    float t = (-b - sqrt(delta)) / (2.0 * a);

    if(t < 0.001)
    return false;

    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + ray.direction * t;
    hit.normal = normalize(hit.position - sphere.positionRadius.xyz);
    hit.albedo = sphere.albedoEmission.xyz;
    hit.emission = sphere.albedoEmission.w;
    hit.emissive = sphere.albedoEmission.w > 0.0;

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
    hit.emission = 0.0;
    hit.emissive = false;

    return true;
}

bool traceScene(Ray ray, Plane plane, out HitInfo hit) {
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

    if(intersectPlane(ray, plane, temp)) {
        if(temp.t < hit.t) {
            hit = temp;
            hit.emission = 0.0;
        }
    }

    return hit.hit;
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

    Plane plane;
    plane.point = vec3(0.0, -1.0, 0.0);
    plane.normal = vec3(0.0, 1.0, 0.0);

    Ray ray;
    ray.origin = uCameraPosition;
    ray.direction = rayDir;

    vec3 throughput = vec3(1.0);

    vec3 radiance = vec3(0.0);


    for (int bounce = 0; bounce < 10; bounce++)
    {
        HitInfo hit;

        if (!traceScene(ray, plane, hit)) {
            break;
        }

        if (hit.emission > 0.0) {
            radiance += throughput * hit.albedo * hit.emission;
            break;
        }

        for (int i = 0; i < uSphereCount; i++)
        {
            Sphere light =
            spheres[i];

            float lightEmission =
            light.albedoEmission.w;

            if (lightEmission <= 0.0) {
                continue;
            }

            vec3 lightPoint = sampleSphere(light, rng);

            vec3 toLight = lightPoint - hit.position;

            float lightDistance = length(toLight);

            vec3 lightDirection = toLight / lightDistance;

            float NdotL = max(dot(hit.normal, lightDirection), 0.0);

            if (NdotL <= 0.0) {
                continue;
            }

            Ray visibilityRay;

            visibilityRay.origin = hit.position + hit.normal * 0.001;

            visibilityRay.direction = lightDirection;

            HitInfo lightHit;

            if (traceScene(visibilityRay, plane, lightHit)) {
                if (lightHit.emission > 0.0) {
                    vec3 lightRadiance = lightHit.albedo * lightHit.emission;
                    radiance += throughput * hit.albedo * lightRadiance * NdotL;
                }
            }
        }

        throughput *= hit.albedo;

        ray.origin = hit.position + hit.normal * 0.001;

        ray.direction = cosineSampleHemisphere(hit.normal, rng);
    }

    FragColor = vec4(radiance, 1.0);
}