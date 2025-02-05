#version 460 core

layout(std430, binding = 0) buffer GaussiansIn {
    float depths[];
};

layout(std430, binding = 1) buffer Keys {
    uint key[];
};

layout(std430, binding = 2) buffer Values {
    uint val[];
};

uniform uint  u_count; 

layout(local_size_x = 256) in;
void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= u_count) return;
    uint bits = floatBitsToUint(depths[i]);

    key[i] = bits; 
    val[i] = i;
}
