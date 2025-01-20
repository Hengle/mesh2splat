#version 460 core

layout(binding = 0) uniform sampler2D texPosition;  
layout(binding = 1) uniform sampler2D texColor;

struct GaussianVertex {
    vec4 position;
    vec4 color;
};

layout(std430, binding = 2) buffer GaussianBuffer {
    GaussianVertex vertices[];
} gaussianBuffer;

layout(std430, binding = 3) buffer DrawCommand {
    uint count;
    uint primCount;
    uint first;
    uint baseInstance;
} drawCommand;


layout(local_size_x = 16, local_size_y = 16) in;  
void main() {
    if (gl_GlobalInvocationID.x == 0 && gl_GlobalInvocationID.y == 0) {
        drawCommand.count = 0;        
        drawCommand.primCount = 1;    
        drawCommand.first = 0;        
        drawCommand.baseInstance = 0; 
    }
    memoryBarrierShared();

    uint index = atomicAdd(drawCommand.count, 1);
    //ivec2 texSize   = textureSize(texPosition, 0); TODO: use it to check bounds but should be ok anyway
    ivec2 pix       = ivec2(gl_GlobalInvocationID.xy);

    vec4 posData    = texelFetch(texPosition, pix, 0);
    vec4 colorData  = texelFetch(texColor, pix, 0);

    gaussianBuffer.vertices[index].position    = vec4(posData.xyz, 0);
    gaussianBuffer.vertices[index].color       = vec4(colorData.rgb, 1); 
}
