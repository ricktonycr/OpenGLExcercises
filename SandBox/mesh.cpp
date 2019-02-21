
#include <GL/glew.h>


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


#include <iostream>

#include "mesh.h"

#include "shaderhandler.h"
#include "lighthandler.h"
#include "texturehandler.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "common/tangentspace.hpp"



Mesh::Mesh(std::string modelPath){
	texturePath = modelPath;
	TextureDiff=0;
	TextureSpec=0;
	TextureBump=0;
	TextureNorm=0;
	TextureEmissive=0;
	ModelMatrix = glm::mat4(1.0);
	depthBiasMVP = glm::mat4(1.0);
	isRoot=false;
	lightCount=0;
	format = GL_RGB;
	occluder=false;
	hasEmissive=false;
	receiver=false;
}

Mesh::~Mesh(){
	
	if(isRoot){
		glDeleteVertexArrays(1, &VertexArrayID);
		for(Mesh* m : vmesh){
			delete m;
		}
		return;
	}
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &tangenbuffer);
	glDeleteBuffers(1, &bitangentbuffer);
	glDeleteBuffers(1, &lightbuffer);

	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureDiff);
	glDeleteTextures(1, &TextureSpec);
	glDeleteTextures(1, &TextureBump);
	glDeleteTextures(1, &TextureNorm);
	glDeleteTextures(1, &ShadowMap);

	for(Mesh* m : vmesh){
		delete m;
	}
	delete skel;


}

void Mesh::setShaders(std::string vfile,std::string ffile){

	if(isRoot){

		for(Mesh* m : vmesh){
			m->setShaders(vfile, ffile);
		}
		return;
	}
	programID = ShaderHandler::getInstance()->getShader( vfile.c_str(), ffile.c_str());
	//std::cout << "PROGRAMID :: " << programID << " " <<vfile << std::endl;

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	TextureID  = glGetUniformLocation(programID, "rgbTextureSampler");
	TextureSpecID  = glGetUniformLocation(programID, "specTextureSampler");
	TextureBumpID  = glGetUniformLocation(programID, "bumpTextureSampler");
	TextureNormID  = glGetUniformLocation(programID, "normTextureSampler");
	TextureEmissiveID  = glGetUniformLocation(programID, "emissiveTextureSampler");
	TexelOffsetID  = glGetUniformLocation(programID, "TexelOffset");

	//shadow
	depthBiasMVPID = glGetUniformLocation(programID, "DepthBiasMVP");
	ShadowMapID  = glGetUniformLocation(programID, "shadowMap");
	isReceiverID  = glGetUniformLocation(programID, "IsReceiver");

	//create shadowmap
	depthMVPID = glGetUniformLocation(programID, "depthMVP");


	loadLights();

}

void Mesh::autoShaders(){

	if(isRoot){

		for(Mesh* m : vmesh){
			m->autoShaders();
		}
		return;
	}

	if(TextureDiff==0){
		setShaders("StandardShading.vert", "phongNoText.frag");
	}else if(TextureBump){
		setShaders("tangentShading.vert", "phongBump.frag");

	}else if(TextureNorm){
		setShaders("tangentShading.vert", "normal.frag");

	}else{
		setShaders("StandardShading.vert", "phong.frag");

	}

	for(Mesh* m : vmesh){
		m->autoShaders();
	}
}

void Mesh::autoShadersForBloom()
{
	if (isRoot) {

		for (Mesh* m : vmesh) {
			m->autoShadersForBloom();
		}
		return;
	}
	//std::cout << "AUTO CHOOSE SHADERS, DIFF"<< TextureDiff << " BUMP" << TextureBump 
	//<< " NORM" << TextureNorm << " EMISSIVE " << TextureEmissive << " SPEC " << TextureSpec  << std::endl;
	if (TextureDiff == 0) {
		setShaders("StandardShading.vert", "phongNoTextBloom.frag");
	}
	else if (TextureBump) {
		setShaders("tangentShading.vert", "phongBumpBloom.frag");

	}
	else if (TextureNorm) {
		setShaders("tangentShading.vert", "normalBloom.frag");

	}
	else {
		setShaders("StandardShading.vert", "phongBloom.frag");

	}

	for (Mesh* m : vmesh) {
		m->autoShadersForBloom();
	}
}
void Mesh::autoShadersForGlow()
{
	
	if (isRoot) {

		for (Mesh* m : vmesh) {
			m->autoShadersForGlow();
		}
		return;
	}
	setShaders("emissive.vert", "emissive.frag");

	for (Mesh* m : vmesh) {
		m->autoShadersForGlow();
	}
}

