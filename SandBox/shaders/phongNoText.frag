#version 450 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace[2];
in vec4 ShadowCoord;

// Output <- color
out vec3 color;

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

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

vec3 MaterialDiffuseColor = vec3(0.3,0.3,0.3);
vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
vec3 MaterialSpecularColor = vec3(0.4,0.4,0.4);

vec3 lit(in vec3 l,in vec3 n,in vec3 v,in vec3 lc){
	
	vec3 r = reflect(-l,n);
	float cosAlpha = clamp( dot( v,r ), 0,1 );
	
	float cosTheta = clamp( dot( n,l ), 0,1 );
	

	vec3 c = MaterialAmbientColor + MaterialDiffuseColor * lc * cosTheta +MaterialSpecularColor * lc * pow(cosAlpha,100);
	return c;

}

void main(){

	
	vec3 n = normalize( Normal_cameraspace );
	vec3 v = normalize(EyeDirection_cameraspace);


	color= vec3(0.0,0.0,0.0);
	for(int i=0;i<LightCount;i++){// Distancia hacia la luz
		float distance = length( vec3(lights[i].position) - Position_worldspace );
		//frosbite / cryengine
		vec3 LightColor = vec3(lights[i].color)*pow(4.0/(max(distance,0.1f)),2);
		//vec3 LightColor = vec3(lights[i].color);
		//Unreal
		//vec3 LightColor = vec3(1.0f,1.0f,1.0f)*pow(4.0,2)/(pow(distance,2)+0.1f);
		// Direccion de la luz : fragment -> luz antes de interpolacion era vertex -> luz
		vec3 l = normalize( LightDirection_cameraspace[i] );
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