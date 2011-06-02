#include <SDL/SDL.h>
#ifdef PUFFIN_GLEWDOTFRAMEWORK
#include <GLEW/glew.h>
#else
#include <GL/glew.h>
#endif
#include "puffin.h"
#include "loadobj.h"
#include "matrix.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>


//globals

static struct {
	unsigned int windowHeight;
	unsigned int windowWidth;
    unsigned int frameRate;
    SDL_Surface* windowSurface;
	int glMajorVersion;
	int glMinorVersion;
    Uint32 frameCounter;
    unsigned long frameTick; 
    unsigned long frameDelay;
} puffin;



void pufInit(int windowWidth, int windowHeight, int framerate)
{
	if (windowHeight == 0)	// prevent divide by zero etc
        windowHeight = 1;
    puffin.windowHeight = windowHeight;
    puffin.windowWidth = windowWidth;
	SDL_Init(SDL_INIT_EVERYTHING);
    puffin.windowSurface = SDL_SetVideoMode(puffin.windowWidth,puffin.windowHeight,24,SDL_OPENGL|SDL_GL_DOUBLEBUFFER|SDL_RESIZABLE);
	puffin.frameRate = framerate;
    glewInit();

	//get OpenGL vPUFglobals
	puffin.glMajorVersion = atoi(&glGetString(GL_VERSION)[0]);
	puffin.glMajorVersion = atoi(&glGetString(GL_VERSION)[2]); 
    puffin.frameCounter = 0;
    puffin.frameDelay = 0;    
    puffin.frameTick = 0;
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
   
    
}

void pufWindowResize(int windowWidth, int windowHeight)
{
    puffin.windowWidth = windowWidth;
    puffin.windowHeight = windowHeight;
    puffin.windowSurface = SDL_SetVideoMode(puffin.windowWidth,puffin.windowHeight,24,SDL_OPENGL|SDL_GL_DOUBLEBUFFER|SDL_RESIZABLE);
}

void pufUpdate()
{
    SDL_GL_SwapBuffers();
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
    if (puffin.frameRate > 0)
    {
        if (puffin.frameTick == 0)
        {
            puffin.frameTick = SDL_GetTicks();
        }
    

        int delay = puffin.frameTick + (1000/puffin.frameRate) - SDL_GetTicks();
    
        if(delay > 0) 
            SDL_Delay(delay);
        puffin.frameTick = SDL_GetTicks();
    }
    ++puffin.frameCounter;
    
     
}

float pufGetStats(int type)
{
    float returnValue;
    switch (type)
    {
        case 0:
            returnValue = (float)SDL_GetTicks()/1000;
            break;
        case 1:
            returnValue = (puffin.frameCounter*1000.0f)/(SDL_GetTicks());
            break;
    }
    
    return returnValue;
}

void pufCameraInit(PUFcamera* camera, float fov, float nearClip, float farClip)
{
    camera->width = puffin.windowWidth;
    camera->height = puffin.windowHeight;
    glViewport(0,0, camera->width,camera->height);
	glShadeModel(GL_SMOOTH);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    pufMatrixProject(fov, camera->width, camera->height, nearClip, farClip,true, camera->projectionMatrix);
    
    for (int i=0;i < 15; i++)
		camera->cameraMatrix[i] = 0.0f;
    
    camera->cameraMatrix[0] = 1.0f;
    camera->cameraMatrix[5] = 1.0f;
    camera->cameraMatrix[10] = 1.0f;
    camera->cameraMatrix[15] = 1.0f;
}

void pufCameraTranslate(PUFcamera* camera, float X, float Y, float Z)
{
	GLfloat tempCameraMatrix[16];
	GLfloat tempMatrix[16];
	pufMatrixMult(memcpy(tempCameraMatrix,camera->cameraMatrix,sizeof(GLfloat)*16),pufMatrixTranslate(-X,-Y,-Z,tempMatrix),camera->cameraMatrix);
}

