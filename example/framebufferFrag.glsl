#version 330

in vec2 texCoordM;
in vec2 texCoordNE;
in vec2 texCoordSE;
in vec2 texCoordSW;
in vec2 texCoordNW;

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D textureSampler;
uniform vec2 renderTargetSize;
uniform float iTime;

//TO DO: FXAA

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

vec4 red()
{
    vec4 red;
    red = vec4(1.0,0.0,0.0,1.0);
    return red;
}

vec3 chromaticAberration(sampler2D texture, vec2 fragCoord)
{
    vec2 p = (fragCoord*2.0)-1.0;
    vec3 rgb = vec3(0.0);
    rgb.r = texture2D(textureSampler, vec2(fragCoord.x, fragCoord.y)).r;
    rgb.g = texture2D(textureSampler, vec2(fragCoord.x + p.x*0.005, fragCoord.y + p.y*0.005)).g;
    rgb.b = texture2D(textureSampler, vec2(fragCoord.x + p.x*0.01, fragCoord.y + p.y*0.01)).b;
    //rgb -= vec3(pow(distance(fragCoord.xy, vec2(0.5)),5));
    //rgb -= vec3(vignette(fragCoord));
    //rgb += vec3(rand(fragCoord*vec2(time))*0.1);

    return rgb;
}

void main()
{
    vec3 fragRGB;
    vec2 fragCoord = fragTexCoord * renderTargetSize;

    fragRGB = chromaticAberration(textureSampler, fragTexCoord);    
    fragRGB -= vignette(fragTexCoord);
    //fragRGB += vec3(rand(fragTexCoord*vec2(time))*0.15);
    fragRGB += grain(fragTexCoord, iTime);

    fragColor = vec4(fragRGB, 1.0);

}