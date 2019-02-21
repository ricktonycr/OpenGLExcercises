#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <GL/glew.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <map>

#include <FreeImage.h>

#include <IL/il.h>
#include <IL/ilu.h>



GLuint loadFreeImage(const char * filename, int &w, int &h, GLuint &_format)
{

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);


	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if (fif == FIF_UNKNOWN)
		return 0;

	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if (!dib)
		return 0;


	FIBITMAP* temp = FreeImage_ConvertTo32Bits(dib);


	//retrieve the image data
	bits = FreeImage_GetBits(temp);
	//get the image width and height
	width = FreeImage_GetWidth(temp);
	height = FreeImage_GetHeight(temp);

	w = width;
	h = height;
	//if this somehow one of these failed (they shouldn't), return failure
	if ((bits == 0) || (width == 0) || (height == 0))
		return 0;


	std::cout << " FREEIMAGEEEEE " << FreeImage_GetFIFDescription(fif) << std::endl;

	_format = GL_RGBA;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);


	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);
	FreeImage_Unload(temp);
	return textureID;
}

GLuint loadDevIL(const char * filename, int &w, int &h, GLuint &_format)
{

	ilInit();
	iluInit();
	ILuint imageID;
	bool success;
	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage((const ILstring)(filename)); 	// Load the image file

	_format = GL_RGBA;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);


	//Free FreeImage's copy of the data
	return textureID;
}


GLuint loadBMP_custom(const char * imagepath, int &w, int &h, GLuint &_format){

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if ( fread(header, 1, 54, file)!=54 ){ 
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}
	if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	w=width;
	h=height;

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);

	// Everything is in memory now, the file can be closed.
	fclose (file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete [] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	_format = GL_RGB;
	// Return the ID of the texture we just created
	return textureID;
}

// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library, 
// or do it yourself (just like loadBMP_custom and loadDDS)
//GLuint loadTGA_glfw(const char * imagepath){
//
//	// Create one OpenGL texture
//	GLuint textureID;
//	glGenTextures(1, &textureID);
//
//	// "Bind" the newly created texture : all future texture functions will modify this texture
//	glBindTexture(GL_TEXTURE_2D, textureID);
//
//	// Read the file, call glTexImage2D with the right parameters
//	glfwLoadTexture2D(imagepath, 0);
//
//	// Nice trilinear filtering.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	// Return the ID of the texture we just created
//	return textureID;
//}



#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath, int &w, int &h, GLuint &_format){

	unsigned char header[124];

	FILE *fp; 

	/* try to open the file */ 
	fp = fopen(imagepath, "rb"); 
	if (fp == NULL){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); 
		return 0;
	}

	/* verify the type of file */ 
	char filecode[4]; 
	fread(filecode, 1, 4, fp); 
	if (strncmp(filecode, "DDS ", 4) != 0) { 
		fclose(fp); 
		return 0; 
	}
	
	/* get the surface desc */ 
	fread(&header, 124, 1, fp); 

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);

	w=width;
	h=height;


	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */ 
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
	fread(buffer, 1, bufsize, fp); 
	/* close the file pointer */ 
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	unsigned int format;
	switch(fourCC) 
	{ 
		case FOURCC_DXT1: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
		break; 
		case FOURCC_DXT3: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
		break; 
		case FOURCC_DXT5: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
		break; 
		default: 
		free(buffer); 
		return 0; 
	}

	_format = GL_RGBA;

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	unsigned int offset = 0;

	/* load the mipmaps */ 
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
			0, size, buffer + offset); 

		offset += size; 
		width  /= 2; 
		height /= 2; 

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if(width < 1) width = 1;
		if(height < 1) height = 1;

	} 

	free(buffer); 

	return textureID;


}


typedef struct
{
    GLubyte Header[12];         // File Header To Determine File Type
} TGAHeader;
 
