#version 330
 
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPosition;
out vec4 fragColor;
uniform sampler2D textureSampler;

// basic light source
vec3 lightPosition = vec3(0.0,5.0, 1.0);
vec3 lightColor = vec3(.9,0.6,0.0);
float lightMultiplier = 0.2;

void main()
{
    // do a simple diffuse light calculation on the mesh
    vec3 lightDirection = normalize(lightPosition - fragWorldPosition);
    float lightAmount = dot(normalize(fragNormal),lightDirection) * lightMultiplier;
    // sample texture
    vec4 textureColor = texture(textureSampler, fragTexCoord);
    // light texture
    vec3 litTextureColor = textureColor.rgb + (vec3(lightAmount)*lightColor);
    // color mesh white and alpha-over the lit texture map on top
    fragColor = vec4(mix(vec3(1.0), litTextureColor, textureColor.a), 1.0);
}