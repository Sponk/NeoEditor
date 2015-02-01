#version 330

uniform sampler2D Textures[5];
in vec2 texCoord;

out vec4 FragColor;

void main(void)
{
    if(texCoord.x <= 0.5)
    FragColor = texture2D(Textures[0], texCoord);
    else if(texCoord.y >= 0.5)
	FragColor = texture2D(Textures[1], texCoord);
    else
	FragColor = texture2D(Textures[2], texCoord);

}
