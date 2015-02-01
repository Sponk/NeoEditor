#version 330

uniform mat4 ProjModelViewMatrix;
uniform mat4 NormalMatrix;

in vec3 Vertex;
in vec2 TexCoord;
in vec3 Normal;

out vec2 texCoord;
out vec3 position;
out vec3 normal;

//varying vec2 texCoord;
//attribute vec2 TexCoord;

void main(void)
{
    gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);
    position = gl_Position.xyz;

    normal = normalize(vec4(NormalMatrix * vec4(Normal, 1.0)).xyz);
    texCoord = TexCoord;
}
