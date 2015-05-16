#version 150

in vec2 texcoordpass;

out vec4 outputColor;

uniform vec2 windowSize;
uniform vec2 texSize;
uniform float texScale;
uniform sampler2DRect tex0;

void main()
{
    outputColor = texture(tex0, round((vec2(texcoordpass.x, 1.0-texcoordpass.y) * windowSize)/texScale));
}