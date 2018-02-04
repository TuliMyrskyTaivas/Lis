// Sphere shader
// (c) Jarmo Pietiläinen 2012
// http://z0b.kapsi.fi
// Public domain

#version 330

uniform mat4 mvp;       // modelviewproject matrix (most likely ortho)
in vec4 vertex;         // XYST
out vec2 texCoord;

void main()
{
    gl_Position = mvp * vec4(vertex.xy, 0.0, 1.0);
    texCoord = vertex.zw;
}
