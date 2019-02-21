#version 330 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace[2];


// Output <- color
out vec4 color;

uniform sampler2D specTextureSampler;

uniform mat4 MV;
uniform int LightCount;
uniform bool IsReceiver;

struct Light {
	vec4 position ;
	vec4 color ;
};

layout(std140) uniform LightBlock{
	Light lights[2];
};




float specFactor = 5;
vec4 MaterialSpecularColor = vec4(1.0,1.0,1.0,1.0);

vec4 lit(in vec3 l,in vec3 n,in vec3 v,in vec4 lc){
	
	vec3 r = reflect(-l,n);
	float cosAlpha = clamp( dot( v,r ), 0,1 );
	
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	vec4 c = specFactor *MaterialSpecularColor * lc * pow(cosAlpha,100);
	return c;

}

void main(){

	

	
	specFactor = texture( specTextureSampler, UV ).r+0.5;

	vec3 n = normalize( Normal_cameraspace );
	vec3 v = normalize(EyeDirection_cameraspace);


	color= vec4(0.0,0.0,0.0,0.0);
	for(int i=0;i<LightCount;i++){// Distancia hacia la luz
		float distance = length( vec3(lights[i].position) - Position_worldspace );
		//frosbite / cryengine
		vec4 LightColor = lights[i].color*pow(4.0/(max(distance,0.1f)),2);

		//Unreal
		//vec3 LightColor = vec3(1.0f,1.0f,1.0f)*pow(4.0,2)/(pow(distance,2)+0.1f);

		vec3 l = normalize( LightDirection_cameraspace[i] );
		
		color = color + lit(l,n,v,LightColor);

	}




	

}