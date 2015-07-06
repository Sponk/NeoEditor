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
// 3 -> Data
uniform sampler2D Textures[5];
in vec2 texCoord;

vec3 Specular = vec3(1,1,1);

layout (location = 0) out vec4 FragColor;

vec4 cookTorranceSpecular(LightInfo light, vec3 p, vec3 n, vec4 diffuse, float roughness)
{
  vec3 l = light.Position - p;
  //if(length(l) > light.Radius) return vec4(0,0,0,0);
  
  roughness = 1.0 - 1.0/roughness;//0.00001; //roughness;
  // Guass constant
  float c = 5.0;
  
  vec3 s = normalize(l);
  vec3 v = normalize(-p);
  vec3 h = normalize(v+s);

  float nDoth = dot(n, h);
  float nDotv = dot(n, v);
  float vDoth = dot(v, h);
  float nDots = dot(n, s);

  float Geometric = min(1.0, min((2*nDoth*nDotv)/vDoth, (2*nDoth*nDots)/vDoth));

  float alpha = acos(nDoth);
  float Roughness = c * exp(-((alpha * alpha) / (roughness * roughness)));

  float F0 = 1;
  float Fresnel = F0 + pow(1 - vDoth, 5) * (1 - F0);

  float rs = ((Fresnel * Geometric * Roughness) / (3.14159265 * dot(v,n)));

  //vec3 specular = max(0.0f, nDotv) * (Geometric * Roughness * Fresnel) / (nDotv * nDots) * light.Specular);
  // vec3 retval = light.Intensity * max(0.0, nDots) * ((/*light.Specular */ Specular) * rs + (diffuse.rgb * light.Diffuse));
  vec3 retval = light.Intensity * max(0.0, nDots) * (diffuse.rgb * light.Diffuse + (light.Specular * Specular) * rs);
   
  if(light.SpotCos > 0.0 /*&& light.SpotCos < 1.0*/)
  {
	float spot = dot(-s, light.SpotDir);

	if(spot > light.SpotCos)
	{
		spot = clamp(pow(spot, light.SpotExp), 0.0, 1.0);
        float attenuation = spot/(light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation));//*shadow;

		//retval = vec3(1.0,1.0,1.0);
        return vec4(attenuation*retval, diffuse.a);
    }
   
    return vec4(0,0,0,0);
   }
   
   float attenuation = 1.0/(light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation));//1.0/dot(l,l) * light.Radius; //(1.0 / dot(l,l));//)(light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation)));	
   return vec4(attenuation*retval, diffuse.a);
}

vec4 calculateAllCookLight(vec3 p, vec3 n, vec4 d, float s)
{
  vec4 result;
  for(int i = 0; i < LightsCount; i++)
    result = result + cookTorranceSpecular(lights[i], p, n, d, s);

  return result;
}

vec4 gammaCorrection(vec4 diffuse, float gamma)
{
	return pow(diffuse, vec4(1.0 / gamma));
}

void main(void)
{
	vec4 data = texture2D(Textures[4], texCoord);
    FragColor = texture2D(Textures[0], texCoord);//gammaCorrection(texture2D(Textures[0], texCoord), 1.2);

	if(FragColor.a == 1.0 && data.r == 0)
	{
		vec4 p = texture2D(Textures[3], texCoord);
		vec3 n = texture2D(Textures[1], texCoord).xyz;
		FragColor = calculateAllCookLight(p.xyz, n, FragColor, /*p.a*/ 1.0);
	}
	
	FragColor = gammaCorrection(FragColor, 1.2);
	//FragColor = texture2D(Textures[1], texCoord);
}
