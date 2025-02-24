#version 450 core

layout(location = 0) out vec4 outColor;

in vec2 fragUV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

// A few simple lighting uniforms:
uniform vec3 uLightDirection;  
uniform vec3 uLightColor;      
uniform float uLightIntensity; 
uniform float uAmbientFactor;  

void main()
{
    vec3 positionData = texture(gPosition, fragUV).xyz;  // world-space pos
    vec3 normalData   = texture(gNormal,   fragUV).xyz;  // world-space normal
    vec3 albedoData   = texture(gAlbedo,   fragUV).rgb;  // base color


    vec3 N = normalize(normalData);

    vec3 L = normalize(-vec3(-1,0,0)); // If your convention is that the lightDir points *towards* the object, you can omit the minus sign.

    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = vec3(1,1,1) * 5.0f * NdotL * albedoData;

    vec3 ambient = albedoData * 0.2f;

    vec3 color = ambient + diffuse;

    outColor = vec4(color, 1.0);
}
