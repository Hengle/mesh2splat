#version 450 core

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

// Lighting uniforms for a point light:
uniform vec3 u_LightPosition;
uniform mat4 u_clipToView;
uniform mat4 u_viewToWorld;

in vec2 fragUV;
out vec4 FragColor;

void main()
{
    vec3 lightColor = vec3(1,1,1);     
    float lightIntensity = 1.0f; 
    float ambientFactor = 0.2f; 

    // Fetch data from the G-buffer:
    vec3 positionData = texture(gPosition, fragUV).xyz;  // world-space position

    vec4 ndcPos = vec4(positionData, 1.0);
    vec4 viewPos = u_clipToView * ndcPos;
    viewPos /= viewPos.w;
    vec3 worldPos = (u_viewToWorld * viewPos).xyz;

    vec3 normalData   = texture(gNormal,   fragUV).xyz;  // world-space normal
    vec3 albedoData   = texture(gAlbedo,   fragUV).rgb;  // base color

    vec3 N = normalize(normalData);

    vec3 L = normalize(u_LightPosition - worldPos);

    float distance = length(u_LightPosition - worldPos);
    float attenuation = 1.0 / (distance * distance); // simple inverse-square law

    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = lightColor * lightIntensity * attenuation * NdotL * albedoData;

    vec3 ambient = albedoData * ambientFactor;

    vec3 color = ambient + diffuse;

    FragColor = vec4(color, 1.0);
}
