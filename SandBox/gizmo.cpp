
#include <GL/glew.h>


#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "gizmo.h"

#include "shaderhandler.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"



Gizmo::Gizmo(){

	ModelMatrix = glm::mat4(1.0);

	vertices.push_back(glm::vec3(0.0,0.0,0.0));
	vertices.push_back(glm::vec3(100.0,0.0,0.0));
	
	vertices.push_back(glm::vec3(0.0,0.0,0.0));
	vertices.push_back(glm::vec3(0.0,100.0,0.0));
	
	vertices.push_back(glm::vec3(0.0,0.0,0.0));
	vertices.push_back(glm::vec3(0.0,0.0,100.0));

	colors.push_back(glm::vec3(255.0,0.0,0.0));
	colors.push_back(glm::vec3(255.0,0.0,0.0));


	colors.push_back(glm::vec3(0.0,255.0,0.0));
	colors.push_back(glm::vec3(0.0,255.0,0.0));

	colors.push_back(glm::vec3(0.0,0.0,255.0));
	colors.push_back(glm::vec3(0.0,0.0,255.0));

	glCreateVertexArrays(1, &VertexArrayID);

	glCreateBuffers(1, &vertexbuffer);
	glNamedBufferStorage(vertexbuffer,vertices.size() * sizeof(glm::vec3), &vertices[0],GL_DYNAMIC_STORAGE_BIT);
	
	glCreateBuffers(1, &colorbuffer);
	glNamedBufferStorage(colorbuffer,colors.size() * sizeof(glm::vec3), &colors[0],GL_DYNAMIC_STORAGE_BIT);
	


	setShaders("gizmo.vert", "gizmo.frag");


}


Gizmo::~Gizmo(){

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);


}

void Gizmo::setShaders(std::string vfile,std::string ffile){



	programID = ShaderHandler::getInstance()->getShader(vfile.c_str(), ffile.c_str());
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");


}




void Gizmo::draw(){

	glUseProgram(programID);
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();

	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


	glBindVertexArray(VertexArrayID);

	glVertexArrayVertexBuffer(VertexArrayID, 0, vertexbuffer, 0, sizeof(glm::vec3));
	glVertexArrayAttribFormat(VertexArrayID, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VertexArrayID, 0, 0);
	glEnableVertexArrayAttrib(VertexArrayID, 0);
	
	glVertexArrayVertexBuffer(VertexArrayID, 1, colorbuffer, 0, sizeof(glm::vec3));
	glVertexArrayAttribFormat(VertexArrayID, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VertexArrayID, 1, 1);
	glEnableVertexArrayAttrib(VertexArrayID, 1);
	
	glDrawArrays(GL_LINES, 0, vertices.size() );


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);




}