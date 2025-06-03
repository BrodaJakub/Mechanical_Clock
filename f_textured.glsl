#version 330 core

in vec2 i_tc;
out vec4 pixelColor;

uniform sampler2D tex;

void main(void) {
    pixelColor = texture(tex, i_tc);
}