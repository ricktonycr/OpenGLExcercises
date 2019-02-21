#ifndef TEXTURE_HPP
#define TEXTURE_HPP

GLuint loadBMP_custom(const char * imagepath, int &w, int &h, GLuint &_format);

GLuint loadDDS(const char * imagepath,int &w, int &h, GLuint &_format);

GLuint loadFreeImage(const char * imagepath, int& w, int& h, GLuint &_format);

GLuint loadTGA(const char * imagepath,int &w, int &h, GLuint &_format);

#endif