#version 460 core

in vec3 out_pos;

uniform float u_near; 
uniform float u_far;  

uniform vec3 u_lightPos;


void main() {
    gl_FragDepth = length(out_pos - u_lightPos) / 100;
}