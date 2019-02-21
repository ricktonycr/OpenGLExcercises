
#include <GL/glew.h>


#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

#include "shadowmap.h"
#include "shaderhandler.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"



ShadowMap::ShadowMap(int w,int h){

	width = w;
	height = h;
	glGenFramebuffers(1, &FramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferID);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, width, height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);

}


ShadowMap::~ShadowMap(){

	glDeleteFramebuffers(1, &FramebufferID);
	glDeleteTextures(1, &depthTexture);
}

 void ShadowMap::setShadowMap(){
 	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferID);
	glViewport(0,0,width,height);




 }