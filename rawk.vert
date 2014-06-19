#version 150

uniform vec2  offset;
uniform float scale;

in  vec2 vPosition;
in  vec2 vTexCoord;
out vec2 fTexCoord;

void main()
{
	fTexCoord   = vTexCoord;
	gl_Position = vec4((vPosition + offset) * scale, 0.0, 1.0);
}