void pufCameraRotate(PUFcamera* camera, float ang, float X, float Y, float Z, bool degrees)
{
    GLfloat tempCameraMatrix[16];
	GLfloat tempMatrix[16];
    pufMatrixMult(memcpy(tempCameraMatrix,camera->cameraMatrix,sizeof(GLfloat)*16),pufMatrixRotate(ang,-X,-Y,-Z,degrees,tempMatrix),camera->cameraMatrix);
}

void pufMeshInit(PUFmesh* mesh) //zero the modelview matrix of the mesh
{
    for (int i=0;i < 15; i++)
		mesh->modelView[i] = 0.0f;
	
    mesh->modelView[0] = 1.0f;
    mesh->modelView[5] = 1.0f;
    mesh->modelView[10] = 1.0f;
    mesh->modelView[15] = 1.0f;
}

void pufMeshShapeQuad(PUFmesh* mesh) //generates a nicely generic quad
{
	mesh->vertexCount = 6;
	mesh->verts = malloc(sizeof(PUFvertex)*mesh->vertexCount);
	mesh->verts[0].position[0] = -1.0f;
	mesh->verts[0].position[1] = 1.0f;
	mesh->verts[0].position[2] = 0.0f;
	mesh->verts[0].texture[0] = 0.0f;
	mesh->verts[0].texture[1] = 1.0f;
	mesh->verts[0].texture[2] = 0.0f;
	
	mesh->verts[1].position[0] = -1.0f;
	mesh->verts[1].position[1] = -1.0f;
	mesh->verts[1].position[2] = 0.0f;
	mesh->verts[1].texture[0] = 0.0f;
	mesh->verts[1].texture[1] = 0.0f;
	mesh->verts[1].texture[2] = 0.0f;
	
	mesh->verts[2].position[0] = 1.0f;
	mesh->verts[2].position[1] = -1.0f;
	mesh->verts[2].position[2] = 0.0f;
	mesh->verts[2].texture[0] = 1.0f;
	mesh->verts[2].texture[1] = 0.0f;
	mesh->verts[2].texture[2] = 0.0f;
	
	mesh->verts[3].position[0] = -1.0f;
	mesh->verts[3].position[1] = 1.0f;
	mesh->verts[3].position[2] = 0.0f;
	mesh->verts[3].texture[0] = 0.0f;
	mesh->verts[3].texture[1] = 1.0f;
	mesh->verts[3].texture[2] = 0.0f;
	
	mesh->verts[4].position[0] = 1.0f;
	mesh->verts[4].position[1] = -1.0f;
	mesh->verts[4].position[2] = 0.0f;
	mesh->verts[4].texture[0] = 1.0f;
	mesh->verts[4].texture[1] = 0.0f;
	mesh->verts[4].texture[2] = 0.0f;
	
	mesh->verts[5].position[0] = 1.0f;
	mesh->verts[5].position[1] = 1.0f;
	mesh->verts[5].position[2] = 0.0f;
	mesh->verts[5].texture[0] = 1.0f;
	mesh->verts[5].texture[1] = 1.0f;
	mesh->verts[5].texture[2] = 0.0f;
	
	for (int i=0;i<mesh->vertexCount;++i)
	{
		mesh->verts[i].color[0] = ((float)(rand() % 100))/100;
        mesh->verts[i].color[1] = (float)(rand() % 100)/100;
        mesh->verts[i].color[2] = (float)(rand() % 100)/100;
        mesh->verts[i].normal[0] = 0.0f;
        mesh->verts[i].normal[1] = 0.0f;
        mesh->verts[i].normal[2] = 1.0f;
	}
}

void pufMeshLoadOBJ(PUFmesh* mesh, char const* file) //loads an OBJ file into Puffin mesh
{
    GLfloat* obj = pufLoadOBJ(file, &mesh->vertexCount);
    mesh->verts = malloc(sizeof(PUFvertex)*mesh->vertexCount);

    for (int i=0;i<mesh->vertexCount;++i)
    {
        mesh->verts[i].position[0] = obj[i*9+0];
        mesh->verts[i].position[1] = obj[i*9+1];
        mesh->verts[i].position[2] = obj[i*9+2];
        mesh->verts[i].color[0] = ((float)(rand() % 100))/100;
        mesh->verts[i].color[1] = (float)(rand() % 100)/100;
        mesh->verts[i].color[2] = (float)(rand() % 100)/100;
        mesh->verts[i].texture[0] = obj[i*9+3];
        mesh->verts[i].texture[1] = obj[i*9+4];
        mesh->verts[i].texture[2] = obj[i*9+5];
        mesh->verts[i].normal[0] = obj[i*9+6];
        mesh->verts[i].normal[1] = obj[i*9+7];
        mesh->verts[i].normal[2] = obj[i*9+8];
    }
    free((void*)obj);
}

