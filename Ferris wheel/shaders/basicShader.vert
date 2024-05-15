#version 330 core

in vec3 vertex;
in vec3 color;
uniform mat4 TG;

out vec4 vertex_color;
out vec3 nuvolPos;

void main()  {
    gl_Position = TG * vec4 (vertex, 1.0);
    vertex_color = vec4 (color,1.0);
    nuvolPos = vertex;
}
