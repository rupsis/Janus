//VERTEX SHADER
// A vertex shader uses the uploaded vertex data as input and transforms 
// the incoming primitive types from 3D to 2D screen space. 
// It then passes the generated data into the rest of the pipeline (i.e fragment shader)
#version 460 core

// Vertex Buffers. Must match definition in c++ code
// this is x,y,z location
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
// vertex arrays 
// U,V locations
layout (location = 2) in vec2 aTexCoord;

// output parameter; Passed to next part of the GPU pipleline.
layout (location = 0) out vec4 texColor; 
layout (location = 1) out vec2 texCoord; 

void main() {
    gl_Position = vec4(aPos, 1.0);
    texColor = vec4(aColor, 1.0);
    texCoord = aTexCoord;
}