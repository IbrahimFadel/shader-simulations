#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;
layout(rgba32f) uniform image2D texture;

uniform vec2 textureSize;

float diffusedSpeed = 0.5;
float evaporateSpeed = 0.004;

void main (void) {
    vec2 coords = gl_GlobalInvocationID.xy;

    if(coords.x < 0 || coords.x >= textureSize.x || coords.y < 0 || coords.y >= textureSize.y) {
        return;
    }

    vec4 originalValue = imageLoad(texture, ivec2(coords)).rgba;

    vec4 sum = vec4(0);
    for(int offsetX = -1; offsetX <= 1; offsetX++) {
        for(int offsetY = -1; offsetY <= 1; offsetY++) {
            int sampleX = int(coords.x) + offsetX;
            int sampleY = int(coords.y) + offsetY;

            if(sampleX >= 0 && sampleX < textureSize.x && sampleY >= 0 && sampleY < textureSize.y) {
                sum += imageLoad(texture, ivec2(sampleX, sampleY)).rgba;
            }
        }
    }

    vec4 blurResult = sum / 9;
    vec4 diffusedValue = mix(originalValue, blurResult, diffusedSpeed);
    vec4 evaporatedValue = max(vec4(0), diffusedValue - evaporateSpeed);
    // vec4 evaporatedValue = max(vec4(0), originalValue - vec4(0, evaporateSpeed, evaporateSpeed, 1));

    // vec4 pixel_color = vec4(1, 0, 0, 1);

    imageStore(texture, ivec2(coords), evaporatedValue);
}
