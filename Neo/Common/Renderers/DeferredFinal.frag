#version 330

#define MAX_ENTITY_LIGHTS 256
#define SPECULAR_MULTIPLIER 3.0

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
};

uniform LightInfo lights[MAX_ENTITY_LIGHTS];
uniform int LightsCount;

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
    vec3 lightDistance = light.Position - pos;
    float lightDistance2 = dot(lightDistance, lightDistance);

    //if(lightDistance2 > light.Radius)
	//return vec3(0.0,0.0,0.0);

    vec3 s = normalize(lightDistance);
    vec3 v = normalize(-pos);
    vec3 r = reflect(s, n);
    float attenuation = (1.0 / (light.ConstantAttenuation + (lightDistance2 * light.QuadraticAttenuation)));

    vec3 Ka = vec3(0);

    vec3 diffuse = (Ka + light.Diffuse *  max( 0.0,dot(n, s)));
    vec3 spec = Ks * pow(max(0.0, dot(r,v)), shininess * SPECULAR_MULTIPLIER);

    return tex * light.Intensity * (spec + diffuse) * attenuation; //(diffuse + spec);
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


   FragColor = vec4(0.0,0.0,0.0,0.0);
   for(int i = 0; i < LightsCount; i++)
   {
       //FragColor = FragColor + vec4(diffuseModel(position.rgb, normal.rgb, currentPixel.rgb, position.aaa, lights[i].Position, lights[i].Diffuse, lights[i].Intensity, normal.a), 1.0);
       FragColor = FragColor + vec4(lambertModel(position.rgb, normal.rgb, currentPixel.rgb, position.aaa, lights[i], normal.a), 1.0);
   }
   //}
   //else
   //{
    //if(texCoord.x > 0.5)
      //  FragColor = texture2D(Textures[3], texCoord);

    //if(texCoord.x > 0.5 && texCoord.y < 0.5)
      //  FragColor = texture2D(Textures[1], texCoord);

    //FragColor.a = 1.0;
	//}

}
