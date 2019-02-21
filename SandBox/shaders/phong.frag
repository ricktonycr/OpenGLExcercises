#version 330 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace[2];
in vec4 ShadowCoord;


// Output <- color
out vec4 color;

uniform sampler2D rgbTextureSampler;
uniform sampler2D specTextureSampler;
uniform sampler2D emissiveTextureSampler;
uniform sampler2DShadow shadowMap;

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


vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);


float specFactor = 5;
vec4 MaterialDiffuseColor = texture( rgbTextureSampler, UV ).rgba;
vec4 MaterialAmbientColor = vec4(0.3,0.3,0.3,1.0) * MaterialDiffuseColor;
vec4 MaterialSpecularColor = vec4(1.0,1.0,1.0,1.0);

vec4 lit(in vec3 l,in vec3 n,in vec3 v,in vec4 lc){
	
	vec3 r = reflect(-l,n);
	float cosAlpha = clamp( dot( v,r ), 0,1 );
	
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	vec4 c = vec4(texture( emissiveTextureSampler, UV ).rgb,0.0) +  MaterialAmbientColor + MaterialDiffuseColor * lc * cosTheta + specFactor *MaterialSpecularColor * lc * pow(cosAlpha,100);
	return c;

}

void main(){

	
	if(MaterialDiffuseColor.a < 0.15){
		discard;
	}

	
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