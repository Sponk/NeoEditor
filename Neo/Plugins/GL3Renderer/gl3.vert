#version 130

struct LightInfo
{
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Intensity;
    float QuadraticAttenuation;
    float ConstantAttenuation;
    float LinearAttenuation;
    float Radius;

    float SpotExp;
    float SpotCos;
    vec3 SpotDir;
};

uniform LightInfo VertexLights[4];
uniform int VertexLightsCount;

attribute vec3 Vertex;
attribute vec2 TexCoord;

attribute vec3 Normal;
attribute vec3 Color;
attribute vec3 Tangent;

uniform mat4 ProjModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat4 NormalMatrix;

uniform float Shininess;
uniform vec3 Specular;

varying vec3 color;
varying vec3 normal;
varying vec2 texCoord;
varying vec3 tangent;
varying vec4 position;

vec3 calculatePhongLight(LightInfo light, vec3 p, vec3 n, float shininess)
{
	vec3 l = light.Position - p;
	vec3 s = normalize(l);
	vec3 v = normalize(-p);
	vec3 h = normalize(v + s);

	float attenuation = (1.0 / (light.ConstantAttenuation +
								(dot(l, l) * light.QuadraticAttenuation)));

	if (light.SpotCos > 0.0 && light.SpotCos < 1.0)
	{
		float spot = dot(-s, light.SpotDir);

		if (spot > light.SpotCos)
		{
			spot = clamp(pow(spot - light.SpotCos, light.SpotExp), 0.0, 1.0);
			attenuation *= spot;
		}
		else
			return vec3(0, 0, 0);
	}

	return attenuation * light.Intensity * (light.Diffuse * max(dot(s, n), 0.0) + light.Diffuse * Specular * pow(max(dot(h, n), 0.0), shininess));
}

void main()
{
	gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);
	position = ViewMatrix * vec4(Vertex, 1.0);

	normal = normalize(mat3(NormalMatrix) * Normal);
	texCoord = TexCoord;
	tangent = mat3(NormalMatrix)*Tangent;

    color = vec3(0,0,0);
    if(VertexLightsCount >= 1)
    {
    	color = color + calculatePhongLight(VertexLights[0], position.xyz,
    				   							normal, 2.0*Shininess);

        if(VertexLightsCount >= 2)
        {
           	color = color + calculatePhongLight(VertexLights[1], position.xyz,
                         							normal, 2.0*Shininess);

            if(VertexLightsCount >= 3)
            {
               	color = color + calculatePhongLight(VertexLights[2], position.xyz,
                                							normal, 2.0*Shininess);

                if(VertexLightsCount >= 4)
                {
                   	color = color + calculatePhongLight(VertexLights[3], position.xyz,
                                   							normal, 2.0*Shininess);
                }
            }
        }
    }

	//color = Color;
}
