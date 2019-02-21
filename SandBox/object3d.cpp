
#include <GL/glew.h>


#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

#include "object3d.h"
#include "mesh.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "shaderhandler.h"



Object3D::Object3D(){

	ModelMatrix = glm::mat4(1.0);
}

Object3D::Object3D(std::string path, bool _occluder, bool _receiver, bool emissive){

	ModelMatrix = glm::mat4(1.0);
	modelPath = path;

	occluder=_occluder;
	receiver=_receiver;
	hasEmissive = emissive;

	loadMesh(); 
	

}

Object3D::~Object3D(){

	if(rootMesh != NULL)
		delete rootMesh;


}

void Object3D::setShaders(std::string vfile,std::string ffile){

	rootMesh->setShaders(vfile,ffile);

}

void Object3D::autoShaders() {

	rootMesh->autoShaders();
}
void Object3D::autoShadersForGlow() {

	rootMesh->autoShadersForGlow();
}

void Object3D::autoShadersForBloom() {

	rootMesh->autoShadersForBloom();
}

void Object3D::loadMesh(){

	rootMesh = new Mesh(modelPath+"textures/");
	rootMesh->isRoot=true;

	loadOBJ(std::string(modelPath+"source/model.obj").c_str(),this);

	rootMesh->setReceiver(receiver,true);
	rootMesh->setOccluder(occluder,true);

}

void Object3D::loadTextureDiffuse(std::string filename){
	rootMesh->loadTextureDiff(filename);
}

void Object3D::loadTextureSpec(std::string filename){
	rootMesh->loadTextureSpec(filename);
}

void Object3D::loadTextureBump(std::string filename){
	rootMesh->loadTextureBump(filename);
}

void Object3D::loadTextureNormal(std::string filename){
	rootMesh->loadTextureNormal(filename);
}

void Object3D::loadTextureEmissive(std::string filename){
	rootMesh->loadTextureEmissive(filename);
}

void Object3D::setShadowMap(GLuint shadowMap){
	rootMesh->setShadowMap(shadowMap);

}

void Object3D::draw(){

	rootMesh->draw(ModelMatrix);

}
