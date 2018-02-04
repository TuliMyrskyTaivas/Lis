#version 430

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
uniform highp mat4 matrix;
out vec2 texc;

void main()
{
    gl_Position = matrix * vec4(vertexPosition, 1.0);
    texc = texCoord;
}