void Mesh::loadTextureDiff(std::string filename, bool propagate){

	if(isRoot || propagate){
		for(Mesh* m : vmesh){
			m->loadTextureDiff(filename,propagate);
		}
		if(isRoot)
			return;
	}

	std::cout << "LOAD TEX ::::::" <<texturePath + filename<< std::endl;

	TextureDiff = TextureHandler::getInstance()->getTextureID(texturePath + filename);
	TextureHandler::getInstance()->getTextureFormat(texturePath + filename,format);

	
}


void Mesh::loadTextureSpec(std::string filename, bool propagate){

	if(isRoot || propagate){
		for(Mesh* m : vmesh){
			m->loadTextureSpec(filename,propagate);
		}
		if(isRoot)
			return;
	}

	TextureSpec = TextureHandler::getInstance()->getTextureID(texturePath + filename);

}

void Mesh::loadTextureBump(std::string filename, bool propagate){

	if(isRoot || propagate){
		for(Mesh* m : vmesh){
			m->loadTextureBump(filename,propagate);
		}
		if(isRoot)
			return;
	}

	TextureBump = TextureHandler::getInstance()->getTextureID(texturePath + filename);
	int w,h;
	TextureHandler::getInstance()->getTextureSize(texturePath + filename,w,h);
	texelOffset.x=1.0/float(w);
	texelOffset.y=1.0/float(h);

}

void Mesh::loadTextureNormal(std::string filename, bool propagate){

	if(isRoot || propagate){
		for(Mesh* m : vmesh){
			m->loadTextureNormal(filename,propagate);
		}
		if(isRoot)
			return;
	}

	TextureNorm = TextureHandler::getInstance()->getTextureID(texturePath + filename);
}

void Mesh::loadTextureEmissive(std::string filename, bool propagate){

	if(isRoot || propagate){
		for(Mesh* m : vmesh){
			m->loadTextureEmissive(filename,propagate);
		}
		if(isRoot)
			return;
	}

	TextureEmissive = TextureHandler::getInstance()->getTextureID(texturePath + filename);
	hasEmissive = true;
}

void Mesh::setReceiver(bool r,bool propagate) {
	if(isRoot || propagate){
		for(Mesh* m : vmesh){
			m->setReceiver(r,propagate);
		}
		if(isRoot)
			return;
	}

	receiver=r;
}
void Mesh::setOccluder(bool o,bool propagate) {
	if(isRoot || propagate){
		for(Mesh* m : vmesh){
			m->setOccluder(o,propagate);
		}
		if(isRoot)
			return;
	}

	occluder=o;
}

void Mesh::setShadowMap(GLuint sm){
	for(Mesh* m : vmesh){
		m->setShadowMap(sm);
	}
	if(isRoot)
		return;
	
	ShadowMap=sm;


}


void Mesh::loadLights(){

	lightBindingPoint =0;

	LightsBlockID = glGetUniformBlockIndex(programID, "LightBlock");
	LightCountID = glGetUniformLocation(programID, "LightCount");
	glUniformBlockBinding(programID,LightsBlockID,lightBindingPoint);

	lightCount = LightHandler::getInstance()->vLights.size();
	lightbuffer = LightHandler::getInstance()->getLightBuffer();
	glBindBuffer(GL_UNIFORM_BUFFER, lightbuffer);


}

void Mesh::loadMesh(){

	polygonCount = vertices.size();

	if (polygonCount == 0)
		return;
	

	if(uvs.size()>0)
		computeTangentBasis(vertices,uvs,normals,tangents,bitangents);
	

	glCreateVertexArrays(1, &VertexArrayID);
	glCreateBuffers(1, &vertexbuffer);
	glNamedBufferStorage(vertexbuffer,vertices.size() * sizeof(glm::vec3), &vertices[0],GL_DYNAMIC_STORAGE_BIT);


	if(uvs.size()>0){
		glCreateBuffers(1, &uvbuffer);
		glNamedBufferStorage(uvbuffer,uvs.size() * sizeof(glm::vec2), &uvs[0],GL_DYNAMIC_STORAGE_BIT);
		

	}

	glCreateBuffers(1, &normalbuffer);
	glNamedBufferStorage(normalbuffer,normals.size() * sizeof(glm::vec3), &normals[0],GL_DYNAMIC_STORAGE_BIT);
	

	if (uvs.size() > 0) {
		glCreateBuffers(1, &tangenbuffer);
		glNamedBufferStorage(tangenbuffer, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &bitangentbuffer);
		glNamedBufferStorage(bitangentbuffer, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_DYNAMIC_STORAGE_BIT);
	}
	

	/*vertices.clear();
	uvs.clear();
	normals.clear();
	tangents.clear();
	bitangents.clear();*/


}


