#version 330 core

// datos recibidos del vertex shader despues de interpolacion
in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_tangentspace;
in vec3 Normal_tangentspace;
in vec3 LightDirection_tangentspace[2];

// Output <- color
out vec4 color;

// datos unifromes a todo el objeto
uniform sampler2D rgbTextureSampler;
uniform sampler2D specTextureSampler;
uniform sampler2D normTextureSampler;
uniform sampler2D emissiveTextureSampler;

uniform mat4 MV;
uniform int LightCount;


struct Light {
	vec4 position ;
	vec4 color ;
};

layout(std140) uniform LightBlock{
	Light lights[2];
};




float specFactor = 0.0;
vec4 MaterialDiffuseColor = texture( rgbTextureSampler, UV ).rgba;
vec4 MaterialSpecularColor = vec4(1.0,1.0,1.0,MaterialDiffuseColor.a);

vec4 lit(in vec3 l,in vec3 n,in vec3 v,in vec4 lc){
	
	vec3 r = reflect(-l,n);
	float cosAlpha = clamp( dot( v,r ), 0,1 );
	
	float cosTheta = clamp( dot( n,l ), 0,1 );
	

	vec4 c = specFactor *MaterialSpecularColor * lc * pow(cosAlpha,100) ;
	return c;

}

void main(){

	
	specFactor = texture( specTextureSampler, UV ).r;
	// Normal despues de la interpolacion
	vec3 n = normalize(texture(normTextureSampler,UV).rgb*2.0-1.0);
	vec3 v = normalize(EyeDirection_tangentspace);


	color= vec4(0.0,0.0,0.0,0.0);
	for(int i=0;i<LightCount;i++){// Distancia hacia la luz
		float distance = length( vec3(lights[i].position) - Position_worldspace );
		//frosbite / cryengine
		vec4 LightColor = lights[i].color*pow(4.0/(max(distance,0.1f)),2);

		//Unreal
		//vec3 LightColor = vec3(1.0f,1.0f,1.0f)*pow(4.0,2)/(pow(distance,2)+0.1f);
		// Direccion de la luz : fragment -> luz antes de interpolacion era vertex -> luz
		vec3 l = normalize( LightDirection_tangentspace[i] );
		// vector fragmento -> camera antes vertex -> camera
		// reflejamos el vector Descarte (espejo perfecto)
		color = color + lit(l,n,v,LightColor);

	}
	//color=vec4(1.0,0.0,0.0,1.0);

}