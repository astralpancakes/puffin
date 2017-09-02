#version 330

layout(location = 2) in vec2 vertexTexCoord;
layout(location = 0) in vec3 vertexPosition;
layout(location = 3) in vec3 vertexNormal;
out vec3 fragWorldPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;

uniform mat4 modelviewProjectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 modelviewMatrix;

void main()
{
	fragNormal = mat3(transpose(inverse(modelMatrix)))*vertexNormal;
    fragTexCoord = vertexTexCoord;
    fragWorldPosition = vec3(modelMatrix * vec4(vertexPosition,1.0));
    gl_Position = modelviewProjectionMatrix * vec4(vertexPosition,1.0);
}