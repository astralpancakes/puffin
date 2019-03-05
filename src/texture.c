//
//  texture.c
//  Puffin
//
//  Created by Peter Schulman on 1/22/13.
//
//

PUFtexture pufTextureLoadBMP(char const* file) //loads BMP file into Puffin texture, binds the pixel buffer object and loads into OpenGL texture map
{
    PUFtexture texture;

    glGenTextures(1, &texture.textureId);
    
    FILE* img = NULL;
    if ((img = fopen(file,"rb")))
    {
        int pixelDataStartingOffset;
        GLshort pixelBits;
        fseek(img,10,SEEK_SET);
        fread(&pixelDataStartingOffset,4,1,img);
        fseek(img,4,SEEK_CUR);
        fread(&texture.width,4,1,img);
        fread(&texture.height,4,1,img);
        fseek(img,2,SEEK_CUR);
        fread(&pixelBits,2,1,img);
        GLint pixelBytes = pixelBits/8;
        
        if ((pixelBytes != 4) & (pixelBytes != 3))
        {
            printf("Puffin BMP loader found that %s does not use 32 or 24 bits per pixel. This wasn't expected.\n", file);
            printf("Bytes per pixel: %i\n",pixelBytes);
        }

        GLubyte *tempBuffer = (GLubyte*)malloc(texture.height*texture.width*pixelBytes*sizeof(GL_UNSIGNED_BYTE));
        texture.pixels = (GLfloat*)malloc(texture.height*texture.width*4*sizeof(GL_FLOAT));
        
        fseek(img,pixelDataStartingOffset,SEEK_SET);	// start reading image data
        
        for(int i = 0; i<texture.height; i++) // for each row...
        {
            for(int j = 0; j<texture.width; j++) // for each pixel...
                fread(tempBuffer+(i*texture.width*pixelBytes+j*pixelBytes),pixelBytes,1,img); // read it...
            if ((texture.width*pixelBytes) % 4 != 0)  // BMP rows are stored on four byte alignments, so hop ahead after each row if needed
                fseek(img,4 - ((texture.width*pixelBytes) % 4), SEEK_CUR);
        }
        
        fclose(img);
        
        for (int i=0; i < texture.width*texture.height; i++)
        {
        /* BMP files are stored with the pixel data as ABGR (32 bits per pixel) or BGR (24 bits per pixel). 
        We want puffin textures to be RGBA, so we do some twiddling when we copy the pixel data.
        
        If you want to play around with this, note that Photoshop does not seem to save BMP files with alpha channels properly. Use The GIMP or something...*/

            if(pixelBytes == 4)
            {
                texture.pixels[i*4] = (GLfloat)tempBuffer[i*pixelBytes+3]/255.0f;
                texture.pixels[i*4+1] = (GLfloat)tempBuffer[i*pixelBytes+2]/255.0f;
                texture.pixels[i*4+2] = (GLfloat)tempBuffer[i*pixelBytes+1]/255.0f;
                texture.pixels[i*4+3] = (GLfloat)tempBuffer[i*pixelBytes]/255.0f;
            }
			if(pixelBytes == 3)
            {
                texture.pixels[i*4] = (GLfloat)tempBuffer[i*pixelBytes+2]/255.0f;
                texture.pixels[i*4+1] = (GLfloat)tempBuffer[i*pixelBytes+1]/255.0f;
                texture.pixels[i*4+2] = (GLfloat)tempBuffer[i*pixelBytes]/255.0f;
                texture.pixels[i*4+3] = 1.0f;
            }
        }
        free((void*)tempBuffer);
        
    }
    else /* failed to open file, let's make the texture solid yellow instead */
    {
        printf("Puffin BMP loader failed to open file %s\n",file);
        texture.width = 1;
        texture.height = 1;
        texture.pixels = (GLfloat*)malloc(texture.height*texture.width*4*sizeof(GL_FLOAT));
        texture.pixels[0] = 0.0f;
		for (int i = 1; i<4; i++)
            texture.pixels[i] = 1.0f;
    }
    
     texture.textureFormat = GL_RGBA;

    glGenBuffers(1, &texture.pixelBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture.pixelBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, texture.width*texture.height*4*sizeof(GL_FLOAT), NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, texture.width*texture.height*4*sizeof(GL_FLOAT), texture.pixels);

    glBindTexture(GL_TEXTURE_2D, texture.textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texture.width,texture.height,0,texture.textureFormat,GL_FLOAT,NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    return texture;
}

PUFtexture pufTextureCreate(uint32_t width, uint32_t height) //creates blank Puffin texture of specified size
{	
    PUFtexture texture;

    texture.textureFormat = GL_RGBA;
    texture.width = width;
    texture.height = height;
    
    texture.pixels = (GLfloat*)malloc(texture.width*texture.height*4*sizeof(GL_FLOAT));
    
    glGenBuffers(1, &texture.pixelBuffer);

    glGenTextures(1, &texture.textureId);
    glBindTexture(GL_TEXTURE_2D, texture.textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void pufTexturePixelSet(PUFtexture* texture, GLuint x, GLuint y, PUFcolor* color)
{
    if (color->R < 0.0f)
        color->R = 0.0f;
    if (color->G < 0.0f)
        color->G = 0.0f;
    if (color->B < 0.0f)
        color->B = 0.0f;
    if (color->A < 0.0f)
        color->A = 0.0f;
    if (texture->textureFormat == GL_BGRA)
    {
        texture->pixels[y*texture->width*4+x*4] = color->B;
        texture->pixels[y*texture->width*4+x*4+1] = color->G;
        texture->pixels[y*texture->width*4+x*4+2] = color->R;
        texture->pixels[y*texture->width*4+x*4+3] = color->A;
    }
    else
    {
        texture->pixels[y*texture->width*4+x*4] = color->R;
        texture->pixels[y*texture->width*4+x*4+1] = color->G;
        texture->pixels[y*texture->width*4+x*4+2] = color->B;
        texture->pixels[y*texture->width*4+x*4+3] = color->A;
    }
}

PUFcolor pufTexturePixelGet(PUFtexture* texture, GLuint x, GLuint y) 
{
    PUFcolor color;
    if (texture->textureFormat == GL_BGRA)
    {
        color.B = texture->pixels[y*texture->width*4+x*4];
        color.G = texture->pixels[y*texture->width*4+x*4+1];
        color.R = texture->pixels[y*texture->width*4+x*4+2];
        color.A =texture->pixels[y*texture->width*4+x*4+3];
    }
    else
    {
        color.R = texture->pixels[y*texture->width*4+x*4];
        color.G = texture->pixels[y*texture->width*4+x*4+1];
        color.B = texture->pixels[y*texture->width*4+x*4+2];
        color.A = texture->pixels[y*texture->width*4+x*4+3];
    }
    if (color.R < 0.0f)
        color.R = 0.0f;
    if (color.G < 0.0f)
        color.G = 0.0f;
    if (color.B < 0.0f)
        color.B = 0.0f;
    if (color.A < 0.0f)
        color.A = 0.0f;
    return color;
}

/* clears a Puffin texture */
void pufTextureClear(PUFtexture* texture)
{
    memset(texture->pixels, 0, texture->width*texture->height*4*sizeof(GL_FLOAT));
}

/* binds pixel buffer object of Puffin texture, and updates OpenGL texture map */
void pufTextureUpdate(PUFtexture* texture) 
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->pixelBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, texture->width*texture->height*4*sizeof(GL_FLOAT), NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, texture->width*texture->height*4*sizeof(GL_FLOAT), texture->pixels);

    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texture->width,texture->height,0,texture->textureFormat,GL_FLOAT,NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

/* binds Puffin texture for use */
void pufTextureBind(PUFtexture* texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
}

/* deletes Puffin texture */
void pufTextureDestroy(PUFtexture* texture)
{
    free((void*)texture->pixels);
}
