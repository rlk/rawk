#version 150

uniform sampler2D Image;

in  vec2 fTexCoord;
out vec4 fColor;

float peak(float k, float c)
{
    return max(0.0, 1.0 - abs(k - c) * 5.0);
}

vec3 colormap(float k)
{
    return peak(k, 0.0) * vec3(1.0, 0.0, 1.0) +
           peak(k, 0.2) * vec3(0.0, 0.0, 1.0) +
           peak(k, 0.4) * vec3(0.0, 1.0, 1.0) +
           peak(k, 0.6) * vec3(1.0, 1.0, 0.0) +
           peak(k, 0.8) * vec3(1.0, 0.0, 0.0) +
           peak(k, 1.0) * vec3(1.0, 0.0, 1.0);
}

void main()
{
	vec2 bound = step(vec2(0.), fTexCoord) * step(fTexCoord, vec2(1.));
	vec4 color = texture(Image, fTexCoord) * bound.x * bound.y;

	fColor = vec4(colormap(fract(color.r * 4.0)), 1.0);
}
