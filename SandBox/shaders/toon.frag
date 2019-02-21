#version 330 core

// datos recibidos del vertex shader despues de interpolacion
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace[2];

// Output <- color
out vec3 color;

// datos unifromes a todo el objeto
uniform sampler2D rgbTextureSampler;
uniform sampler2D specTextureSampler;
uniform mat4 MV;
uniform int LightCount;

struct Light {
	vec4 position ;
	vec4 color ;
};

layout(std140) uniform LightBlock{
	Light lights[2];
};

float specFactor = 0;
vec3 MaterialDiffuseColor = texture( rgbTextureSampler, vec2(UV.x,1-UV.y) ).rgb;//vec3(UV,0.0);;
vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
vec3 MaterialSpecularColor = vec3(1.0,1.0,1.0);

vec3 lit(in vec3 l,in vec3 n,in vec3 v,in vec3 lc){
	
	vec3 r = reflect(-l,n);
	float cosAlpha = clamp( dot( v,r ), 0,1 );
	
	float cosTheta = clamp( dot( n,l ), 0,1 );
	

	vec3 c;
	if(cosTheta>0.9){
		c=MaterialDiffuseColor;
	}else if(cosTheta>0.6){
		c=vec3(0.6,0.6,0.6)*MaterialDiffuseColor;
	}else if(cosTheta>0.3){
		c=vec3(0.3,0.3,0.3)*MaterialDiffuseColor;
	}else{
		c=MaterialAmbientColor;
	}

	if(cosAlpha>0.95){
		c=lc;		
	}
	return c;

}

void main(){

	

	if(texture( specTextureSampler, vec2(UV.x,1-UV.y) ).r > 0.2){
		specFactor = 1;
	}
	specFactor = texture( specTextureSampler, vec2(UV.x,1-UV.y) ).r;
	// Normal despues de la interpolacion
	vec3 n = normalize( Normal_cameraspace );
	vec3 v = normalize(EyeDirection_cameraspace);


	color= vec3(0.0,0.0,0.0);
	for(int i=0;i<LightCount;i++){// Distancia hacia la luz
		float distance = length( vec3(lights[i].position) - Position_worldspace );
		//frosbite / cryengine
		vec3 LightColor = vec3(lights[i].color)*pow(4.0/(max(distance,0.1f)),2);

		//Unreal
		//vec3 LightColor = vec3(1.0f,1.0f,1.0f)*pow(4.0,2)/(pow(distance,2)+0.1f);
		// Direccion de la luz : fragment -> luz antes de interpolacion era vertex -> luz
		vec3 l = normalize( LightDirection_cameraspace[i] );
		// vector fragmento -> camera antes vertex -> camera
		// reflejamos el vector Descarte (espejo perfecto)
		color = color + lit(l,n,v,LightColor);

	}


	

	/*if(texture( specTextureSampler, UV ).r > 0.5){
		color = vec3(1.0,0.0,0.0);
	}*/
	
	

}