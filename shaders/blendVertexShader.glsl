#version 130

in vec3 pos;
in vec4 color;

out vec4 vcolor;

uniform mat4 osg_ModelViewProjectionMatrix;

void main()
{
    gl_Position = osg_ModelViewProjectionMatrix * vec4(pos, 1);
    vcolor = color;
}
