// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>


// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>

GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#include "object3d.h"
#include "gizmo.h"
#include "lighthandler.h"
#include "shaderhandler.h"
#include "fbo.h"
#include "bloom.h"
#include "glow.h"
#include "shadowmap.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"



int main( int argc, char* argv[] )
{

	std::string filename="merc";
	float rotatex=0.0f;
	float scaleUniform=1.0f;

	bool useGlow = true;
	bool shadow = true;
	bool useBloom = true;
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE); 

	// Open a window and create its OpenGL context
	int width = 1024;
	int height = 768;
	window = glfwCreateWindow( width, height, "Phong Shader", NULL, NULL);

	glfwSetWindowPos(window, 200, 0);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, width/2, height/2);


	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
	//glEnable(GL_CULL_FACE);

	LightHandler::getInstance()->addLight(glm::vec4(5.0,2.0,0.2,1.0), glm::vec4(0.4,0.4,0.4,1.0));
	LightHandler::getInstance()->addLight(glm::vec4(-4.0,4.0,1.0,1.0), glm::vec4(0.0,0.0,0.0,1.0));
	LightHandler::getInstance()->addLightForShadow(glm::vec3(4.0,5.0,3.0));

	
	Gizmo* gizmo = new Gizmo();
	Object3D* mainModel = new Object3D("../SandBox/data/"+filename+"/",shadow,false,true);
	mainModel->loadTextureDiffuse("merc.jpg");
	mainModel->loadTextureSpec("spec.jpg");
	mainModel->loadTextureEmissive("emissive.jpg");
	
	mainModel->autoShaders();
	mainModel->ModelMatrix = glm::scale(mainModel->ModelMatrix,glm::vec3(scaleUniform));
	mainModel->ModelMatrix = glm::rotate(mainModel->ModelMatrix,glm::radians(rotatex),glm::vec3(1.0f,0.0f,0.0f));


	/*Object3D* floor = new Object3D("../openglSandBox/data/floor2/",true,shadow);
	floor->autoShaders();
	floor->ModelMatrix = glm::scale(floor->ModelMatrix,glm::vec3(0.01,0.01,0.01));*/



	std::vector<Object3D* >* objects =  new std::vector<Object3D*>();

	objects->push_back(mainModel);
	//objects->push_back(floor);

	FBO* fbo = NULL;
	FBO* fboEmissive = NULL;
	FBO* fboGlowX = NULL;

	Bloom* bloom = new Bloom(width / 2, height / 2, objects);
	Glow* glow = new Glow(width/2, height/2, objects);



	if(useGlow || useBloom){
		//FBO FOR Emissive Draw
		fboEmissive = new FBO(width,height,"Passthrough.vert", "fbouseGlowX.frag");
		//FBO FOR GLOW X On emissive texture
		fboGlowX = new FBO(width,height,"Passthrough.vert", "fbouseGlowX.frag");
		//FBO FOR GLOW Y On GLOW X texture and merge with normal rendering
		fbo =  new FBO(width*2,height*2,"Passthrough.vert", "fboAdd.frag");/**/

		fbo->setEmissiveTexture(glow->renderedTexture);
		fbo->setBloomTexture(bloom->renderedTexture);


		/*for (int i = 0; i < objects->size(); i++) {
			objects->at(i)->setShaders("emissive.vert", "emissive.frag");
		}*/

	}
	else{

		//
		fbo =  new FBO(width*2,height*2,"Passthrough.vert", "fbo.frag");
	}




	ShadowMap* sm = NULL;
	if(shadow){
		sm = new ShadowMap(4096,4096);
		ShaderHandler::getInstance()->getShader("ShadowPassthrough.vert", "ShadowMap.frag");
		


		for (int i = 0; i < objects->size(); i++) {
			objects->at(i)->setShadowMap(sm->depthTexture);
		}
	}



	do{


		computeMatricesFromInputs();


		if(shadow){
			if (sm == NULL)
				return 0;
			sm->setShadowMap();

				glClear(GL_DEPTH_BUFFER_BIT);

				for (int i = 0; i < objects->size(); i++) {
					if (objects->at(i)->occluder) {
						objects->at(i)->setShaders("ShadowPassthrough.vert", "ShadowMap.frag");
						objects->at(i)->draw();

					}
				}
 			glDisable(GL_CULL_FACE);

		}

		if(useGlow){
			glow->render();

		}

		if (useBloom) {
			bloom->render();
		}

		fbo->setFBO();

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

			for (int i = 0; i < objects->size(); i++) {

				objects->at(i)->autoShaders();
				objects->at(i)->draw();
			}

			gizmo->draw();
			
		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0,0,width,height);
		fbo->draw();	
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0 );

	delete fbo;

	if(useGlow){
		delete fboEmissive;
		delete fboGlowX;
	}
	if(shadow){
		delete sm;
	}
	// Cleanup VBO and shader
	// Close OpenGL window and terminate GLFW
	//delete floor;
	delete gizmo;
	//delete mainModel;
	std::cout << "END" << std::endl;
	glfwTerminate();

	return 0;
}
