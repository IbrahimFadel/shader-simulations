#version 330 core

uniform vec2 windowSize;
uniform sampler2D computeTexture;

out vec3 color;

void main() {

    // Divide by window size to map to uv coords betwen 0 and 1
    vec3 tex_color = texture (computeTexture, gl_FragCoord.xy  / windowSize).rgb;

    color = tex_color;
}
