#version 330 core
out vec4 FragColor;

uniform vec2 uResolution;
uniform vec3 uSpherePosition;

void main() {
    vec2 uv = gl_FragCoord.xy / uResolution;

    uv = uv * 2.0 - 1.0;
    uv.x *= uResolution.x / uResolution.y;

    vec3 rayOrigin = vec3(0.0, 0.0, 2.0);
    vec3 rayDir = normalize(vec3(uv.x, uv.y, -1.0));

    float sphereRadius = 1.0;

    vec3 oc = rayOrigin - uSpherePosition;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(oc, rayDir);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4.0 * a * c;

    if(discriminant < 0.0) { //background
        FragColor = vec4(0.05, 0.05, 0.05, 1.0);
        return;
    }

    float t = (-b - sqrt(discriminant)) / (2.0 * a);
    vec3 hitPoint = rayOrigin + t * rayDir;

    vec3 normal = normalize(hitPoint - uSpherePosition);

    vec3 color = normal * 0.5 + 0.5;
    FragColor = vec4(color, 1.0);
}