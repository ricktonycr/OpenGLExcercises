
#ifndef BLOOM_H
#define BLOOM_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Object3D;

class Bloom
{
	// Access specifier 
public:

	Bloom(int w, int h, std::vector<Object3D*> * _objects);
	~Bloom();


	void renderObjects();
	void draw();
	void render();
	GLuint FramebufferID;
	GLuint renderedTexture;
	GLuint depthrenderbuffer;



	GLuint programIDBlurX;
	GLuint programIDBlurY;
	GLuint texID;
	GLuint quad_vertexbuffer;
	GLuint TexelOffsetID;

	int width;
	int height;
	glm::vec2 texelOffset;
	std::vector<Object3D*> * objects;
};


#endif