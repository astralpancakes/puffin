#version 330
 
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPosition;
out vec4 fragColor;
uniform sampler2D textureSampler;

uniform vec4 baseColor = vec4(0.0);

void main()
{
    // sample texture
    vec4 textureColor = texture(textureSampler, fragTexCoord);
    // mix texture with base color according to texture alpha channel
    vec4 outputColor = mix(baseColor, textureColor, textureColor.a);

    if (outputColor.a < 0.1) discard;
    fragColor = outputColor;
}