void Mesh::draw(glm::mat4 modelmat){


	//skel->ModelMatrix = glm::translate(ModelMatrix,glm::vec3(0,0,1));
	//skel->draw();

	//std::cout <<"\n\n" << name << "\n" << glm::to_string(ModelMatrix) << std::endl;
	if(false){//format == GL_RGBA){
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glm::mat4 Model = ModelMatrix;
	Model = modelmat*Model;
	if(isRoot || polygonCount==0){

		for(Mesh* m : vmesh){
			m->draw(Model);
		}
		return;
	}

	glUseProgram(programID);
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * Model;

	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
	glm::mat4 depthViewMatrix = glm::lookAt(LightHandler::getInstance()->lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * Model;
	depthBiasMVP = getBiasMatrix()*depthMVP;



		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
	

	glUniformMatrix4fv(depthBiasMVPID, 1, GL_FALSE, &depthBiasMVP[0][0]);
	glUniformMatrix4fv(depthMVPID, 1, GL_FALSE, &depthMVP[0][0]);

	glBindBufferBase(GL_UNIFORM_BUFFER,lightBindingPoint,lightbuffer);

	glUniform1i(LightCountID, lightCount);

	glUniform1i(isReceiverID, receiver);

	glUniform2fv(TexelOffsetID,1, &texelOffset[0]);
	

		// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureDiff);
	glUniform1i(TextureID, 0);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureSpec);
	glUniform1i(TextureSpecID, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureBump);
	glUniform1i(TextureBumpID, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureNorm);
	glUniform1i(TextureNormID, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureEmissive);
	glUniform1i(TextureEmissiveID, 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, ShadowMap);
	glUniform1i(ShadowMapID, 5);

	/*std::cout <<" programID " << programID <<" ShadowMapID " << ShadowMapID << std::endl;
	std::cout <<" programID " << programID <<" depthText " << ShadowMap << std::endl;
	std::cout <<" programID " << programID <<" depthMVPID " << depthMVPID << std::endl;
	std::cout <<" programID " << programID <<" Bias " << depthBiasMVPID << std::endl;
	std::cout <<" programID " << programID <<" MatrixID " << MatrixID << std::endl;
	std::cout <<" programID " << programID <<" ModelMatrixID " << ModelMatrixID << std::endl;
	std::cout <<" programID " << programID <<" ViewMatrixID " << ViewMatrixID << std::endl;*/


		// 1rst attribute buffer : vertices

	/*glVertexArrayVertexBuffer(VertexArrayID,0,vertexbuffer,0, sizeof(glm::vec3));
	glVertexArrayAttribFormat(VertexArrayID,0,3,GL_FLOAT,GL_FALSE,0);*/
	//std::cout <<" VertexArrayID " << VertexArrayID <<" name " << name <<" poly : " << polygonCount<< std::endl;
	glBindVertexArray(VertexArrayID);
	
	glVertexArrayVertexBuffer(VertexArrayID, 0, vertexbuffer, 0, sizeof(glm::vec3));
	glVertexArrayAttribFormat(VertexArrayID, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VertexArrayID,0, 0);
	glEnableVertexArrayAttrib(VertexArrayID, 0);

	if (normals.size()) {
		glVertexArrayVertexBuffer(VertexArrayID, 2, normalbuffer, 0, sizeof(glm::vec3));
		glVertexArrayAttribFormat(VertexArrayID, 2, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(VertexArrayID, 2, 2);
		glEnableVertexArrayAttrib(VertexArrayID, 2);

	}

	if (uvs.size()) {

		glVertexArrayVertexBuffer(VertexArrayID, 1, uvbuffer, 0, sizeof(glm::vec2));
		glVertexArrayAttribFormat(VertexArrayID, 1, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(VertexArrayID, 1, 1);
		glEnableVertexArrayAttrib(VertexArrayID, 1);

		glVertexArrayVertexBuffer(VertexArrayID, 3, tangenbuffer, 0, sizeof(glm::vec3));
		glVertexArrayAttribFormat(VertexArrayID, 3, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(VertexArrayID, 3, 3);
		glEnableVertexArrayAttrib(VertexArrayID, 3);

		glVertexArrayVertexBuffer(VertexArrayID, 4, bitangentbuffer, 0, sizeof(glm::vec3));
		glVertexArrayAttribFormat(VertexArrayID, 4, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(VertexArrayID, 4, 4);
		glEnableVertexArrayAttrib(VertexArrayID, 4);
	}

	glDrawArrays(GL_TRIANGLES, 0, polygonCount );



	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisable(GL_BLEND);


	
	for(Mesh* m : vmesh){
		m->draw(ModelMatrix);
	}




}


