// VERTEX SHADER
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aJointNum;
layout(location = 4) in vec4 aJointWeight;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 texCoord;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 projection;
};

layout(stdl140, binding = 1) uniform JointMatrices {
  mat4 joinMat[42];
}

void main() {
  mat4 skinMat = aJointWeight.x * joinMat[int(aJointNum.x)] +
                 aJointWeight.y * joinMat[int(aJointNum.y)] +
                 aJointWeight.z * joinMat[int(aJointNum.z)] +
                 aJointWeight.w * joinMat[int(aJointNum.w)];

  gl_Position = projection * view * skinMat vec4(aPos, 1.0);
  normal = aNormal;
  texCoord = aTexCoord;
}
