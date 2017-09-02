#version 330

layout(location = 2) in vec2 vertexTexCoord;
layout(location = 0) in vec3 vertexPosition;
out vec2 fragTexCoord;

uniform vec2 renderTargetSize;

// computed texture coordinates used for FXAA in fragment shader, calculating them here avoids dependent texture reads (texture reads based on calculations in that same shader)
out vec2 texCoordM;
out vec2 texCoordNE;
out vec2 texCoordSE;
out vec2 texCoordSW;
out vec2 texCoordNW;

void main()
{
    vec2 fragCoord = vertexTexCoord * renderTargetSize;
    vec2 invRenderTargetSize = 1.0 / renderTargetSize.xy;

    texCoordM = vec2(fragCoord * invRenderTargetSize);
    texCoordNE = (fragCoord + vec2(1.0,-1.0)) * invRenderTargetSize;
    texCoordSE = (fragCoord + vec2(1.0,1.0)) * invRenderTargetSize;
    texCoordSW = (fragCoord + vec2(-1.0,1.0)) * invRenderTargetSize;
    texCoordNW = (fragCoord + vec2(-1.0,-1.0)) * invRenderTargetSize;

	fragTexCoord = vertexTexCoord;
    gl_Position = vec4(vertexPosition,1.0);
}