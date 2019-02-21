
#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class ShadowMap 
{ 
    // Access specifier 
public:

 ShadowMap(int w, int h);
 ~ShadowMap();
 void setShadowMap();


 GLuint FramebufferID;
 GLuint depthTexture;
 GLuint renderedTexture;
 GLuint programID;

 int width;
 int height;

};


#endif


