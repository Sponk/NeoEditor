#version 330

#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_shader_subroutine : enable
//#extension GL_ARB_bindless_texture : enable

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
    
    int ShadowTexture;

    float SpotExp;
    float SpotCos;
    vec3 SpotDir;
    
    mat4 ShadowMatrix;
};

uniform LightInfo lights[MAX_ENTITY_LIGHTS];
uniform int LightsCount;

uniform vec3 AmbientLight;

uniform int Width;
uniform int Height;
uniform int PostEffects;
uniform float Near;
uniform float Far;

uniform float dofFocus = 0.2;
uniform float dofStrength = 2.0;
uniform int dofAutofocus = 1;

uniform sampler2D Textures[5];
in vec2 texCoord;

vec3 Specular = vec3(1,1,1);

#ifdef GL_ARB_explicit_attrib_location
layout (location = 0) out vec4 FragColor;
#else
out vec4 FragColor;
#endif

vec4 cookTorranceSpecular(LightInfo light, vec3 p, vec3 n, vec4 diffuse, float roughness)
{
  vec3 l;
  if(light.SpotCos < 1.0)
  	l = light.Position - p;
  else
  	l = -light.SpotDir;

  // Gauss constant
  float c = 1.0;
  
  vec3 s = normalize(l);
  vec3 v = normalize(-p);
  vec3 h = normalize(v+s);

  float nDoth = dot(n, h);
  float nDotv = dot(n, v);
  float vDoth = dot(v, h);
  float nDots = dot(n, s);

  float Geometric = min(1.0, min((2*nDoth*nDotv)/vDoth, (2*nDoth*nDots)/vDoth));

  float alpha = acos(nDoth);
  float Roughness = c * exp(-(alpha / (roughness * roughness)));

  const float normIncidence = 1.0;
  const float F0 = 1;
  float Fresnel = F0 + pow(1.0f - vDoth, 5.0f) * (1.0f - F0);
  Fresnel *= (1.0f - normIncidence);
  Fresnel += normIncidence;

  float numerator = (Fresnel * Geometric * Roughness);
  float denominator = nDotv * nDots;
  float rs = numerator / denominator;
  vec3 retval = light.Intensity * (max(0.0, nDots) * ((diffuse.rgb * light.Diffuse) + (light.Diffuse*Specular) * rs));
      
  if(light.SpotCos > 0.0 && light.SpotCos < 1.0)
  {
	float spot = dot(-s, light.SpotDir);

	if(spot > light.SpotCos)
	{
		spot = clamp(pow(spot, light.SpotExp), 0.0, 1.0);
        float attenuation = spot/(light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation));//*shadow;

		return vec4(attenuation*retval, 1.0);
    }
   
    return vec4(0,0,0,0);
   }
   
   float attenuation = 1.0/(light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation));
   return vec4(attenuation*retval, 1.0);
}

vec4 calculatePhongLight(LightInfo light, vec3 p, vec3 n, vec4 diffuse, float shininess)
{  
  vec3 l = light.Position - p;
  vec3 s = normalize(l);
  vec3 v = normalize(-p);
  vec3 h = normalize(v + s);
  float ldsqr = dot(l,l);
 
  shininess *= 2.0;
  vec3 returnColor = light.Intensity * (AmbientLight + light.Diffuse * diffuse.rgb * max(dot(s, n), 0.0) + Specular * pow(max(dot(h,n), 0.0), shininess));
  
  if(light.SpotCos > 0.0 && light.SpotCos < 1.0)
  {
	float spot = dot(-s, light.SpotDir);

	if(spot > light.SpotCos)
	{
		spot = clamp(pow(spot, light.SpotExp), 0.0, 1.0);

		float attenuation = (spot / (light.ConstantAttenuation + (ldsqr * light.QuadraticAttenuation)));//*shadow;
   		return vec4(returnColor * attenuation, 1.0);
   	}
   
    return vec4(0,0,0,0);
   }
  
  float attenuation = (1.0 / (light.ConstantAttenuation + (ldsqr * light.QuadraticAttenuation)));
  return vec4(returnColor * attenuation, 1.0);
}

vec4 calculateAllCookLight(vec3 p, vec3 n, vec4 d, float s)
{
  vec4 result;
  for(int i = 0; i < LightsCount; i++)
  {
    result = result + cookTorranceSpecular(lights[i], p, n, d, s);
  }
  return result;
}

vec4 calculateAllPhongLight(vec3 p, vec3 n, vec4 d, float s)
{
  vec4 result;
  for(int i = 0; i < LightsCount; i++)
    result = result + calculatePhongLight(lights[i], p, n, d, s);

  return result;
}

