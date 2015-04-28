#version 330

smooth in vec2 vTexCoord;
flat in float page;

// layout(location = 0) out vec4 diffuseColor;
out vec4 diffuseColor;

uniform vec4 fontStyle;
uniform vec4 color = vec4(1, 1, 1, 1);
uniform sampler2DArray fontTex;

void main() {
  vec4 texVal = texture(fontTex, vec3(vTexCoord, page));
  
  float fontVal = 1.55;
  float msVal = 1.49;
  float msMult = (fontVal - msVal);
  float pixel = ((texVal.x + texVal.y + texVal.z) / 3) + fontStyle.x + 0.05;
  if (pixel > fontVal) {
    diffuseColor = color;
  } else if (pixel > msVal) {
    float msalpha = clamp((pixel - msVal)/msMult, 0, 1);
    diffuseColor = vec4(color.xyz, msalpha);
  } else {
    diffuseColor = vec4(0, 0, 0, 0);
  }
}
