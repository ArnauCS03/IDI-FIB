#version 330 core

in vec3 vertex;
in vec4 color;
out vec4 fcolor;

void main()  {
    fcolor = color;
    gl_Position = vec4 (vertex*0.5, 1.0);
}
