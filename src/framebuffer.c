void pufFramebufferInit(PUFframebuffer* framebuffer)
{
    glGenFramebuffers(1, &framebuffer->framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebufferId);
    glGenRenderbuffers(1, &framebuffer->depthbufferId);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebuffer->textureIsAttached = 0;
}

void pufFramebufferTexture(PUFframebuffer* framebuffer, PUFtexture* texture)
{

    // We only support one framebuffer texture for now.
    // In the future we might want more, we then need a way to specify the attachment point and to keep track of the smallest texture (which is the renderable area for the entire thing, apparently).

    if (framebuffer->textureIsAttached == 0)
    {
        framebuffer->textureIsAttached = 1;

        framebuffer->width = texture->width;
        framebuffer->height = texture->height;

        //texture stuff
        glBindTexture(GL_TEXTURE_2D, texture->textureId);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texture->width,texture->height,0,texture->textureFormat,GL_FLOAT,0); 
        
        //framebuffer stuff
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebufferId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->textureId, 0);
        
        //depth buffer stuff -- this is a renderbuffer because that's faster, but could also be a texture if we want it in the future
        glBindRenderbuffer(GL_RENDERBUFFER,framebuffer->depthbufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture->width, texture->height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthbufferId);

        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Alert!");
        
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void pufFramebufferBindAndClear(PUFframebuffer* framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebufferId);
    glBindRenderbuffer(GL_RENDERBUFFER,framebuffer->depthbufferId);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
}

void pufFramebufferUnbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
