
#include <GL/glew.h>


#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

#include "Glow.h"
#include "object3d.h"
#include "shaderhandler.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"



Glow::Glow(int w, int h, std::vector<Object3D*> * _objects) {

	width = w;
	height = h;
	this->objects = _objects;
	texelOffset.x = 1.0 / w;
	texelOffset.y = 1.0 / h;


	for (int i = 0; i < objects->size(); i++) {
		objects->at(i)->autoShadersForGlow();
	}

	programIDBlurX = ShaderHandler::getInstance()->getShader("Passthrough.vert", "blurx.frag");
	programIDBlurY = ShaderHandler::getInstance()->getShader("Passthrough.vert", "blury.frag");

	glGenFramebuffers(1, &FramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferID);


	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);


	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);


	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);



	// The fullscreen quad's Glow
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);


	texID = glGetUniformLocation(programIDBlurX, "renderedTexture");
	TexelOffsetID = glGetUniformLocation(programIDBlurX, "TexelOffset");


	std::cout << "Glow" << glGetUniformLocation(programIDBlurX, "renderedTexture") << glGetUniformLocation(programIDBlurY, "renderedTexture") << std::endl;


}






Glow::~Glow() {

	glDeleteFramebuffers(1, &FramebufferID);
	glDeleteTextures(1, &renderedTexture);
	glDeleteRenderbuffers(1, &depthrenderbuffer);
	glDeleteBuffers(1, &quad_vertexbuffer);
}

void Glow::renderObjects() {


	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferID);
	glClearColor(0.0, 0.0, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	for (int i = 0; i < objects->size(); i++) {
		objects->at(i)->autoShadersForGlow();
		objects->at(i)->draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Glow::draw() {

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferID);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);


	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	// Set our "renderedTexture" sampler to use Texture Unit 0
	glUniform1i(texID, 0);


	glUniform2fv(TexelOffsetID, 1, &texelOffset[0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Glow::render()
{

	renderObjects();
	glUseProgram(programIDBlurX);
	draw();
	glUseProgram(programIDBlurY);
	draw();
	glUseProgram(programIDBlurX);
	draw();
	glUseProgram(programIDBlurY);
	draw();

	glUseProgram(programIDBlurX);
	draw();
	glUseProgram(programIDBlurY);
	draw();

}
