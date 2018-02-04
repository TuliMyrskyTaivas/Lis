// Sphere shader
// (c) Jarmo Pietiläinen 2012
// http://z0b.kapsi.fi
// Public domain

#version 330

uniform sampler2D tex;          // sphere texture
uniform vec4 color;             // color (usually (1, 1, 1, 1))
in vec2 texCoord;

uniform mat4 rotation;          // sphere rotation matrix
uniform vec4 lightPos;          // where the light is (assume sphere is at (0, 0, 0))
uniform float minLight;         // minimum light level

out vec4 colorOut;              // pixel output

#define PIP2    1.5707963       // PI/2
#define PI      3.1415629
#define TWOPI   6.2831853       // 2PI

/*
Define WANT_AA if you want anti-aliasing. Anti-aliasing requires blending
to be enabled first:

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
*/

#define WANT_AA

/*
Anti-aliasing "distance", ie. how thick the edge is. The bigger the
final sphere is, the lower this should be. If this is too big, the
output will be too crisp. Too low and you get too blurry edges.
Adjust accordingly.
*/

#define AA_MIN 0.985

void main()
{
    // make us round
    float d = texCoord.x * texCoord.x + texCoord.y * texCoord.y;

    if (d > 1.0)
        discard;

    // we're in, compute the exact Z
    float z = sqrt(1.0 - d);

    // get light intensity
    vec4 point = vec4(texCoord.xy, z, 1.0);

    float l = clamp(dot(point, lightPos), minLight, 1.0);

    // rotate
    point *= rotation;

    // get texture coordinates (I believe this could be replaced with a
    // precomputed texture lookup, if you need more performance)
    float x = (atan(point.x, point.z) + PI) / TWOPI,
          y = (asin(point.y) + PIP2) / PI;

    // get texel, shade, colorize and output it
    vec4 texel = texture2D(tex, vec2(x, y)) * vec4(l, l, l, 1.0) * color;
#ifndef WANT_AA
    colorOut = texel;
#else
    colorOut = texel * (1.0 - smoothstep(AA_MIN, 1.0, d));
#endif
}
