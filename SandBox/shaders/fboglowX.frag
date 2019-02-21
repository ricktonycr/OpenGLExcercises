#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;
uniform vec2 TexelOffset;

vec4 blur(vec4 c){
	vec4 sum = vec4(0.0);
	
	for(int i=-5;i<=5;i++){
			sum += texture(renderedTexture, vec2(UV.x+i*TexelOffset.x,UV.y)).rgba;
		
	}

	return sum/11.0;

}

void main()
{

    color = blur(texture(renderedTexture, UV.st).rgba);

}