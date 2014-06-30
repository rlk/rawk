#version 150

uniform sampler2D Image;
uniform float     zoom;

in  vec2 fTexCoord;
out vec4 fColor;

void main()
{
	vec2 bound = step(vec2(0.), fTexCoord) * step(fTexCoord, vec2(1.));

	float c1 = textureOffset(Image, fTexCoord, ivec2(+1, +1)).r;
	float c2 = textureOffset(Image, fTexCoord, ivec2( 0, +1)).r;
	float c3 = textureOffset(Image, fTexCoord, ivec2(-1, +1)).r;
	float c4 = textureOffset(Image, fTexCoord, ivec2(+1,  0)).r;
	float c5 = textureOffset(Image, fTexCoord, ivec2( 0,  0)).r;
	float c6 = textureOffset(Image, fTexCoord, ivec2(-1,  0)).r;
	float c7 = textureOffset(Image, fTexCoord, ivec2(+1, -1)).r;
	float c8 = textureOffset(Image, fTexCoord, ivec2( 0, -1)).r;
	float c9 = textureOffset(Image, fTexCoord, ivec2(-1, -1)).r;

	vec2 L = normalize(vec2(1.0, 1.0));

    vec2 G = 8.0 * vec2(c3 - c1 + 2.0 * (c6 - c4) + c9 - c7,
                        c1 - c7 + 2.0 * (c2 - c8) + c3 - c9) / zoom;

	fColor = vec4(vec3(0.5 + dot(G, L)), 1.0);
}