void pufMeshBind(PUFmesh* mesh) //uploads Puffin mesh vertex buffer object
{
		glGenBuffers(1, &mesh->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PUFvertex)*mesh->vertexCount, NULL,GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PUFvertex)*mesh->vertexCount, mesh->verts);	
}

void pufMeshDraw(PUFmesh* mesh, PUFcamera* camera, PUFshader* shader) //draws Puffin mesh
{
	
		glUseProgram(shader->shaderProgram);
		GLint uniformModelviewProjection = glGetUniformLocation(shader->shaderProgram, "modelviewProjection");
		GLfloat modelviewProjectionMatrix[16];
        GLfloat modelviewCameraMatrix[16];
        
        pufMatrixMult(camera->cameraMatrix, mesh->modelView, modelviewCameraMatrix);
		pufMatrixMult(camera->projectionMatrix, modelviewCameraMatrix, modelviewProjectionMatrix);
		glUniformMatrix4fv(uniformModelviewProjection, 1, GL_FALSE, modelviewProjectionMatrix);
	
		GLint uniformNormalMatrix = glGetUniformLocation(shader->shaderProgram, "modelviewMatrix");
		glUniformMatrix4fv(uniformNormalMatrix, 1, GL_FALSE, mesh->modelView);
	
		GLint vertexPosition = glGetAttribLocation(shader->shaderProgram, "vertexPosition");
		GLint vertexColor = glGetAttribLocation(shader->shaderProgram, "vertexColor");
		GLint vertexTexture = glGetAttribLocation(shader->shaderProgram, "vertexTexture");
		GLint vertexNormal = glGetAttribLocation(shader->shaderProgram, "vertexNormal");
		GLint vertexMeta3f = glGetAttribLocation(shader->shaderProgram, "inMeta3f");
		GLint vertexMeta1f = glGetAttribLocation(shader->shaderProgram, "inMeta1f");
	
		glEnableVertexAttribArray(vertexPosition);
		glEnableVertexAttribArray(vertexColor);
		glEnableVertexAttribArray(vertexTexture);
		glEnableVertexAttribArray(vertexNormal);
		glEnableVertexAttribArray(vertexMeta3f);
		glEnableVertexAttribArray(vertexMeta1f);
	
	
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	
	
		glVertexAttribPointer(vertexPosition, 3, GL_FLOAT, 1, sizeof(PUFvertex), (void*)0);
		glVertexAttribPointer(vertexColor, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*3));
		glVertexAttribPointer(vertexTexture, 2, GL_FLOAT, 0 , sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*6));
		glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*9));
		glVertexAttribPointer(vertexMeta3f, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*12));
		glVertexAttribPointer(vertexMeta1f, 1, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*15));
	
		glDrawArrays(GL_TRIANGLES,0,mesh->vertexCount);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		glDisableVertexAttribArray(vertexMeta1f);
		glDisableVertexAttribArray(vertexMeta3f);
		glDisableVertexAttribArray(vertexNormal);
		glDisableVertexAttribArray(vertexTexture);
		glDisableVertexAttribArray(vertexColor);
		glDisableVertexAttribArray(vertexPosition);
		glUseProgram(0);
	

}

void pufMeshTranslate(PUFmesh* mesh, float X, float Y, float Z) //translates Puffin mesh
{
	//copy the existing mesh modelview matrix into tempModelView, ask for a new matrix for tempMatrix, then multiply them and put into mesh modelview
	GLfloat tempModelView[16];
	GLfloat tempMatrix[16];
	pufMatrixMult(memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),pufMatrixTranslate(X,Y,Z,tempMatrix),mesh->modelView);
}

