#version 410

uniform vec4 color;
out vec4 outColor;

void main() {
  float i = gl_FragCoord.z - 0.1f;

  outColor = color-i;
}