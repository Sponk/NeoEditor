#version 330

uniform sampler2D Textures[5];
in vec2 texCoord;

layout (location = 0) out vec4 FragColor;

vec3 diffuseModel(vec3 pos, vec3 norm, vec3 diff, vec3 lpos, float lintensity)
{
    vec3 s = normalize(lpos - pos);
    float sDotN = abs(dot(s, norm));
    return lintensity * diff * sDotN;
}

void main(void)
{
    //if(texCoord.x <= 0.5)
    //FragColor = texture2D(Textures[0], texCoord);
    //else if(texCoord.y > 0.5)
	//FragColor = texture2D(Textures[1], texCoord);
    //else
    vec4 currentPixel = texture2D(Textures[0], texCoord);
    if(currentPixel.a == 0.0)
	discard;

    FragColor = vec4(diffuseModel(texture2D(Textures[2], texCoord).rgb, texture2D(Textures[1], texCoord).rgb, currentPixel.rgb, vec3(0.0,1.0,0.0), 2.0), 1.0);
}
