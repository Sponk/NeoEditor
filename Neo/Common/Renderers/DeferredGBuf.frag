#version 330

uniform sampler2D Textures[5];
uniform int TextureMode;
uniform vec3 DiffuseColor;

uniform vec3 Diffuse;
uniform vec3 Emit;
uniform vec3 Specular;
uniform float Shininess;

//varying vec2 texCoord;
in vec2 texCoord;
in vec3 position;
in vec3 normal;
in vec3 tangent;
in mat3 normalMatrix;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Normal;
layout (location = 2) out vec4 Position;

void main(void)
{
    if(TextureMode > 0)
	FragColor = vec4(Emit, 0.0) + texture2D(Textures[0], texCoord);
    else
	FragColor = vec4(Diffuse,1.0);

    if(TextureMode >= 2)
	Normal = vec4(normalize(normal+(texture2D(Textures[2], texCoord).xyz * 2 - 1)), Shininess);
    else
	Normal = vec4(normal, Shininess);
    Position = vec4(position, 1.0);
}
