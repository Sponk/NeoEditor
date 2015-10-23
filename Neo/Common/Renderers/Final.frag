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

  //if(length(l) > light.Radius) return vec4(0,0,0,0);
  
  //roughness = roughness; //1.0/roughness;//0.00001; //roughness;
  // Guass constant
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

  float normIncidence = 1.0;
  float F0 = 1;
  float Fresnel = F0 + pow(1.0f - vDoth, 5.0f) * (1.0f - F0);
  Fresnel *= (1.0f - normIncidence);
  Fresnel += normIncidence;

  float numerator = (Fresnel * Geometric * Roughness);
  float denominator = nDotv * nDots;
  float rs = numerator / denominator;
  
  //float rs = ((Fresnel * Geometric * Roughness) / (3.14159265 * dot(v,n)));

  //vec3 specular = max(0.0f, nDotv) * (Geometric * Roughness * Fresnel) / (nDotv * nDots) * light.Specular);
  // vec3 retval = light.Intensity * max(0.0, nDots) * ((/*light.Specular */ Specular) * rs + (diffuse.rgb * light.Diffuse));
  vec3 retval = light.Intensity * (max(0.0, nDots) * ((diffuse.rgb * light.Diffuse) + (light.Diffuse*Specular) * rs));
      
  if(light.SpotCos > 0.0 && light.SpotCos < 1.0)
  {
	float spot = dot(-s, light.SpotDir);

	if(spot > light.SpotCos)
	{
		spot = clamp(pow(spot, light.SpotExp), 0.0, 1.0);
        	float attenuation = spot/(light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation));//*shadow;

		//retval = vec3(1.0,1.0,1.0);
        	return vec4(attenuation*retval, 1.0);
    	}
   
    return vec4(0,0,0,0);
   }
   
   float attenuation = 1.0/(light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation));//1.0/dot(l,l) * light.Radius; //(1.0 / dot(l,l));//)(light.ConstantAttenuation + (dot(l,l) * light.QuadraticAttenuation)));	
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
    result = result + cookTorranceSpecular(lights[i], p, n, d, s); // * textureProj(lights[i].ShadowTexture, );
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

vec4 blur(vec4 diffuse, vec2 texcoord, sampler2D sampler, float amount, float strength)
{
    if(strength == 0.0 || amount == 0.0)
       return diffuse;

        //strength *= float(textureSize(Texture[0], 1).x);

    vec4 color;
	color += texture2D(sampler, texcoord+vec2(0.01, 0.0) * amount);
	color += texture2D(sampler, texcoord+vec2(-0.01, 0.0) * amount);
	color += texture2D(sampler, texcoord+vec2(0.0, 0.01) * amount);
	color += texture2D(sampler, texcoord+vec2(0.0, -0.01) * amount);
	color += texture2D(sampler, texcoord+vec2(0.007, 0.007) * amount);
	color += texture2D(sampler, texcoord+vec2(-0.007, -0.007) * amount);
	color += texture2D(sampler, texcoord+vec2(0.007, -0.007) * amount);
	color += texture2D(sampler, texcoord+vec2(-0.007, 0.007) * amount);

	return mix(diffuse, color / vec4(8.0), amount);
}

// Based on code found here: 
// http://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/
vec4 fxaa(sampler2D textureSampler, vec2 vertTexcoord, vec2 texcoordOffset)
{
	// The parameters are hardcoded for now, but could be
	// made into uniforms to control fromt he program.
	float FXAA_SPAN_MAX = 8.0;
	float FXAA_REDUCE_MUL = 1.0/6.0;//1.0/8.0;
	float FXAA_REDUCE_MIN = 1.0/128.0;//(1.0/128.0);

	vec3 rgbNW = texture(textureSampler, vertTexcoord + (vec2(-1.0, -1.0) * texcoordOffset)).xyz;
	vec3 rgbNE = texture(textureSampler, vertTexcoord + (vec2(+1.0, -1.0) * texcoordOffset)).xyz;
	vec3 rgbSW = texture(textureSampler, vertTexcoord + (vec2(-1.0, +1.0) * texcoordOffset)).xyz;
	vec3 rgbSE = texture(textureSampler, vertTexcoord + (vec2(+1.0, +1.0) * texcoordOffset)).xyz;
	vec3 rgbM  = texture(textureSampler, vertTexcoord).xyz;
	
	vec3 luma = vec3(0.299, 0.587, 0.114);
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

	vec4 outfrag;
  
	if((lumaB < lumaMin) || (lumaB > lumaMax))
	{
    	outfrag.xyz = rgbA;
	} 
	else 
	{
    	outfrag.xyz = rgbB;
	}

    outfrag.a = 1.0;
  	return outfrag;
}

#define FxaaInt2 ivec2
#define FxaaFloat2 vec2
#define FxaaTexLod0(t, p) textureLod(t, p, 0.0)
#define FxaaTexOff(t, p, o, r) textureLodOffset(t, p, 0.0, o)

//uniform float FXAA_SPAN_MAX = 2.0;
//uniform float FXAA_REDUCE_MUL = 2.0; //1.0/8.0;

