
#ifndef OBJECT3D_H
#define OBJECT3D_H


#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Mesh;


class Object3D 
{ 
    // Access specifier 
public:

 Object3D();
 Object3D(std::string path, bool occulder = false, bool receiver = false,bool emissive=false);
 ~Object3D();

 void setShaders(std::string vfile,std::string ffile);
 void autoShaders();
 void autoShadersForBloom();
 void autoShadersForGlow();
 void loadMesh();
 void loadTextureDiffuse(std::string filename);
 void loadTextureSpec(std::string filename);
 void loadTextureBump(std::string filename);
 void loadTextureNormal(std::string filename);
 void loadTextureEmissive(std::string filename);
 void setShadowMap(GLuint shadowMap);
 void draw();

 glm::mat4 ModelMatrix;
 std::string modelPath;
 Mesh* rootMesh;
 float load_scale;


 bool occluder;
 bool receiver;
 bool hasEmissive;



}; 

#endif