PUFtexture pufPaletteKorruptor()
{    
    PUFtexture texture;

    texture.textureFormat = GL_RGBA;
    texture.width = 16;
    texture.height = 16;
    
    texture.pixels = (GLfloat*)malloc(texture.width*texture.height*4*sizeof(GL_FLOAT));
    
    for (int i = 0; i < 64; i++)
    {
        float beta = pufClampf(((float)(i*4))/255.0f, 0.0f, 1.0f);

        texture.pixels[i*4] = 1.0f;
        texture.pixels[i*4+1] = beta;
        texture.pixels[i*4+2] = 1.0f - beta;
        texture.pixels[i*4+3] = 1.0f;
        
        texture.pixels[(i+64)*4] = 1.0f - beta;     
        texture.pixels[(i+64)*4+1] = 1.0f;
        texture.pixels[(i+64)*4+2] = beta;
        texture.pixels[(i+64)*4+3] = 1.0;
            
        texture.pixels[(i+128)*4+0] = 0.0f;     
        texture.pixels[(i+128)*4+1] = 1.0f - beta;
        texture.pixels[(i+128)*4+2] = 1.0f;
        texture.pixels[(i+128)*4+3] = 1.0f;
     
        texture.pixels[(i+192)*4+0] = beta;        
        texture.pixels[(i+192)*4+1] = 0.0f;  
        texture.pixels[(i+192)*4+2] = 255.0f;
        texture.pixels[(i+192)*4+3] = 1.0f;
    }

    glGenBuffers(1, &texture.pixelBuffer);

    glGenTextures(1, &texture.textureId);
    glBindTexture(GL_TEXTURE_2D, texture.textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;

}