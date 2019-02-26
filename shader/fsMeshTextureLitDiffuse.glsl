#version 330
 
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPosition;
out vec4 fragColor;
uniform sampler2D textureSampler;

// basic light source
uniform vec3 lightPosition = vec3(2.0,2.0, 2.0);
uniform vec3 lightColor = vec3(1.0,1.0,1.0);
uniform float lightMultiplier = 1.0;

uniform vec4 baseColor = vec4(0.0);

void main()
{
    // do a simple diffuse light calculation on the mesh
    vec3 lightDirection = normalize(lightPosition - fragWorldPosition);
    float lightAmount = dot(normalize(fragNormal),lightDirection) * lightMultiplier;
    // sample texture
    vec4 textureColor = texture(textureSampler, fragTexCoord);
    // light texture
    vec4 litTextureColor = vec4(textureColor.rgb + (lightAmount*lightColor), textureColor.a);

    // mix lit texture with base color according to texture alpha channel
    vec4 outputColor = mix(baseColor, litTextureColor, textureColor.a);

    if (outputColor.a < 0.1) discard;
    fragColor = outputColor;
}