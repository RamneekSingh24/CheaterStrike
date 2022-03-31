#version 330

in vec4 vCol;
in vec2 TexCoord;

out vec4 colour;

uniform sampler2D theTexture;

void main()
{
	vec4 tc = texture(theTexture, TexCoord);
	if (tc.a < 0.5)
		discard;
	colour = tc;
}