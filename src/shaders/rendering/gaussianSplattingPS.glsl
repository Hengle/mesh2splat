#version 460 core


in vec3 out_color;
in float out_opacity;
in vec2 out_screen;
in vec3 out_conic;

uniform vec2 u_resolution;

out vec4 FragColor;

void main() {	
    vec2 d = (out_screen - gl_FragCoord.xy);
    //float alpha = -0.5 * (d.x * d.x * out_conic.x + d.y * d.y * out_conic.z) - d.x * d.y * out_conic.y;
    float alpha = dot(out_conic.xzy, vec3(d*d, d.x*d.y));
    float g = exp(alpha);

    //FragColor = vec4(out_color, out_opacity) * g;
    FragColor = vec4(out_color, out_opacity) * g;
}