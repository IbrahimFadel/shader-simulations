#version 430 core

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f) uniform image2D texture;

uniform vec2 textureSize;

struct Fluid
{
    float dt;
    float diff;
    float visc;

    float s[4096];
    float density[4096];

    float Vx[4096];
    float Vy[4096];

    float Vx0[4096];
    float Vy0[4096];
};

layout(std430, binding = 0) buffer SSBO
{
   Fluid fluid;
};

float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   
float goldNoise(vec2 xy, float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

int getIndex(vec2 pos) {
	return int(pos.x + pos.y * textureSize.x);
}

void main(void) {
	vec2 pos = gl_GlobalInvocationID.xy;
	int index = getIndex(pos);

	vec4 pixelColor = vec4(pos / textureSize, 0.2, 1);

	fluid.density[index] = goldNoise(pos, 1);

	pixelColor.r = fluid.density[index];
	// pixelColor.r = goldNoise(pos, 1);

	imageStore(texture, ivec2(pos), pixelColor);
}