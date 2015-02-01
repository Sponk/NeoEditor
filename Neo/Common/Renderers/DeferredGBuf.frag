#version 330

uniform sampler2D Texture;
uniform int TextureMode;
uniform vec3 DiffuseColor;

//varying vec2 texCoord;
in vec2 texCoord;
in vec3 position;
in vec3 normal;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Normal;

void main(void)
{
    if(TextureMode == 1)
	FragColor = texture2D(Texture, texCoord);
    else
	FragColor = vec4(DiffuseColor,1.0);

    Normal = vec4(normal, 1.0);
}