vec4 gammaCorrection(vec4 diffuse, float gamma)
{
	return pow(diffuse, vec4(1.0 / gamma));
}

vec4 blur(vec4 diffuse, vec2 texcoord, sampler2D sampler, float amount)
{
    if(amount == 0.0)
       return diffuse;

	diffuse += texture2D(sampler, texcoord+vec2(0.01, 0.0) * amount);
	diffuse += texture2D(sampler, texcoord+vec2(-0.01, 0.0) * amount);
	diffuse += texture2D(sampler, texcoord+vec2(0.0, 0.01) * amount);
	diffuse += texture2D(sampler, texcoord+vec2(0.0, -0.01) * amount);
	diffuse += texture2D(sampler, texcoord+vec2(0.007, 0.007) * amount);
	diffuse += texture2D(sampler, texcoord+vec2(-0.007, -0.007) * amount);
	diffuse += texture2D(sampler, texcoord+vec2(0.007, -0.007) * amount);
	diffuse += texture2D(sampler, texcoord+vec2(-0.007, 0.007) * amount);

	return diffuse / vec4(8.0);
}

// Based on code found here: 
// http://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/
vec3 fxaa(sampler2D textureSampler, vec2 vertTexcoord, vec2 texcoordOffset)
{
	// The parameters are hardcoded for now, but could be
	// made into uniforms to control fromt he program.
	const float FXAA_SPAN_MAX = 8.0;
	const float FXAA_REDUCE_MUL = 1.0/8.0;//1.0/8.0;
	const float FXAA_REDUCE_MIN = 1.0/128.0;//(1.0/128.0);

	vec3 rgbNW = texture(textureSampler, vertTexcoord + (vec2(-1.0, -1.0) * texcoordOffset)).xyz;
	vec3 rgbNE = texture(textureSampler, vertTexcoord + (vec2(+1.0, -1.0) * texcoordOffset)).xyz;
	vec3 rgbSW = texture(textureSampler, vertTexcoord + (vec2(-1.0, +1.0) * texcoordOffset)).xyz;
	vec3 rgbSE = texture(textureSampler, vertTexcoord + (vec2(+1.0, +1.0) * texcoordOffset)).xyz;
	vec3 rgbM  = texture(textureSampler, vertTexcoord).xyz;
	
	const vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot( rgbM, luma);
	
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	  
	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), 
    	  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * texcoordOffset;
		
	vec3 rgbA = (1.0/2.0) * (
              texture(textureSampler, vertTexcoord + dir * (1.0/3.0 - 0.5)).xyz +
              texture(textureSampler, vertTexcoord + dir * (2.0/3.0 - 0.5)).xyz);
	vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
              texture(textureSampler, vertTexcoord + dir * (0.0/3.0 - 0.5)).xyz +
              texture(textureSampler, vertTexcoord + dir * (3.0/3.0 - 0.5)).xyz);
	float lumaB = dot(rgbB, luma);

	if((lumaB < lumaMin) || (lumaB > lumaMax))
	   	return rgbA;

	return rgbB;
}

#define FXAA_SUBPIX_SHIFT 5//0.5
#define BLOOM_SAMPLES 4
#define HALF_BLOOM_SAMPLES 4
#define BLOOM_SHIFT 0.0018
#define BLOOM_STRENGTH 0.015

vec3 bloom(sampler2D sampler, vec2 texcoord, vec3 diffuse, vec2 frame, float strength, float minLum)
{
	float avg = ((diffuse.r + diffuse.g + diffuse.b) / 3.0);
    vec3 sum = vec3(0.0);

    for (int i = -5; i <= 5; i++)
    {
    	for (int j = -5; j <= 5; j++)
    	{
    	    vec3 color = texture(sampler, (texcoord + frame * vec2(i, j))).rgb;

            if((color.r + color.g + color.b) / 3.0 >= minLum)
                sum += color * strength;//0.015;
    	}
    }

    if (avg < 0.025)
    {
    	return diffuse + sum * 0.335;
    }
    else if (avg < 0.10)
    {
    	return diffuse + (sum * sum) * 0.5;
    }
    else if (avg < 0.88)
    {
    	return diffuse + ((sum * sum) * 0.333);
    }
    else if (avg >= 0.88)
    {
    	return diffuse + sum;
    }

   	return diffuse;
}

