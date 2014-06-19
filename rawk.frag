#version 150

uniform sampler2D Image;

in  vec2 fTexCoord;
out vec4 fColor;

void main()
{
	fColor = texture(Image, fTexCoord);
}
