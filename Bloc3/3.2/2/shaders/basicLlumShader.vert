#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

out vec4 vertexSCO;
out vec3 normalSCO;


out vec3 matambFS;
out vec3 matdifFS;
out vec3 matspecFS;
out float matshinFS;


void main() {	

  // PosVertex en SCO
    vertexSCO = view * TG * vec4(vertex, 1.0);

  // Normal en SCO
    mat3 normalMatrix = inverse (transpose(mat3(view*TG)));
    normalSCO = normalize(normalMatrix*normal);

    matambFS = matamb;
    matdifFS = matdiff;
    matspecFS = matspec; 
    matshinFS = matshin;

    gl_Position = proj * vertexSCO;
}
