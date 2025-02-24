#version 450 core

layout(location = 0) out vec4 outColor;

in vec2 fragUV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

void main()
{
    vec3 positionData = texture(gPosition, fragUV).xyz;
    vec3 normalData   = texture(gNormal,   fragUV).xyz;
    vec3 albedoData   = texture(gAlbedo,   fragUV).rgb;

    vec3 color = albedoData * 0.8 + normalData * 0.2;
    outColor = vec4(color, 1.0);
}
