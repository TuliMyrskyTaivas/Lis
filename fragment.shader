#version 430

uniform sampler2D texture;
varying vec2 texc;

void main()
{
    gl_FragColor = texture2D(texture, texc);
}
