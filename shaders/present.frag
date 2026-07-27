#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uAccum;
uniform int uSampleCount;

void main() {
    vec3 radiance = texture(uAccum, vUV).rgb;

    // Average all accumulated samples for this pixel.
    radiance /= max(float(uSampleCount), 1.0);

    // Reinhard tonemap (moved here from fragment.frag, since it must run
    // AFTER averaging, not per-sample). Same operator as before, just
    // relocated so it operates on the converged result.
    vec3 color = radiance / (radiance + vec3(1.0));

    FragColor = vec4(color, 1.0);
}