void pufMeshRotate(PUFmesh* mesh, float ang, float X, float Y, float Z, bool degrees) //rotates Puffin mesh
{
    GLfloat tempModelView[16];
	GLfloat tempMatrix[16];
    pufMatrixMult(memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),pufMatrixRotate(ang,X,Y,Z,degrees,tempMatrix),mesh->modelView);
}

void pufMeshScale(PUFmesh* mesh, float X, float Y, float Z) //scales Puffin mesh
{
	GLfloat tempModelView[16];
	GLfloat tempMatrix[16];
    pufMatrixMult(memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),pufMatrixScale(X,Y,Z,tempMatrix),mesh->modelView);
}

void pufMeshDestroy(PUFmesh* mesh) //deletes Puffin mesh
{
    free((void*)mesh->verts);
}

void pufShaderLoad(PUFshader* shader, char const* vertexShaderSourceFile, char const* fragmentShaderSourceFile) //loads shader from GLSL source files, compiles and links shader program
{

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* vertexShaderSource = pufReadFile(vertexShaderSourceFile);
		glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
		glCompileShader(vertexShader);
		free((void*)vertexShaderSource);
		
		
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		
		const GLchar* fragmentShaderSource = pufReadFile(fragmentShaderSourceFile);
		glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
		glCompileShader(fragmentShader);
		free((void*)fragmentShaderSource);
		
		shader->shaderProgram = glCreateProgram();
		glAttachShader(shader->shaderProgram,vertexShader);
		glAttachShader(shader->shaderProgram,fragmentShader);
		glLinkProgram(shader->shaderProgram);
	

} 

void pufTextureLoadBMP(PUFtexture* texture, char const* file) //loads BMP file into Puffin texture, binds the pixel buffer object and loads into OpenGL texture map
{
    glGenTextures(1, &texture->textureId);
    
    
    FILE* img = NULL;
    if ((img = fopen(file,"r")))
    {
        int pixelDataStartingOffset;
        int pixelBits;
        fseek(img,10,SEEK_SET);
        fread(&pixelDataStartingOffset,4,1,img);
        fseek(img,4,SEEK_CUR);
        fread(&texture->width,4,1,img);
        fread(&texture->height,4,1,img);
        fseek(img,2,SEEK_CUR);
        fread(&pixelBits,2,1,img);
        texture->pixelBytes = pixelBits/8;

#ifdef PUFFIN_SQUAWK
        printf("Puffin BMP loader opened file %s\n",name);
        printf("Pixel data start offset is %i...\n", pixelDataStartingOffset);
        printf("Width is %lu...\n", texture->width);
        printf("Height is %lu...\n", texture->height);
        printf("Number of bits per pixel is %i...\n", pixelBits);
        printf("Number of bytes per pixel is %i...\n", texture->pixelBytes);
        printf("Allocating memory for %lu pixels of %i bytes each...\n",texture->height*texture->width, texture->pixelBytes);
#endif
        
        texture->pixels = (GLchar*)malloc(texture->height*texture->width*texture->pixelBytes);
        
        fseek(img,pixelDataStartingOffset,SEEK_SET);	// start reading image data
        for(int i = 0;i<texture->height;i++) // for each row...
        {
            for(int j = 0;j<texture->width;j++) // for each pixel...
                fread(texture->pixels+(i*texture->width*texture->pixelBytes+j*texture->pixelBytes),texture->pixelBytes,1,img); // read it...
            if ((texture->width*texture->pixelBytes) % 4 != 0)  // BMP rows are stored on four byte alignments, so hop ahead after each row if needed
                fseek(img,4 - ((texture->width*texture->pixelBytes) % 4), SEEK_CUR);
        }
        fclose(img);
    }
    else // failed to open file, let's make the texture solid white instead
    {
        printf("Puffin BMP loader failed to open file %s\n",file);
        texture->width = 1;
        texture->height = 1;
        texture->pixelBytes = 4;
        texture->pixels = (GLchar*)malloc(texture->height*texture->width*texture->pixelBytes);
        for (int i = 0;i<texture->pixelBytes;i++)
            texture->pixels[i] = 255;
    }

    if (texture->pixelBytes == 4)  // this will be wrong on big-endian, but whatever...
        texture->textureFormat = GL_BGRA;
    else if (texture->pixelBytes == 3)
        texture->textureFormat = GL_BGR;
    else
        printf("Puffin BMP loader found that %s does not use 32 or 24 bits per pixel. This wasn't expected.", file);
    
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);    
    glGenBuffers(1, &texture->pixelBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->pixelBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, texture->width*texture->height*texture->pixelBytes, NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, texture->width*texture->height*texture->pixelBytes, texture->pixels);
        
    

    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,texture->pixelBytes,texture->width,texture->height,0,texture->textureFormat,GL_UNSIGNED_BYTE,NULL);
}

