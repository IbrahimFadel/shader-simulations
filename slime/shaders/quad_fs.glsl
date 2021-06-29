#version 330 core

uniform vec2 window_size;
uniform sampler2D compute_texture;

out vec3 color;

void main() {

    // Divide by window size to map to uv coords betwen 0 and 1
    vec3 tex_color = texture (compute_texture, gl_FragCoord.xy  / window_size).rgb;

    color = tex_color;
}
