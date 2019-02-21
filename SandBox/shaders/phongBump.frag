#version 330 core

// datos recibidos del vertex shader despues de interpolacion
in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_tangentspace;
in vec3 Normal_tangentspace;
in vec3 LightDirection_tangentspace[2];
in vec4 ShadowCoord;

// Output <- color
out vec3 color;

uniform sampler2DShadow shadowMap;
uniform sampler2D rgbTextureSampler;
uniform sampler2D specTextureSampler;
uniform sampler2D bumpTextureSampler;
uniform mat4 MV;
uniform int LightCount;
uniform vec2 TexelOffset;
uniform bool IsReceiver;

struct Light {
	vec4 position ;
	vec4 color ;
};

layout(std140) uniform LightBlock{
	Light lights[2];
};


vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float specFactor = 5;
vec3 MaterialDiffuseColor = texture( rgbTextureSampler, UV ).rgb;//vec3(UV,0.0);;
vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
vec3 MaterialSpecularColor = vec3(1.0,1.0,1.0);

vec3 lit(in vec3 l,in vec3 n,in vec3 v,in vec3 lc){
	
	vec3 r = reflect(-l,n);
	float cosAlpha = clamp( dot( v,r ), 0,1 );
	
	float cosTheta = clamp( dot( n,l ), 0,1 );
	

	vec3 c = MaterialAmbientColor + MaterialDiffuseColor * lc * cosTheta + specFactor *MaterialSpecularColor * lc * pow(cosAlpha,100);
	return c;

}

void main(){

	

	
	specFactor = texture( specTextureSampler, UV ).r;
	// Normal despues de la interpolacion
	vec3 n = normalize(Normal_tangentspace);

	float bump1 = texture(bumpTextureSampler,UV).r;
	float bump2 = texture(bumpTextureSampler,vec2(UV.x+TexelOffset.x,UV.y)).r;
	float bump3 = texture(bumpTextureSampler,vec2(UV.x,UV.y+TexelOffset.y)).r;

	float tx = bump2 - bump1;
	float ty = bump3 - bump1;
	n = vec3(n.x-tx*2,n.y-ty*2,n.z);
	n = normalize(n);



	vec3 v = normalize(EyeDirection_tangentspace);


	color= vec3(0.0,0.0,0.0);
	for(int i=0;i<LightCount;i++){// Distancia hacia la luz
		float distance = length( vec3(lights[i].position) - Position_worldspace );
		//frosbite / cryengine
		vec3 LightColor = vec3(lights[i].color)*pow(4.0/(max(distance,0.1f)),2);

		//Unreal
		//vec3 LightColor = vec3(1.0f,1.0f,1.0f)*pow(4.0,2)/(pow(distance,2)+0.1f);
		// Direccion de la luz : fragment -> luz antes de interpolacion era vertex -> luz
		vec3 l = normalize( LightDirection_tangentspace[i] );
		// vector fragmento -> camera antes vertex -> camera
		// reflejamos el vector Descarte (espejo perfecto)
		color = color + lit(l,n,v,LightColor);

	}


	if(IsReceiver){
		float bias = 0.0005;
		float visibility = 1.0;
		for (int i=0;i<4;i++){
			int index = i;
			visibility -= 0.2*(1.0-texture( shadowMap, vec3(ShadowCoord.xy + poissonDisk[index]/700.0,  (ShadowCoord.z-bias)/ShadowCoord.w) ));
		}
		color=visibility*color;
	}

	

}