#version 150

uniform vec2  offset;
uniform float scale;

in  vec2 vPosition;
in  vec2 vTexCoord;
out vec2 fTexCoord;

void main()
{
	vec2 pos = (vPosition + offset) * scale;
	fTexCoord = vTexCoord;
	gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);
}
