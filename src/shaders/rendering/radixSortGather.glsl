#version 460

layout(std430, binding = 0) buffer GaussiansIn  { vec4 in_data[];  };
layout(std430, binding = 1) buffer GaussiansOut { vec4 out_data[]; };
layout(std430, binding = 2) buffer Values       { uint val[]; };

layout(std430, binding = 3) writeonly buffer DrawCommand {
    uint count;
    uint instanceCount;
    uint first;
    uint baseInstance;
} drawElementsIndirectCommand;

uniform uint u_count;

layout(local_size_x = 256) in;
void main() {
    uint gid = gl_GlobalInvocationID.x;

    if (gid >= u_count) return;

    uint old_index = val[gid]; 
    //I am basically doing a buffer swap based on the values (indices) computed during the radxi sort pass
    for (int j=0; j<6; j++) {
        out_data[gid*6 + j] = in_data[old_index*6 + j];
    }

    if (gid == 0) {
        drawElementsIndirectCommand.count        = 6;
        drawElementsIndirectCommand.instanceCount = u_count;
        drawElementsIndirectCommand.first        = 0;
        drawElementsIndirectCommand.baseInstance = 0;
    }
}
