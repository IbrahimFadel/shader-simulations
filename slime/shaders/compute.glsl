#version 430 core

layout(local_size_x = 16, local_size_y = 1) in;
layout(rgba32f) uniform image2D texture;

uniform vec2 textureSize;

struct Agent {
        float positionX;
        float positionY;
        float angle;
};

layout(std430, binding = 0) buffer SSBO
{
   Agent agents[ ];
};

float PI = 3.14159265358979323846;
float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   
float gold_noise(vec2 xy, float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

// float speed = 3;
// float turnSpeed = 0.5;
// float sensorOffsetDst = 10;
// int sensorSize = 5;
// float sensorAngleSpacing = 20;

// Settings A

// float speed = 20;
// float turnSpeed = 2;
// float sensorOffsetDst = 35;
// int sensorSize = 1;
// float sensorAngleSpacing = 30;

// Settings C

float speed = 1;
float turnSpeed = 3;
float sensorOffsetDst = 20;
int sensorSize = 1;
float sensorAngleSpacing = 112;

float sense(Agent agent, float sensorAngleOffset) {
        vec2 agentPos = vec2(agent.positionX, agent.positionY);

        float sensorAngle = agent.angle + sensorAngleOffset;
        vec2 sensorDir = vec2(cos(sensorAngle), sin(sensorAngle));
        vec2 sensorCenter = agentPos + sensorDir * sensorOffsetDst;

        float sum = 0;

        for(int offsetX = -sensorSize; offsetX <= sensorSize; offsetX++) {
                for(int offsetY = -sensorSize; offsetY <= sensorSize; offsetY++) {
                        vec2 pos = sensorCenter + vec2(offsetX, offsetY);

                        if(pos.x >= 0 && pos.x < textureSize.x && pos.y >= 0 && pos.y < textureSize.y) {
                                sum += imageLoad(texture, ivec2(pos)).g; // When you change the pixel color, this should be updated to sample from the correct color channel
                        }
                }
        }

        return sum;
}

void main (void) {
        uint index = gl_GlobalInvocationID.x;
        Agent agent = agents[index];
        vec2 position = vec2(agent.positionX, agent.positionY);
        // vec4 pixelColor = vec4(0, 0.8, 0.5, 1);
        vec4 pixelColor = vec4(1);

        float hashedNum = gold_noise(position, 50);

        float weightForward = sense(agent, 0);
        float weightLeft = sense(agent, sensorAngleSpacing);
        float weightRight = sense(agent, -sensorAngleSpacing);

        float randomSteerStrength = hashedNum;

        if(weightForward > weightLeft && weightForward > weightRight) {
                // pixelColor = vec4(1, 0, 0, 1);
                agents[index].angle += 0;
        }
        else if(weightForward < weightLeft && weightForward < weightRight) {
                agents[index].angle += (randomSteerStrength - 0.5) * 2 * turnSpeed;
        }
        else if(weightRight > weightLeft) {
                // pixelColor = vec4(0, 1, 0, 1);
                agents[index].angle -= randomSteerStrength * turnSpeed;
        }
        else if(weightLeft > weightRight) {
                // pixelColor = vec4(0, 0, 1, 1);
                agents[index].angle += randomSteerStrength * turnSpeed;
        }

        vec2 direction = vec2(cos(agent.angle), sin(agent.angle));
        vec2 newPos = position + direction * speed;

        if(newPos.x < 0 || newPos.x >= textureSize.x || newPos.y < 0 || newPos.y >= textureSize.y) {
                newPos.x = min(textureSize.x - 0.01, max(0, newPos.x));
                newPos.y = min(textureSize.y - 0.01, max(0, newPos.y));

                agents[index].angle = hashedNum * 2 * PI;
        }
        
        agents[index].positionX = newPos.x;
        agents[index].positionY = newPos.y;
        imageStore(texture, ivec2(newPos), pixelColor);
}
