//FRAGMENT SHADER
#version 460 core

layout (location = 0) in vec3 normal; 
layout (location = 1) in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D Tex;

vec3 lightPos = vec3(4.0, 5.0, -3.0);
vec3 lightColor = vec3(0.5, 0.5, 0.5);

void main() {
    float lightAngle = max(dot(normalize(normal), normalize(lightPos)), 0.0);
    FragColor = texture(Tex, texCoord) * vec4((0.3 + 0.7 * lightAngle) * lightColor, 1.0); 
}

