#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	TexCoords = aTexCoords;
	vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
	vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = proj * view * vec4(FragPos + Normal * 0.004, 1.0f);
}