void pufTextureCreateRGBA(PUFtexture* texture, GLulong width, GLulong height) //creates blank Puffin texture of specified size
{
    texture->pixelBytes = 4;
    texture->textureFormat = GL_RGBA;
    texture->width = width;
    texture->height = height;
    
    texture->pixels = (GLchar*)malloc(texture->width*texture->height*texture->pixelBytes);
    
    glGenBuffers(1, &texture->pixelBuffer);

    glGenTextures(1, &texture->textureId);
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void pufTextureCreateRGB(PUFtexture* texture, GLulong width, GLulong height)
{
    texture->pixelBytes = 3;
    texture->textureFormat = GL_RGB;
    texture->width = width;
    texture->height = height;
    
    texture->pixels = (GLchar*)malloc(texture->width*texture->height*texture->pixelBytes);
    
    glGenBuffers(1, &texture->pixelBuffer);
    
    glGenTextures(1, &texture->textureId);
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void pufTextureUpdate(PUFtexture* texture) //binds pixel buffer object of Puffin texture, and updates OpenGL texture map 
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->pixelBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, texture->width*texture->height*texture->pixelBytes, NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, texture->width*texture->height*texture->pixelBytes, texture->pixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glTexImage2D(GL_TEXTURE_2D,0,texture->pixelBytes,texture->width,texture->height,0,texture->textureFormat,GL_UNSIGNED_BYTE,NULL);
}

void pufTextureBind(PUFtexture* texture) //binds Puffin texture for use
{
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
}

void pufTextureDestroy(PUFtexture* texture) //deletes Puffin texture
{
    free((void*)texture->pixels);
}

void pufTextureOldLoadBMP(PUFtexture* texture, char const* name)
{

	SDL_Surface* surface;
surface = SDL_LoadBMP(name);

#ifdef PUFFIN_SQUAWK
	printf("Loading image %s...\n", name);
	if ( (surface->w & (surface->w - 1)) != 0 ) {
		printf("Warning: Width of %s is not a power of 2\n",name);
	}


	if ( (surface->h & (surface->h - 1)) != 0 ) {
		printf("Warning: Height of %s is not a power of 2\n",name);
	}
#endif
	
	texture->pixelBytes = surface->format->BytesPerPixel; // get the number of channels in the SDL surface
	if (texture->pixelBytes == 4)     // contains an alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
			texture->textureFormat = GL_RGBA;
		else
			texture->textureFormat = GL_BGRA;
	} 
	else if (texture->pixelBytes == 3)     // no alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
			texture->textureFormat = GL_RGB;
		else
			texture->textureFormat = GL_BGR;
	} 
	else 
	{
		printf("warning: %s is not truecolor\n",name);
	}


	glGenBuffers(1, &texture->pixelBuffer);
	
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->pixelBuffer);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, surface->w*surface->h*surface->format->BytesPerPixel, NULL,GL_STATIC_DRAW);
	glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, surface->w*surface->h*surface->format->BytesPerPixel, surface->pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,texture->pixelBytes,surface->w,surface->h,0,texture->textureFormat,GL_UNSIGNED_BYTE,NULL);
	if(surface)
		SDL_FreeSurface(surface);
}
