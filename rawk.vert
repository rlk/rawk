#version 150

uniform vec2  offset;
uniform float scale;

in  vec4 vPosition;
in  vec2 vTexCoord;
out vec2 fTexCoord;

void main()
{
	fTexCoord = vTexCoord;
//	gl_Position =  vec4((vPosition.xy * scale + offset), 0.0, 1.0);
	gl_Position =  vec4((vPosition.xy + offset) * scale, 0.0, 1.0);
}
