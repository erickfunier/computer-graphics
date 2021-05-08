#version 410 core

layout(location = 0) in vec3 inPosition;

uniform float angle;
uniform float posY;
uniform float posX;
uniform float posZ;


void main() {
  float sinAngle = sin(angle);
  float cosAngle = cos(angle);

  gl_Position =
      vec4((inPosition.x + posX) * cosAngle + (inPosition.z + posZ) * sinAngle, 
           inPosition.y + posY,
           (inPosition.z + posZ) * cosAngle - (inPosition.x + posX) * sinAngle, 1.0);
}