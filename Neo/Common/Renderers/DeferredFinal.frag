#version 330

#define MAX_ENTITY_LIGHTS 256

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

uniform LightInfo lights[MAX_ENTITY_LIGHTS];
uniform int LightsCount;

uniform vec3 AmbientLight;

uniform int Width;
uniform int Height;

// 0 -> GBuffer
// 1 -> Normals
// 2 -> Depth
uniform sampler2D Textures[5];
in vec2 texCoord;

layout (location = 0) out vec4 FragColor;

vec3 diffuseModel(vec3 pos, vec3 n, vec3 tex, vec3 Ks, vec3 lpos, vec3 Kd, float intensity, float shininess)
{
    vec3 s = normalize(lpos -  pos);
    vec3 v = normalize(-pos);
    vec3 r = reflect(s, n);


    vec3 Ka = vec3(0);

    vec3 diffuse = (Ka+ Kd *  max( 0.0,dot(n, s) ));
    vec3 spec = Ks *  pow(max(0.0, dot(r,v)), shininess);

    return tex * intensity * (diffuse + 0.5*spec);
}


vec3 lambertModel(vec3 pos, vec3 n, vec3 tex, vec3 Ks, LightInfo light, float shininess)
{
    vec3 lightDistance = pos - light.Position;
    float lightDistance2 = dot(lightDistance, lightDistance);

    vec3 s = normalize(lightDistance);
    vec3 v = normalize(-pos);
    vec3 r = normalize(reflect(s, n));
    float attenuation = (1.0 / (light.ConstantAttenuation + (lightDistance2 * light.QuadraticAttenuation)));

    vec3 diffuse = (light.Diffuse *  max( 0.0,dot(n, s)));
    vec3 spec = Ks * pow(max(0.0, dot(r,v)), shininess);

    return tex * light.Intensity * diffuse;//(spec + diffuse) * attenuation; //(diffuse + spec);
}

vec3 E;
vec3 light(vec3 position, vec3 N, vec3 tex, vec3 Ks, LightInfo light, float shininess)
{
    vec3 lightDir = normalize(light.Position - position);
    vec3 L = normalize(lightDir);
    float lightDirLength2 = dot(lightDir, lightDir);

    vec3 diffuse;
    vec3 specular;

    float lambertTerm = max(dot(N, L), 0.0);
    if(lambertTerm > 0.0 || light.SpotCos == 1.0)
    {
            if(light.SpotCos > 0.0 && light.SpotCos < 1.0)
            {
                    float spot = dot(light.SpotDir, -L);

                    if(spot > light.SpotCos)
                    {
                            //float shadow = computeShadow(shad, shadCoord, shadMap, shadBias, shadBlur);

                            spot = clamp(pow(spot, light.SpotExp), 0.0, 1.0);

                            float lightDirLength2 = dot(lightDir, lightDir);
                            float attenuation = (spot / (light.ConstantAttenuation + (lightDirLength2 * light.QuadraticAttenuation)));//*shadow;

                            diffuse = light.Diffuse * lambertTerm * attenuation;

                            vec3 S = normalize(E + L);
                            float spec = pow(max(dot(S, N), 0.0), shininess) * attenuation;
                            specular = Ks * light.Specular * spec;
                    }
            }
            else if(light.SpotCos >= 1.0)
            {
                     float shadow = 1.0; //computeShadow(shad, shadCoord, shadMap, shadBias, shadBlur);

                 float lightDirLength2 = dot(lightDir, lightDir);
                 float attenuation = (1.0 / (light.ConstantAttenuation + (lightDirLength2 * light.QuadraticAttenuation)));//*shadow;

                 diffuse = diffuse + (light.Diffuse * lambertTerm * attenuation);

                 vec3 S = normalize(E + L);
                 float spec = pow(max(dot(S, N), 0.0), shininess) * attenuation;
                 specular = specular + (light.Specular * spec);
            }
                    else
                    {
                            float lightDirLength2 = dot(lightDir, lightDir);
                            float attenuation = (1.0 / (light.ConstantAttenuation + (lightDirLength2 * light.QuadraticAttenuation)));

                            diffuse = diffuse + (light.Diffuse * lambertTerm * attenuation);

                            vec3 S = normalize(E + L);
                            float spec = pow(max(dot(S, N), 0.0), shininess) * attenuation;
                            specular = specular + (light.Specular * spec);
                    }
            }

    return tex * diffuse + specular;
}

void main(void)
{
    vec4 currentPixel = texture2D(Textures[0], texCoord);
    if(currentPixel.a != 1.0)
	discard;

    // normal.rgb => Normal
    // normal.a => Shininess
   vec4 normal = texture2D(Textures[1], texCoord);

   // position.rgb => Position
   // position.a => Grayscale specular
   vec4 position = texture2D(Textures[3], texCoord);
   E = vec3(normalize(-position));

   FragColor = vec4(AmbientLight, 1.0); //vec4(0.0,0.0,0.0,0.0);
   for(int i = 0; i < LightsCount; i++)
   {
       //FragColor = FragColor + vec4(diffuseModel(position.rgb, normal.rgb, currentPixel.rgb, position.aaa, lights[i].Position, lights[i].Diffuse, lights[i].Intensity, normal.a), 1.0);
       FragColor = FragColor + vec4(light(position.rgb, normal.rgb, currentPixel.rgb, position.aaa, lights[i], normal.a), 1.0);
   }
   //}
   //else
   //{
    if(texCoord.x < 0.5)
        FragColor = texture2D(Textures[3], texCoord);

   // if(texCoord.x < 0.5 && texCoord.y < 0.5)
     //   FragColor = abs(texture2D(Textures[1], texCoord));

    //FragColor = normalize(vec4(abs(lights[0].Position), 1.0));

    //FragColor.a = 1.0;
	//}

}
