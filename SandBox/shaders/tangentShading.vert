#version 450 core

// Aqui van los vertex buffer que mandamos al GPU
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;


struct Light {
	vec4 position ;
	vec4 color ;
};

layout(std140) uniform LightBlock{
	Light lights[2];
};

// datos de salida hacia el fragment shader (lo que tenemos que calcular)
out vec2 UV;
out vec3 Position_worldspace;
out vec3 EyeDirection_tangentspace;
out vec3 LightDirection_tangentspace[2];
out vec3 Normal_tangentspace;
out vec4 ShadowCoord;

// Datos uniformes al objeto
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 DepthBiasMVP;
uniform int LightCount;

void main(){

	// gl_position es la position del vertice despues de la proyeccion
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	mat4 MV = V *M ;
	// La posicion del vertice solamente despues de la transformacion espacial (rotacion)
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	
	// Vector que va del vertice hacia la camara
	// En el espacio camara la posicion de la camara es 0,0,0
	vec3 vertexPosition_cameraspace = ( MV * vec4(vertexPosition_modelspace,1)).xyz;
	vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;


	
	// Normal despues de la transformacion 
	vec3 Normal_cameraspace = ( transpose(inverse(MV)) * vec4(vertexNormal_modelspace,0)).xyz;



	mat3 MV3x3 = mat3(MV);
	vec3 normal_cameraspace = MV3x3 * normalize(vertexNormal_modelspace);
    vec3 tangent_cameraspace =  MV3x3 * normalize(vertexTangent_modelspace);
    vec3 bitangent_cameraspace =  MV3x3 * normalize(vertexBitangent_modelspace);

    mat3 TBN = transpose(mat3(
        tangent_cameraspace,
        bitangent_cameraspace,
        normal_cameraspace
    )); // You can use dot products instead of building this matrix and transposing it. See References for details.

    EyeDirection_tangentspace =  TBN * EyeDirection_cameraspace;
    Normal_tangentspace =  TBN * Normal_cameraspace;

	// Vector que va del vertice hacia la camara( espacio camara) No hay M porque no transformamos la luz.
	for(int i=0;i<LightCount;i++){
		vec3 LightPosition_cameraspace = ( V * lights[i].position).xyz;
		vec3 LightDirection_cameraspace = LightPosition_cameraspace - vertexPosition_cameraspace;
		LightDirection_tangentspace[i] = TBN * LightDirection_cameraspace;

	}

	// UV no hacemos nada mas que interpolacion
	UV = vertexUV;
	ShadowCoord = DepthBiasMVP * vec4(vertexPosition_modelspace,1);
}

