//FRAGMENT SHADER
#version 460 core

layout (location = 0) in vec4 texColor;
// in variable name MUST match out name in 
// vertex shader, else shader compilation will fail.
layout (location = 1)in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D Tex;

void main() {
    FragColor = texture(Tex, texCoord) * texColor;
}

