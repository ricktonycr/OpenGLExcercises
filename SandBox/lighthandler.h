
#ifndef LIGHTHANDLER_H
#define LIGHTHANDLER_H


#include <vector>
#include <string>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Light{
	
	glm::vec4 pos;
	glm::vec4 color;
};

class LightHandler 
{ 
    // Access specifier 
private:
 static LightHandler* lightHandler;
 explicit LightHandler();
 ~LightHandler();

public:
 static LightHandler* getInstance();
 GLuint lightbuffer;
 GLuint getLightBuffer();
 void addLight(glm::vec4 pos,glm::vec4 color);
 void addLightForShadow(glm::vec3 lightDir);

 glm::mat4 depthMVP;
 std::vector<Light> vLights;
 glm::vec3 lightInvDir;

};

#endif