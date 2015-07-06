#version 400

uniform sampler2D Textures[5];
#define MAX_ENTITY_LIGHTS 256

subroutine vec4 shadeModelType(vec3 position, vec3 normal);
subroutine uniform shadeModelType shadeModel;

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

uniform int TextureMode;
uniform vec3 DiffuseColor;

uniform vec3 Diffuse;
uniform vec3 Emit;
uniform vec3 Specular;
uniform float Shininess;
uniform int HasTransparency;

uniform vec2 TextureOffset;

//varying vec2 texCoord;
in vec2 texCoord;
in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec3 color;
in mat4 projModelViewMatrix;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Normal;
layout (location = 2) out vec4 Position;
layout (location = 3) out vec3 Data;

float inverseSquareLaw(float distance, float strength)
{
  return 1.0 * (1.0/(0.01*(distance*distance)));
}

float gaussConstant  = 100;

float cookTorranceSpecularPower(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal,
                                  float roughness, float fresnel)

  {

  float VdotN = max(dot(viewDirection, surfaceNormal), 0.0);
  float LdotN = max(dot(lightDirection, surfaceNormal), 0.0);

  //Half angle vector
  vec3 H = normalize(lightDirection + viewDirection);

  //Geometric term
  float NdotH = max(dot(surfaceNormal, H), 0.0);
  float VdotH = max(dot(viewDirection, H), 0.000001);
  float LdotH = max(dot(lightDirection, H), 0.000001);
  float G1 = (2.0 * NdotH * VdotN) / VdotH;
  float G2 = (2.0 * NdotH * LdotN) / LdotH;
  float G = min(1.0, min(G1, G2));

  //Distribution term
  //float D = beckmannDistribution(NdotH, roughness);
  float alpha = acos(NdotH);
  float D = 1.0 * exp(-((alpha * alpha) / (roughness * roughness)));

  //Fresnel term
  float F = pow(1.0 - VdotN, fresnel);

  //Multiply terms and done
  return  G * F * D / max(3.14159265 * VdotN, 0.000001);
}

/*vec4 cookTorranceSpecular(LightInfo light, vec3 p, vec3 n, vec4 diffuse)
{
  vec3 l = light.Position - p;

  if(length(l) > light.Radius) return vec4(0,0,0,0);
  
  vec3 s = normalize(l);
  vec3 v = normalize(-p);
  vec3 h = normalize(v+s);

  //vec3 retval = diffuse.rgb * cookTorranceSpecularPower(s, v, n, Shininess, 1.0);
  //return vec4(retval, diffuse.a);

  float nDoth = dot(n, h);
  float nDotv = dot(n, v);
  float vDoth = dot(v, h);
  float nDots = dot(n, s);

  float Geometric = min(1.0, min((2*nDoth*nDotv)/vDoth, (2*nDoth*nDots)/vDoth));

  float alpha = acos(nDoth);
  float Roughness = 1.0 * exp(-((alpha * alpha) / (Shininess * Shininess)));

  float F0 = 1;
  float Fresnel = F0 + pow(1 - vDoth, 5) * (1 - F0);

  float rs = ((Fresnel * Geometric * Roughness) / (3.14159265 * dot(v,n))); //(nDotv*3.141);

  float attenuation = (1.0 / (light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation)));

  //vec3 specular = max(0.0f, nDotv) * (Geometric * Roughness * Fresnel) / (nDotv * nDots) * light.Specular);
  vec3 retval = light.Intensity * max(0.0, nDots) * ((light.Specular * Specular) * rs + (diffuse.rgb * light.Diffuse));

  return vec4(attenuation*retval, diffuse.a);
}*/

