#version 150

uniform sampler2D Image;

in  vec2 fTexCoord;
out vec4 fColor;

void main()
{
	vec2 bound = step(vec2(0.), fTexCoord) * step(fTexCoord, vec2(1.));
	vec4 color = texture(Image, fTexCoord) * bound.x * bound.y;

	fColor = vec4(vec3(color.r), 1.0);
}
