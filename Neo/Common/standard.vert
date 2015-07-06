#version 330

layout (location = 0) in vec3 Vertex;
//layout (location = 1) in vec2 Texcoord;

uniform mat4 ProjModelviewMatrix;

void main()
{
        gl_Position = ProjModelviewMatrix * vec4(Vertex, 1.0);
}
