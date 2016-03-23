#version 330

uniform mat4 ProjModelViewMatrix;
uniform mat4 ModelViewMatrix;

uniform mat4 NormalMatrix;
uniform mat4 ModelMatrix;

uniform vec3 ObjectPosition;

in vec3 Vertex;
in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Color;

out vec2 texCoord;
out vec3 normal;
out vec3 position;
out vec3 tangent;
out vec3 color;
out mat4 projModelViewMatrix;

//varying vec2 texCoord;
//attribute vec2 TexCoord;

void main(void)
{
    gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);
    //vec4 tmp = ModelViewMatrix * vec4(Vertex, 1.0);
    //position = tmp.xyz;
    //position = (ModelViewMatrix * vec4(Vertex, 1.0)).xyz;
    position = (ModelViewMatrix * vec4(Vertex, 1.0)).xyz;

    normal = normalize((NormalMatrix * vec4(Normal, 0.0)).xyz);
    texCoord = TexCoord;
    tangent = mat3(NormalMatrix)*Tangent;
    color = Color;
    projModelViewMatrix = ProjModelViewMatrix;
}