typedef struct
{
    GLubyte header[6];          // Holds The First 6 Useful Bytes Of The File
    GLuint bytesPerPixel;           // Number Of BYTES Per Pixel (3 Or 4)
    GLuint imageSize;           // Amount Of Memory Needed To Hold The Image
    GLuint type;                // The Type Of Image, GL_RGB Or GL_RGBA
    GLuint Height;              // Height Of Image                 
    GLuint Width;               // Width Of Image              
    GLuint Bpp;             // Number Of BITS Per Pixel (24 Or 32)
} TGA;





// Load A TGA File! FROM NEHE
GLuint loadTGA(const char * imagepath,int &w, int &h, GLuint &_format)
{
	TGA tga; 
	GLuint textureID;
	GLuint type;
	GLubyte* data;
	// Uncompressed TGA Header
	GLubyte uTGAcompare[12] = {0,0, 2,0,0,0,0,0,0,0,0,0};
	// Compressed TGA Header
	GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

	FILE * fTGA;                    // Declare File Pointer
	fTGA = fopen(imagepath, "rb");           // Open File For Reading

	if(fTGA == NULL)                // If Here Was An Error
	{
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
	    return 0;               // Return False
	} 
	TGAHeader tgaheader;
	// Attempt To Read The File Header
	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)
	{
		printf("%s could not read file header !\n", imagepath);
	    return 0;               // Return False If It Fails
	}    

	// If The File Header Matches The Uncompressed Header
	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
	{
	    // Attempt To Read Next 6 Bytes
		if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
		{                                      
			printf("%s could not read file !\n", imagepath);   
		    return 0;               // Return False
		}  
		w  = tga.header[1] * 256 + tga.header[0];   // Calculate Height
		h = tga.header[3] * 256 + tga.header[2];   // Calculate The Width
		int bpp = tga.header[4];                // Calculate Bits Per Pixel
		tga.Width = w;              // Copy Width Into Local Structure
		tga.Height = h;                // Copy Height Into Local Structure
		tga.Bpp = bpp;  
		// Make Sure All Information Is Valid
		if((w <= 0) || (h <= 0) || ((bpp != 24) && (bpp !=32)))
		{
			printf("%s could not read file !\n", imagepath);   
		    return 0;               // Return False
		}
		type=0;
		if(bpp == 24)               // Is It A 24bpp Image?
    		type    = GL_RGB;       // If So, Set Type To GL_RGB
		else                        // If It's Not 24, It Must Be 32
    		type    = GL_RGBA;      // So Set The Type To GL_RGBA

    	tga.bytesPerPixel = (tga.Bpp / 8);      // Calculate The BYTES Per Pixel
		// Calculate Memory Needed To Store Image
		tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);
		// Allocate Memory
		data = (GLubyte *)malloc(tga.imageSize);
		if(data == NULL)           // Make Sure It Was Allocated Ok
		{
			printf("%s could not read file !\n", imagepath);   
		    return 0;               // If Not, Return False
		}

		// Attempt To Read All The Image Data
		if(fread(data, 1, tga.imageSize, fTGA) != tga.imageSize)
		{
			printf("%s could not read file !\n", imagepath);                     
		    return 0;               // If We Cant, Return False
		}
		 // Start The Loop
	    for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	    {
	        // 1st Byte XOR 3rd Byte XOR 1st Byte XOR 3rd Byte
	        data[cswap] ^= data[cswap+2] ^=
	        data[cswap] ^= data[cswap+2];
	    }
	 	
	    fclose(fTGA);

	}
	// If The File Header Matches The Compressed Header
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
	{                                          
		if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
	    {
			printf("%s could not read file !\n", imagepath);                     
		    return 0;               // If We Cant, Return False
	    }                          
	    w  = tga.header[1] * 256 + tga.header[0];
	    h = tga.header[3] * 256 + tga.header[2];
	    int bpp = tga.header[4];
	    tga.Width   = w;
	    tga.Height  = h;
	    tga.Bpp = bpp;
		// Make Sure All Information Is Valid
		if((w <= 0) || (h <= 0) || ((bpp != 24) && (bpp !=32)))
		{
			printf("%s could not read file !\n", imagepath);   
		    return 0;               // Return False
		}
		type=0;
		if(bpp == 24)               // Is It A 24bpp Image?
    		type    = GL_RGB;       // If So, Set Type To GL_RGB
		else                        // If It's Not 24, It Must Be 32
    		type    = GL_RGBA;      // So Set The Type To GL_RGBA

    	tga.bytesPerPixel = (tga.Bpp / 8);      // Calculate The BYTES Per Pixel
		// Calculate Memory Needed To Store Image
		tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);
		// Allocate Memory
		data = (GLubyte *)malloc(tga.imageSize);

		// Allocate Memory To Store Image Data
		data  = (GLubyte *)malloc(tga.imageSize);
		if(data == NULL)           // If Memory Can Not Be Allocated...
		{
			printf("%s could not read file !\n", imagepath);   
		    return 0;               // Return False
		}  
		GLuint pixelcount = tga.Height * tga.Width; // Number Of Pixels In The Image
		GLuint currentpixel = 0;            // Current Pixel We Are Reading From Data
		GLuint currentbyte  = 0;            // Current Byte We Are Writing Into Imagedata
		// Storage For 1 Pixel
		GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);
		do                      // Start Loop
		{
			GLubyte chunkheader = 0;            // Variable To Store The Value Of The Id Chunk
			if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)  // Attempt To Read The Chunk's Header
			{
				printf("%s could not read file !\n", imagepath);   
			    return 0;               // Return False
			}
			if(chunkheader < 128)                // If The Chunk Is A 'RAW' Chunk
			{                                                  
    			chunkheader++;  
    			// Start Pixel Reading Loop
				for(short counter = 0; counter < chunkheader; counter++)
				{
				    // Try To Read 1 Pixel
				    if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
				    {
						printf("%s could not read file !\n", imagepath);   
			    		return 0;               // Return False
				    }
				    data[currentbyte] = colorbuffer[2];        // Write The 'R' Byte
					data[currentbyte + 1   ] = colorbuffer[1]; // Write The 'G' Byte
					data[currentbyte + 2   ] = colorbuffer[0]; // Write The 'B' Byte
					if(tga.bytesPerPixel == 4)                  // If It's A 32bpp Image...
					{
					    data[currentbyte + 3] = colorbuffer[3];    // Write The 'A' Byte
					}
					// Increment The Byte Counter By The Number Of Bytes In A Pixel
					currentbyte += tga.bytesPerPixel;
					currentpixel++;   
				}
			}else                        // If It's An RLE Header
			{
			    chunkheader -= 127; 
			    // Read The Next Pixel
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
				{  
					printf("%s could not read file !\n", imagepath);   
			    	return 0;               // Return False
				}
				// Start The Loop
				for(short counter = 0; counter < chunkheader; counter++)
				{
				    // Copy The 'R' Byte
				    data[currentbyte] = colorbuffer[2];
				    // Copy The 'G' Byte
				    data[currentbyte + 1   ] = colorbuffer[1];
				    // Copy The 'B' Byte
				    data[currentbyte + 2   ] = colorbuffer[0];
				    if(tga.bytesPerPixel == 4)      // If It's A 32bpp Image
				    {
				        // Copy The 'A' Byte
				        data[currentbyte + 3] = colorbuffer[3];
				    }
				    currentbyte += tga.bytesPerPixel;   // Increment The Byte Counter
				    currentpixel++;      
			  	}       // Increment The Pixel Counter
			}
		}while(currentpixel < pixelcount);    // More Pixels To Read? ... Start Loop Over
    	fclose(fTGA);      
	}
	else                        // If It Doesn't Match Either One
	{
	    
		printf("%s could not read file !\n", imagepath);                     
	    return 0;               // Return False
	}          
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);


	_format = type;
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,type, w, h, 0, type, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	free(data);

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}