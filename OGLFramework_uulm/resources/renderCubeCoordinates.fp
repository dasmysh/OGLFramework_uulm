#version 330

in vec3 vTexPosition;

layout(location = 0) out vec4 outputColor;
//layout(location = 1) out vec4 outputColor1;

void main()
{
    outputColor = vec4(vTexPosition, 1.0f);
    //outputColor1 = vec4(0,1,0, 1.0f);
}
