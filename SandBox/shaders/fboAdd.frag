#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;
uniform sampler2D emissiveRenderedTexture;
uniform sampler2D bloomRenderedTexture;
uniform vec2 TexelOffset;

void main()
{
    color = texture(renderedTexture, UV.st).rgba +texture(emissiveRenderedTexture, UV.st).rgba+texture(bloomRenderedTexture, UV.st).rgba;
	//color = texture(renderedTexture, UV.st).rgba;
	//color = texture(bloomRenderedTexture, UV.st).rgba;
	//color = texture(emissiveRenderedTexture, UV.st).rgba;
}