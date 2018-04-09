#version 330

// TO DO: FXAA
//in vec2 texCoordM;
//in vec2 texCoordNE;
//in vec2 texCoordSE;
//in vec2 texCoordSW;
//in vec2 texCoordNW;

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D textureSampler;
uniform float iTime;


float pseudoRand(vec2 seed)
{
   return fract(sin(dot(seed.xy,vec2(420.5432,25.223))) * 937212.23512);
}

vec3 grain(vec2 fragTexCoord, float iTime)
{
    #define GRAIN_AMOUNT 0.15
    vec3 grain;
    grain = vec3(pseudoRand(fragTexCoord*vec2(iTime))*GRAIN_AMOUNT);
    return grain;
}

vec3 vignette(vec2 fragTexCoord)
{
    #define VIGNETTE_SHARPNESS 5.0
    #define VIGNETTE_STRENGTH 2.0

    return vec3(pow(distance(fragTexCoord.xy, vec2(0.5)),VIGNETTE_SHARPNESS) * VIGNETTE_STRENGTH);
}



vec3 chromaticAberration(sampler2D textureSampler, vec2 fragTexCoord)
{
    vec2 p = (fragTexCoord*2.0)-1.0;
    vec3 rgb;
    rgb.r = texture(textureSampler, vec2(fragTexCoord.x, fragTexCoord.y)).r;
    rgb.g = texture(textureSampler, vec2(fragTexCoord.x + p.x*0.005, fragTexCoord.y + p.y*0.005)).g;
    rgb.b = texture(textureSampler, vec2(fragTexCoord.x + p.x*0.01, fragTexCoord.y + p.y*0.01)).b;

    return rgb;
}

void main()
{
    vec3 fragRGB;
	
    fragRGB = chromaticAberration(textureSampler, fragTexCoord);    
    fragRGB -= vignette(fragTexCoord);
    fragRGB += grain(fragTexCoord, iTime);
    fragColor = vec4(fragRGB, 1.0);
}