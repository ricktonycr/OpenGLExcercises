#version 330 core

// datos recibidos del vertex shader despues de interpolacion
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Output <- color
out vec3 color;

// datos unifromes a todo el objeto
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main(){

	
	// Material del objeto tb deberian de ser uniformes
	vec3 MaterialDiffuseColor = vec3(1.0,1.0,1.0);//texture( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(1.0,1.0,1.0);

	//frosbite / cryengine
	vec3 LightColor = vec3(1.0f,1.0f,1.0f);

	//Unreal
	vec3 LightColor = vec3(1.0f,1.0f,1.0f)*pow(4.0,2)/(pow(distance,2)+0.1f);

	// Normal despues de la interpolacion
	vec3 n = normalize( Normal_cameraspace );
	// Direccion de la luz : fragment -> luz antes de interpolacion era vertex -> luz
	vec3 l = normalize( LightDirection_cameraspace );

	// Coseno del angulo entre la normal y la luz
	// Producto punto ya que los vectos son normalizados
	// clamped  0 - 1
	//  - luz vertical -> 1
	//  - luz perpandicular -> 0
	//  - luz detras de triangulo -> 0

	//Codigo aqui
	
	// vector fragmento -> camera antes vertex -> camera
	vec3 E = normalize(EyeDirection_cameraspace);
	// reflejamos el vector Descarte (espejo perfecto)
	

	//Codigo aqui
	
	// coseno entre luz reflejada y la direccion hacia la camara
	// clamped  0-1
	//  - Miramos en la reflexion -> 1
	//  - nos alejamos de la reflexion -> < 1
	

	//Codigo aqui


	
	

	color = vec3(1.0,0.0,0.0);
	/*color = Codigo aqui*/

}