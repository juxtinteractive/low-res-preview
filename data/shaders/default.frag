#version 150

in vec2 texcoordpass;

out vec4 outputColor;

uniform vec2 windowSize;
uniform vec2 texSize;
uniform float texScale;
uniform float circleOffset;
uniform float circleEdge;
uniform int mode;

uniform sampler2DRect tex0;


const float Epsilon = 0.2;
const vec4 black = vec4(0.0,0.0,0.0,1.0);

void main()
{
    if(mode == 0){
        vec2 pos = (vec2(texcoordpass.x, 1.0-texcoordpass.y) * windowSize)/texScale;
        vec2 unit = fract(pos) * 2.0 - 1.0;
        float dist = sqrt(dot(unit, unit));
        vec4 texColor = texture(tex0, floor(pos));;

        outputColor = mix(black,texColor,smoothstep(circleOffset + circleEdge, circleOffset - circleEdge, dist));
    }else{
        vec2 pos = (vec2(texcoordpass.x, 1.0-texcoordpass.y) * windowSize)/texScale;
        vec2 unit = abs(fract(pos) * 2.0 - 1.0);
        unit -= circleOffset;
        unit = max(unit,0.0);
        
        outputColor = mix( black, texture(tex0, floor(pos)), 1.0 - ceil(max(unit.x,unit.y)));
    }

}