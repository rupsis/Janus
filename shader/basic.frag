//FRAGMENT SHADER
#version 460 core

// in variable name MUST match out name in 
// vertex shader, else shader compilation will fail.
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D Tex;

void main() {
    FragColor = texture(Tex, texCoord);
}

