#version 330 core

in vec3 fcolor;

out vec4 FragColor;

uniform vec3 filtre;


void main() {

	FragColor = vec4(fcolor*filtre, 1);
}

