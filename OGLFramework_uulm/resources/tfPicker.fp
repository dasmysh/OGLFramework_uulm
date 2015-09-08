#version 330

in vec3 pointColor;
out vec4 outputColor;

void main()
{
    outputColor = vec4(pointColor, 1.0f);
}