vec4 cookTorranceSpecular(LightInfo light, vec3 p, vec3 n, vec4 diffuse, float roughness)
{
  vec3 l = light.Position - p;
  if(length(l) > light.Radius) return vec4(0,0,0,0);
  
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

vec4 calculatePhongLight(LightInfo light, vec3 p, vec3 n, vec4 diffuse, float shininess)
{
  vec3 lightDistance = light.Position - p;
  if(length(lightDistance) > light.Radius) return vec4(0,0,0,0);
  
  float lightDistance2 = dot(lightDistance, lightDistance);

  vec3 s = normalize(lightDistance);
  vec3 v = normalize(-p);
  vec3 r = normalize(reflect(-s, n));

  vec4 diffuseFinal = vec4((diffuse.rgb * light.Diffuse) *  max( 0.0,dot(n, s)), diffuse.a);
  vec4 specFinal = vec4(Specular * pow(max(0.0, dot(r,v)), shininess), 0.0f);

  if(light.SpotCos > 0.0 && light.SpotCos < 1.0)
  {
	float spot = dot(-s, light.SpotDir);

	if(spot > light.SpotCos)
	{
		spot = clamp(pow(spot, light.SpotExp), 0.0, 1.0);

        float attenuation = (spot / (light.ConstantAttenuation + (lightDistance2 * light.QuadraticAttenuation)));//*shadow;
        return light.Intensity * (specFinal + diffuseFinal) * attenuation;

        //vec3 S = normalize(E + L);
        //float spec = pow(max(dot(S, N), 0.0), shininess) * attenuation;
        //specular = Ks * light.Specular * spec;
    }
   
    return vec4(0,0,0,0);
   }
  
  float attenuation = (1.0 / (light.ConstantAttenuation + (lightDistance2 * light.QuadraticAttenuation)));
  return light.Intensity * (specFinal + diffuseFinal) * attenuation; //(diffuse + spec);
}

///////////////////////////////////////////////////////////
// Light calculation techniques
///////////////////////////////////////////////////////////
vec4 calculateAllPhongLight(vec3 p, vec3 n, vec4 d, float s)
{
  vec4 result;
  for(int i = 0; i < LightsCount; i++)
    result = result + calculatePhongLight(lights[i], p, n, d, s);

  return result;
}

vec4 calculateAllCookLight(vec3 p, vec3 n, vec4 d, float s)
{
  vec4 result;
  for(int i = 0; i < LightsCount; i++)
    result = result + cookTorranceSpecular(lights[i], p, n, d, s);

  return result;
}

///////////////////////////////////////////////////////////
// Cook Torrance Subroutines
///////////////////////////////////////////////////////////

subroutine(shadeModelType)
vec4 cookModelColor(vec3 position, vec3 n)
{
  return calculateAllCookLight(position, n, vec4(Diffuse, 1.0), Shininess);
}

subroutine(shadeModelType)
vec4 cookModelDiffuse(vec3 position, vec3 n)
{
	vec4 color = texture2D(Textures[0], texCoord);
	if(color.a < 0.5)
		discard;
		
	return calculateAllCookLight(position, n, color, Shininess);
}

subroutine(shadeModelType)
vec4 cookModelDiffuseSpecular(vec3 position, vec3 n)
{
	vec4 color = texture2D(Textures[0], texCoord);
	if(color.a < 0.5)
		discard;
		
	return calculateAllCookLight(position, n, color, texture2D(Textures[3], texCoord).x);
}

subroutine(shadeModelType)
vec4 cookModelDiffuseNormal(vec3 position, vec3 n)
{
	vec4 color = texture2D(Textures[0], texCoord);
	if(color.a < 0.5)
		discard;
		
	vec3 bi = normalize(cross(n, tangent));
    vec3 tan = normalize(tangent.xyz);

    vec3 bump = normalize(texture2D(Textures[2], texCoord).xyz * 2.0 - 1.0);
    n = normalize(tan * bump.x + bi * bump.y + n * bump.z);
	
	return calculateAllCookLight(position, n, color, Shininess);
}

subroutine(shadeModelType)
vec4 cookModelDiffuseNormalSpecular(vec3 position, vec3 n)
{
	vec4 color = texture2D(Textures[0], texCoord);
	if(color.a < 0.5)
		discard;
	
	vec3 bi = normalize(cross(n, tangent));
    vec3 tan = normalize(tangent.xyz);

    vec3 bump = normalize(texture2D(Textures[2], texCoord).xyz * 2.0 - 1.0);
    n = normalize(tan * bump.x + bi * bump.y + n * bump.z);
	
	return calculateAllCookLight(position, n, color, texture2D(Textures[3], texCoord).x);
}

///////////////////////////////////////////////////////////
// Phong Subroutines
///////////////////////////////////////////////////////////

subroutine(shadeModelType)
vec4 phongModelColor(vec3 position, vec3 n)
{
  return calculateAllPhongLight(position, n, vec4(Diffuse, 1.0), Shininess);
}

subroutine(shadeModelType)
vec4 phongModelDiffuse(vec3 position, vec3 n)
{
	vec4 color = texture2D(Textures[0], texCoord);
	if(color.a < 0.5)
		discard;
	
	return calculateAllPhongLight(position, n, color, Shininess);
}

subroutine(shadeModelType)
vec4 phongModelDiffuseSpecular(vec3 position, vec3 n)
{
	vec4 color = texture2D(Textures[0], texCoord);
	if(color.a < 0.5)
		discard;
		
	return calculateAllPhongLight(position, n, color, texture2D(Textures[3], texCoord).x);
}

subroutine(shadeModelType)
vec4 phongModelDiffuseNormal(vec3 position, vec3 n)
{
	vec4 color = texture2D(Textures[0], texCoord);
	if(color.a < 0.5)
		discard;
	
	vec3 bi = normalize(cross(n, tangent));
    vec3 tan = normalize(tangent.xyz);

    vec3 bump = normalize(texture2D(Textures[2], texCoord).xyz * 2.0 - 1.0);
    n = normalize(tan * bump.x + bi * bump.y + n * bump.z);
	
	return calculateAllPhongLight(position, n, color, Shininess);
}

subroutine(shadeModelType)
vec4 phongModelDiffuseNormalSpecular(vec3 position, vec3 n)
{
	vec4 color = texture2D(Textures[0], texCoord);
	if(color.a < 0.5)
		discard;
	
	vec3 bi = normalize(cross(n, tangent));
    vec3 tan = normalize(tangent.xyz);

    vec3 bump = normalize(texture2D(Textures[2], texCoord).xyz * 2.0 - 1.0);
    n = normalize(tan * bump.x + bi * bump.y + n * bump.z);

	return calculateAllPhongLight(position, n, color, texture2D(Textures[3], texCoord).x);
}

#define THRESHOLD 0.5
#define SQRT2 1.4142135623730951
#define SQRT2_2 0.70710678118654757

#define smooth 20.0

subroutine(shadeModelType)
vec4 TextShader(vec3 position, vec3 n)
{
	/*float size = 0.001;
	float w = step(0.3, texture2D(Textures[0], texCoord).w);
	w += step(0.3, texture2D(Textures[0], texCoord + vec2(size, 0.0)).w);
	w += step(0.3, texture2D(Textures[0], texCoord - vec2(size, 0.0)).w);
	w += step(0.3, texture2D(Textures[0], texCoord + vec2(0.0, size)).w);
	w += step(0.3, texture2D(Textures[0], texCoord - vec2(0.0, size)).w);
	w *= 0.2;*/
	
	float afwidth = smooth * SQRT2 / (2.0*gl_FragCoord.w);
	float alpha = smoothstep(THRESHOLD-afwidth, THRESHOLD+afwidth, length(gl_FragCoord.xyz));
	return /*vec4(Diffuse, alpha) */ vec4(Diffuse, 1.0) * texture2D(Textures[0], texCoord);// + vec4(0,0,0,1);
}

void main(void)
{
/*    if(TextureMode > 0)
	FragColor = vec4(Emit, 0.0) + texture2D(Textures[0], texCoord);
        */

	// When rendering text!
	if(TextureMode == -1)
	{
		FragColor = shadeModel(position, normal);

		if(FragColor.a < 0.5)
			discard;
		
	    Data.r = 1.0;
		//FragColor.a = 1.0;
		return;
	}
	
    Data.r = 0.0;

    //FragColor = calculatePhongLight(lights[0], position, normal, texture2D(Textures[0], texCoord));
	if(HasTransparency == 0)
	{
		if(TextureMode > 0)
			FragColor = vec4(Emit, 0.0) + texture2D(Textures[0], texCoord) + vec4(AmbientLight, 0.0);
		else
			FragColor = vec4(Emit, 0.0) + vec4(Diffuse, 1.0) + vec4(AmbientLight, 0.0);
				
		if(FragColor.a < 1.0) discard;
	}
	else
		FragColor = shadeModel(position, normal);
		
    //FragColor = vec4(normal, 1.0);
    //FragColor = vec4(normalize(position - lights[0].Position), 1.0);

    if(TextureMode >= 2)
    {
        vec3 bi = normalize(cross(normal, tangent));
        vec3 tan = normalize(tangent.xyz);

        vec3 bump = normalize(texture2D(Textures[2], texCoord).xyz * 2.0 - 1.0);
        Normal = vec4(normalize(tan*bump.x + bi*bump.y + normal*bump.z), Shininess);
        //Normal = vec4(normalize(normal+(texture2D(Textures[2], texCoord).xyz * 3 - 1)), Shininess);
    }
    else
		Normal = vec4(normal, Shininess);

    if(TextureMode >= 3)
    {
		vec4 spec = texture2D(Textures[3], texCoord);
        Position = vec4(position.xyz, (spec.r + spec.b + spec.g) / 3);
    }
    else
        Position = vec4(position.xyz, (Specular.r + Specular.b + Specular.g) / 3);
}
