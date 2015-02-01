#version 330

uniform mat4 ProjModelViewMatrix;

in vec3 Vertex;
in vec2 TexCoord;

out vec2 texCoord;

//varying vec2 texCoord;
//attribute vec2 TexCoord;

void main(void)
{
    gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);
    texCoord = TexCoord;
}
