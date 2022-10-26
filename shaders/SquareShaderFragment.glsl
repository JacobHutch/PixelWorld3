#version 330 core

in vec3 color;

out vec4 FragColor;

uniform int uTime;

void main() {
	//float offset = (uTime % 20) / 20.0f;
	//float x = (color.x + offset) - (int(color.x + offset) % 1);
	//float y = (color.y + offset) - (int(color.y + offset) % 1);
	//float z = (color.z + offset) - (int(color.z + offset) % 1);
	FragColor = vec4(color, 1.0f);
}