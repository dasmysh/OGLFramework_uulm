#version 330

smooth in vec2 vTexCoord;

out vec4 diffuseColor;

uniform sampler2D guiTex;

void main() {
  vec4 texVal = vec4(texture(guiTex, vTexCoord).xyz, 1.0f);

  diffuseColor = texVal;
}