vec4 radialBlur(vec4 color, vec2 texcoord, sampler2D sampler, float sampleDist, float sampleStrength)
{
	float samples[10] =
	float[](-0.08,-0.05,-0.03,-0.02,-0.01,0.01,0.02,0.03,0.05,0.08);

	vec2 dir = vec2(0.5, 0.5) - texcoord;

	float dist = sqrt(dir.x*dir.x + dir.y*dir.y);

	dir = dir/dist;

	vec4 sum = color;
	for (int i = 0; i < 10; i++)
	{
		sum += texture2D(sampler, texcoord + dir * samples[i] * sampleDist);
	}

	sum *= 1.0/11.0;

	float t = dist * sampleStrength;
	t = clamp(t, 0.0, 1.0); //0 &lt;= t &lt;= 1

	return mix(color, sum, t);
}

float linearize_depth(float z, float near, float far)
{
	return (2.0 * near) / (far + near - z * (far - near));
}

// SSAO
const int sample_count = 16;
const vec2 filter_offset[] = vec2[](vec2(-0.94201624, -0.39906216),
                                vec2(0.94558609, -0.76890725),
                                vec2(-0.094184101, -0.92938870),
                                vec2(0.34495938, 0.29387760),
                                vec2(-0.91588581, 0.45771432),
                                vec2(-0.81544232, -0.87912464),
                                vec2(-0.38277543, 0.27676845),
                                vec2(0.97484398, 0.75648379),
                                vec2(0.44323325, -0.97511554),
                                vec2(0.53742981, -0.47373420),
                                vec2(-0.26496911, -0.41893023),
                                vec2(0.79197514, 0.19090188),
                                vec2(-0.24188840, 0.99706507),
                                vec2(-0.81409955, 0.91437590),
                                vec2(0.19984126, 0.78641367),
                                vec2(0.14383161, -0.14100790));

float ssao(vec2 texcoord, vec2 filterRadius, sampler2D depthTexture, sampler2D positionTexture, sampler2D normalTexture)
{
	const float distanceThreshold = 5.0;
	const vec3 lumcoeff = vec3(0.299,0.587,0.114);

	float occlusion = 0;
	vec3 viewPos = texture2D(positionTexture, texcoord).xyz;
	vec3 viewNormal = texture2D(normalTexture, texcoord).xyz;

	for(int i = 0; i < sample_count; i++)
    {
        vec2 sampleTexCoord = texcoord + (filter_offset[i] * filterRadius);
        vec3 samplePos = texture2D(positionTexture, sampleTexCoord).xyz;
        vec3 sampleDir = normalize(samplePos - viewPos);

        float NdotS = max(dot(viewNormal, sampleDir), 0);
        float VPdistSP = distance(viewPos, samplePos);

        float a = 1.0 - smoothstep(distanceThreshold, distanceThreshold * 2, VPdistSP);
        float b = NdotS;

        occlusion += (a * b);
    }

    float lum = dot(FragColor.rgb, lumcoeff);

    return mix(1.0 - (occlusion / sample_count), 1.0, lum);
}

void main(void)
{
	FragColor = texture2D(Textures[0], texCoord);
	float transparency = FragColor.a;

	if(PostEffects == 1)
	{
		vec4 normal = texture2D(Textures[1], texCoord);

		//FragColor.rgb = vec3(normal.a);
		//return;

		float depth = linearize_depth(texture2D(Textures[4], texCoord).r, Near, Far);
		float depthCenter = linearize_depth(texture2D(Textures[4], vec2(0.5, 0.5)).r, Near, Far);
		vec2 frame = vec2(1.0 / Width, 1.0/ Height);

        float dofAmount = dofStrength * (depth - ((dofAutofocus == 1) ? depthCenter : dofFocus));

		FragColor.rgb = fxaa(Textures[0], texCoord, frame);
        FragColor.rgb = bloom(Textures[0], texCoord, FragColor.rgb, frame, 0.035, 1.0);

		FragColor.rgb = mix(FragColor.rgb, blur(FragColor, texCoord, Textures[0], dofAmount).rgb, dofStrength*abs(dofAmount));

		//FragColor.rgb = vec3(ssao(texCoord, frame * 10.0, Textures[4], Textures[2], Textures[1]));

		if(normal.a == 0 && transparency == 1.0)
		{
			FragColor.rgb *= ssao(texCoord, frame * 10.0, Textures[4], Textures[2], Textures[1]);
		}

		FragColor = transparency * gammaCorrection(FragColor, 1.2);
		return;
	}

	if(FragColor.a == 0.0)
		discard;
		
	vec4 data = texture2D(Textures[3], texCoord);
	vec4 startColor = FragColor;
	vec4 n = texture2D(Textures[1], texCoord);

	if(FragColor.a > 0 && n.a == 0)
	{
		vec4 p = texture2D(Textures[2], texCoord);
		FragColor = calculateAllCookLight(p.xyz, n.rgb, FragColor, p.a) + vec4(startColor.rgb * data.rgb, 0.0);
		FragColor.a = transparency;
	}
}
