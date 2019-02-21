#version 450 core

in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;
uniform vec2 TexelOffset;



float gauss[11] = {0.000003,0.000229,0.005977,0.060598,0.24173,0.382925,0.24173,0.060598,0.005977,0.000229,0.000003};

vec4 blur(){
	vec4 sum = vec4(0.0);
	
	for(int i=-5;i<=5;i++){
			sum += texture(renderedTexture, vec2(UV.x+i*TexelOffset.x,UV.y)).rgba*gauss[i+5];
		
	}

	return sum;

}

void main()
{

    color =  blur();

}