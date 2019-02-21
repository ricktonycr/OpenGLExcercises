
#ifndef FBO_H
#define FBO_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class FBO 
{ 
    // Access specifier 
public:

 FBO(int w, int h,std::string vfile,std::string ffile);
 FBO(int w, int h);
 ~FBO();
 void setFBO();
void setShaders(std::string vfile,std::string ffile);
void setEmissiveTexture(GLuint texture);
void setBloomTexture(GLuint texture);
 void draw();
 GLuint FramebufferID;
 GLuint renderedTexture;
 GLuint renderedGlowTexture;
 GLuint renderedBloomTexture;
 GLuint depthrenderbuffer;
 GLuint programID;
 GLuint texID;
 GLuint texGlowID;
 GLuint texBloomID;
 GLuint quad_vertexbuffer;
 GLuint TexelOffsetID;
 bool useEmissive;
 bool useBloom;

 int width;
 int height;
 glm::vec2 texelOffset;

};


#endif