vec3 FxaaPixelShader(vec4 posPos, sampler2D tex, vec2 rcpFrame)
{   
/*--------------------------------------------------------------------------*/
	float FXAA_SPAN_MAX = 4.0; // 4.0
	float FXAA_REDUCE_MUL = 1.0/32.0; //1.0/8.0; //1.0/32.0;//1.0/8.0;
	float FXAA_REDUCE_MIN = 1.0/256.0;//1.0/256.0;//(1.0/128.0);
/*--------------------------------------------------------------------------*/
    vec3 rgbNW = FxaaTexLod0(tex, posPos.zw).xyz;
    vec3 rgbNE = FxaaTexOff(tex, posPos.zw, FxaaInt2(1,0), rcpFrame.xy).xyz;
    vec3 rgbSW = FxaaTexOff(tex, posPos.zw, FxaaInt2(0,1), rcpFrame.xy).xyz;
    vec3 rgbSE = FxaaTexOff(tex, posPos.zw, FxaaInt2(1,1), rcpFrame.xy).xyz;
    vec3 rgbM  = FxaaTexLod0(tex, posPos.xy).xyz;
/*--------------------------------------------------------------------------*/
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
/*--------------------------------------------------------------------------*/
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
/*--------------------------------------------------------------------------*/
    vec2 dir; 
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
/*--------------------------------------------------------------------------*/
    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(FxaaFloat2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX), 
          max(FxaaFloat2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), 
          dir * rcpDirMin)) * rcpFrame.xy;
/*--------------------------------------------------------------------------*/
    vec3 rgbA = (1.0/2.0) * (FxaaTexLod0(tex, posPos.xy + dir * (1.0/3.0 - 0.5)).xyz +
        FxaaTexLod0(tex, posPos.xy + dir * (2.0/3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        FxaaTexLod0(tex, posPos.xy + dir * (0.0/3.0 - 0.5)).xyz +
        FxaaTexLod0(tex, posPos.xy + dir * (3.0/3.0 - 0.5)).xyz);
    float lumaB = dot(rgbB, luma);
    if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
    return rgbB;
}

#define FXAA_SUBPIX_SHIFT 0.5

//#define TO_LIGHT(v) (v - vec4(0.5,0.5,0.5,0))
/*vec4 bloom(vec4 diffuse, vec2 texcoord, sampler2D sampler, float amount, float strength)
{
    if(strength == 0.0 || amount == 0.0)
       return diffuse;

        //strength *= float(textureSize(Texture[0], 1).x);

	diffuse.a = 1.0;

    vec4 color;
	color += TO_LIGHT(texture2D(sampler, texcoord+vec2(0.01, 0.0) * amount));
	color += TO_LIGHT(texture2D(sampler, texcoord+vec2(-0.01, 0.0) * amount));
	color += TO_LIGHT(texture2D(sampler, texcoord+vec2(0.0, 0.01) * amount));
	color += TO_LIGHT(texture2D(sampler, texcoord+vec2(0.0, -0.01) * amount));
	color += TO_LIGHT(texture2D(sampler, texcoord+vec2(0.007, 0.007) * amount));
	color += TO_LIGHT(texture2D(sampler, texcoord+vec2(-0.007, -0.007) * amount));
	color += TO_LIGHT(texture2D(sampler, texcoord+vec2(0.007, -0.007) * amount));
	color += TO_LIGHT(texture2D(sampler, texcoord+vec2(-0.007, 0.007) * amount));

	color = (color / 8.0);
	color.x = clamp(color.x, 0, 1);
	color.y = clamp(color.y, 0, 1);
	color.z = clamp(color.z, 0, 1);

	return color;
}*/

#define BLOOM_SAMPLES 4
#define HALF_BLOOM_SAMPLES 4
#define BLOOM_SHIFT 0.0018
#define BLOOM_STRENGTH 0.015

vec4 bloom(sampler2D sampler, vec2 texcoord, vec4 diffuse)
{
	return diffuse;
}

/*vec4 bloom(vec4 diffuse, vec2 texcoord, sampler2D sampler, float strength, int samples)
{
 return diffuse;
}*/

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

void main(void)
{
	FragColor = texture2D(Textures[0], texCoord);

	float transparency = FragColor.a;
	if(PostEffects == 1)
	{
		/*FragColor = texture2D(Textures[4], texCoord);
		
		float d = linearize_depth(FragColor.r, 1.0, 100);
		FragColor = vec4(d,d,d,1);
		
		return;*/

		//FragColor.a = transparency;
		//return;
		
		vec2 frame = vec2(vec2(1.0 / Width, 1.0/ Height));
		//FragColor = fxaa(Textures[0], texCoord, frame);

		FragColor.rgb = FxaaPixelShader(vec4(texCoord.xy, texCoord.xy - frame * (0.5 + FXAA_SUBPIX_SHIFT)), Textures[0], frame);

		//FragColor = radialBlur(FragColor, texCoord, Textures[0], 0.3, 5);
		// FragColor = blur(FragColor, texCoord, Textures[0], 1, 0.5);
		//FragColor = bloom(FragColor, texCoord, Textures[0], 0.01, 5);
		//FragColor += bloom(FragColor, texCoord, Textures[0], 0.01, 5);

		FragColor.a = transparency;
		FragColor = bloom(Textures[0], texCoord, FragColor);
		FragColor = gammaCorrection(FragColor, 1.2);
		//FragColor.a = transparency;

		/*if(FragColor.r > 1.0 || FragColor.g > 1.0 || FragColor.b > 1.0)
		    FragColor = vec4(1,1,1,1);
		else
		    FragColor = vec4(0);*/
		return;
	}
	
	if(FragColor.a == 0.0)
		discard;
		
	vec4 data = texture2D(Textures[3], texCoord);
	vec4 startColor = FragColor;
	vec4 n = texture2D(Textures[1], texCoord);

	// data.r = 0;float transparency = FragColor.a;
	if(FragColor.a == 1.0 && n.a == 0)
	{
		vec4 p = texture2D(Textures[2], texCoord);
		FragColor = calculateAllCookLight(p.xyz, n.rgb, FragColor, p.a) + vec4(startColor.rgb * data.rgb, 0.0);
		FragColor.a = transparency;
		//FragColor.xyz += data.rgb;
	}
}
