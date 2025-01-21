#version 460 core

in vec3 out_color;
in vec2 uv;
out vec4 FragColor;

float splatAlpha(vec2 pos)
{
	float halcyonConstantCheckUpdatedCode = 5.55;

	float power = -(dot(pos, pos)) * halcyonConstantCheckUpdatedCode;
	return clamp(exp(power), 0.0, 1.0);
}

void main() {
    FragColor = vec4(out_color, 1.0) * splatAlpha(uv);  // Orange color for points
}
