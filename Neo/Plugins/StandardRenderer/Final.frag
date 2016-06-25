#version 330

#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_shader_subroutine : enable
//#extension GL_ARB_bindless_texture : enable

#define MAX_ENTITY_LIGHTS 256

// #define shadingAlgorithm phongShading
#define shadingAlgorithm cookTorranceShading
const float epsilon = 1e-5; //1e-6; // Very small value

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

// uniform LightInfo lights[MAX_ENTITY_LIGHTS];
uniform int LightsCount;
uniform sampler2D LightData;

uniform vec3 AmbientLight;

uniform int Width;
uniform int Height;
uniform int PostEffects;
uniform float Near;
uniform float Far;
uniform mat4 CameraViewMatrix;
uniform float Time;

uniform float dofFocus = 0.2;
uniform float dofStrength = 2.0;
uniform int dofAutofocus = 1;

uniform sampler2D Textures[6];
in vec2 texCoord;

vec3 Specular = vec3(1, 1, 1);

#ifdef GL_ARB_explicit_attrib_location
layout(location = 0) out vec4 FragColor;
#else
out vec4 FragColor;
#endif

vec4 cookTorranceSpecular(LightInfo light, vec3 p, vec3 n, vec4 diffuse,
						  float roughness)
{
	vec3 l;
	if (light.SpotCos < 1.0)
		l = light.Position - p;
	else
		l = -light.SpotDir;

	// Gauss constant
	const float c = 1.0;

	vec3 s = normalize(l);
	vec3 v = normalize(-p);
	vec3 h = normalize(v + s);

	float attenuation = (1.0 / (light.ConstantAttenuation +
								light.LinearAttenuation * length(l) +
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
			return vec4(0, 0, 0, 0);
	}

	
	float nDoth = dot(n, h);
	float nDotv = dot(n, v);
	float vDoth = dot(v, h);
	float nDots = dot(n, s);

	float Geometric =
		min(1.0, min((2 * nDoth * nDotv) / vDoth, (2 * nDoth * nDots) / vDoth));

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
	vec3 retval =
		light.Intensity * (max(0.0, nDots) * ((diffuse.rgb * light.Diffuse) +
											  (light.Diffuse * Specular) * rs));
	
	return vec4(attenuation * retval, 1.0);
}

vec4 calculatePhongLight(LightInfo light, vec3 p, vec3 n, vec4 diffuse,
						 float shininess)
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
			return vec4(0, 0, 0, 0);
	}
	
	return attenuation * vec4(light.Intensity * (light.Diffuse * diffuse.rgb * max(dot(s, n), 0.0) + light.Diffuse * Specular * pow(max(dot(h, n), 0.0), shininess)), 1.0);
}

vec4 cookTorranceShading(vec3 p, vec3 n, vec4 d, float s)
{
	vec4 result = vec4(0, 0, 0, 0);

	float id = 0.0;
	LightInfo light;
	vec3 data;

	// Convert shininess to roughness
	s = sqrt(2.0f/(1.25f*s));
	
	for (int i = 0; i < LightsCount; i++)
	{
	    light.Position = texelFetch(LightData, ivec2(i, 0), 0).rgb;
		light.Radius = texelFetch(LightData, ivec2(i, 2), 0).x;

		data = texelFetch(LightData, ivec2(i, 1), 0).xyz;

		// The radius is actually a little bigger than the actual value
		// due to the falloff value
		if(data.x < 1.0f && distance(light.Position, p) > light.Radius * 1.5f)
			continue;

		light.SpotCos = data.x;
		light.SpotExp = data.y;
		light.Intensity = data.z;
		
		data = texelFetch(LightData, ivec2(i, 3), 0).xyz;
		light.LinearAttenuation = data.x;
		light.QuadraticAttenuation = data.y;
		light.ConstantAttenuation = data.z;

		light.SpotDir = texelFetch(LightData, ivec2(i, 4), 0).xyz;
		light.Diffuse = texelFetch(LightData, ivec2(i, 5), 0).xyz;

		result = result + cookTorranceSpecular(light, p, n, d, s);
	}
	return result;
}

vec4 phongShading(vec3 p, vec3 n, vec4 d, float s)
{
	vec4 result = vec4(0, 0, 0, 0);

	float id = 0.0;
	LightInfo light;
	vec3 data;

	//s *= 4.0f;
	s = s * 2.0f + epsilon; // Ensure that we never do x^0 since that would produce white spots
	
	for (int i = 0; i < LightsCount; i++)
	{
	    light.Position = texelFetch(LightData, ivec2(i, 0), 0).rgb;
		light.Radius = texelFetch(LightData, ivec2(i, 2), 0).x;

		data = texelFetch(LightData, ivec2(i, 1), 0).xyz;
		
		// The radius is actually a little bigger than the actual value
		// due to the falloff value
		if(data.x < 1.0f && distance(light.Position, p) > light.Radius * 1.5f)
			continue;
		
		light.SpotCos = data.x;
		light.SpotExp = data.y;
		light.Intensity = data.z;
		
		data = texelFetch(LightData, ivec2(i, 3), 0).xyz;
		light.LinearAttenuation = data.x;
		light.QuadraticAttenuation = data.y;
		light.ConstantAttenuation = data.z;

		light.SpotDir = texelFetch(LightData, ivec2(i, 4), 0).xyz;
		light.Diffuse = texelFetch(LightData, ivec2(i, 5), 0).xyz;

		result = result + calculatePhongLight(light, p, n, d, s);
	}
	return result;
}

/***********************************************************************************************************************
 *
 * Post Effects
 *
 ***********************************************************************************************************************/

// Values found here:
// http://www.geeks3d.com/20100628/3d-programming-ready-to-use-64-sample-poisson-disc/
const vec2 poissonDisk[] =
	vec2[](vec2(-0.613392, 0.617481), vec2(0.170019, -0.040254),
		   vec2(-0.299417, 0.791925), vec2(0.645680, 0.493210),
		   vec2(-0.651784, 0.717887), vec2(0.421003, 0.027070),
		   vec2(-0.817194, -0.271096), vec2(-0.705374, -0.668203),
		   vec2(0.977050, -0.108615), vec2(0.063326, 0.142369),
		   vec2(0.203528, 0.214331), vec2(-0.667531, 0.326090),
		   vec2(-0.098422, -0.295755), vec2(-0.885922, 0.215369),
		   vec2(0.566637, 0.605213), vec2(0.039766, -0.396100),
		   vec2(0.751946, 0.453352), vec2(0.078707, -0.715323),
		   vec2(-0.075838, -0.529344), vec2(0.724479, -0.580798),
		   vec2(0.222999, -0.215125), vec2(-0.467574, -0.405438),
		   vec2(-0.248268, -0.814753), vec2(0.354411, -0.887570),
		   vec2(0.175817, 0.382366), vec2(0.487472, -0.063082),
		   vec2(-0.084078, 0.898312), vec2(0.488876, -0.783441),
		   vec2(0.470016, 0.217933), vec2(-0.696890, -0.549791),
		   vec2(-0.149693, 0.605762), vec2(0.034211, 0.979980),
		   vec2(0.503098, -0.308878), vec2(-0.016205, -0.872921),
		   vec2(0.385784, -0.393902), vec2(-0.146886, -0.859249),
		   vec2(0.643361, 0.164098), vec2(0.634388, -0.049471),
		   vec2(-0.688894, 0.007843), vec2(0.464034, -0.188818),
		   vec2(-0.440840, 0.137486), vec2(0.364483, 0.511704),
		   vec2(0.034028, 0.325968), vec2(0.099094, -0.308023),
		   vec2(0.693960, -0.366253), vec2(0.678884, -0.204688),
		   vec2(0.001801, 0.780328), vec2(0.145177, -0.898984),
		   vec2(0.062655, -0.611866), vec2(0.315226, -0.604297),
		   vec2(-0.780145, 0.486251), vec2(-0.371868, 0.882138),
		   vec2(0.200476, 0.494430), vec2(-0.494552, -0.711051),
		   vec2(0.612476, 0.705252), vec2(-0.578845, -0.768792),
		   vec2(-0.772454, -0.090976), vec2(0.504440, 0.372295),
		   vec2(0.155736, 0.065157), vec2(0.391522, 0.849605),
		   vec2(-0.620106, -0.328104), vec2(0.789239, -0.419965),
		   vec2(-0.545396, 0.538133), vec2(-0.178564, -0.596057));

vec4 gammaCorrection(vec4 diffuse, float gamma)
{
	return pow(diffuse, vec4(1.0 / gamma));
}

vec4 blur(vec4 diffuse, vec2 texcoord, sampler2D sampler, float amount)
{
	if (amount == 0.0)
		return diffuse;

	diffuse += texture2D(sampler, texcoord + vec2(0.01, 0.0) * amount);
	diffuse += texture2D(sampler, texcoord + vec2(-0.01, 0.0) * amount);
	diffuse += texture2D(sampler, texcoord + vec2(0.0, 0.01) * amount);
	diffuse += texture2D(sampler, texcoord + vec2(0.0, -0.01) * amount);
	diffuse += texture2D(sampler, texcoord + vec2(0.007, 0.007) * amount);
	diffuse += texture2D(sampler, texcoord + vec2(-0.007, -0.007) * amount);
	diffuse += texture2D(sampler, texcoord + vec2(0.007, -0.007) * amount);
	diffuse += texture2D(sampler, texcoord + vec2(-0.007, 0.007) * amount);

	return diffuse / vec4(8.0);
}

// Based on code found here:
// http://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/
vec3 fxaa(sampler2D textureSampler, vec2 vertTexcoord, vec2 texcoordOffset)
{
	// The parameters are hardcoded for now, but could be
	// made into uniforms to control fromt he program.
	const float FXAA_SPAN_MAX = 3.0;
	const float FXAA_REDUCE_MUL = 1.0 / 8.0;   // 1.0/8.0;
	const float FXAA_REDUCE_MIN = 1.0 / 128.0; //(1.0/128.0);

	vec3 rgbNW = texture(textureSampler,
						 vertTexcoord + (vec2(-1.0, -1.0) * texcoordOffset))
					 .xyz;
	vec3 rgbNE = texture(textureSampler,
						 vertTexcoord + (vec2(+1.0, -1.0) * texcoordOffset))
					 .xyz;
	vec3 rgbSW = texture(textureSampler,
						 vertTexcoord + (vec2(-1.0, +1.0) * texcoordOffset))
					 .xyz;
	vec3 rgbSE = texture(textureSampler,
						 vertTexcoord + (vec2(+1.0, +1.0) * texcoordOffset))
					 .xyz;
	vec3 rgbM = texture(textureSampler, vertTexcoord).xyz;

	const vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(rgbM, luma);

	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float dirReduce =
		max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
			FXAA_REDUCE_MIN);

	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
			  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) *
		  texcoordOffset;

	vec3 rgbA =
		(1.0 / 2.0) *
		(texture(textureSampler, vertTexcoord + dir * (1.0 / 3.0 - 0.5)).xyz +
		 texture(textureSampler, vertTexcoord + dir * (2.0 / 3.0 - 0.5)).xyz);
	vec3 rgbB =
		rgbA * (1.0 / 2.0) +
		(1.0 / 4.0) *
			(texture(textureSampler, vertTexcoord + dir * (0.0 / 3.0 - 0.5))
				 .xyz +
			 texture(textureSampler, vertTexcoord + dir * (3.0 / 3.0 - 0.5))
				 .xyz);
	float lumaB = dot(rgbB, luma);

	if ((lumaB < lumaMin) || (lumaB > lumaMax))
		return rgbA;

	return rgbB;
}

#define FXAA_SUBPIX_SHIFT 5 // 0.5
#define BLOOM_SAMPLES 4
#define HALF_BLOOM_SAMPLES 4
#define BLOOM_SHIFT 0.0018
#define BLOOM_STRENGTH 0.015

vec3 bloom(sampler2D sampler, vec2 texcoord, vec3 diffuse, vec2 frame,
		   float strength, float minLum)
{
	const int kernel_radius = 8;
	const int kernel_content = kernel_radius * kernel_radius;
	float avg = ((diffuse.r + diffuse.g + diffuse.b) / 3.0);
	vec3 sum = vec3(0.0);

	int poisson = 0;
	/*for (int i = -kernel_radius; i <= kernel_radius; i++)
	{
		for (int j = -kernel_radius; j <= kernel_radius; j++)
		{
			vec3 color = texture(sampler, (texcoord + poissonDisk[poisson++] *
	frame * vec2(i, j))).rgb;

			//if((color.r + color.g + color.b) / 3.0 >= minLum)
				sum += color * strength;//0.015;
		}
	}*/

	int y = -kernel_radius;
	int x = -kernel_radius;
	while (y < kernel_radius)
	{
		vec3 color =
			texture(sampler,
					(texcoord + poissonDisk[poisson] * frame * vec2(x, y)))
				.rgb;

		if ((color.r + color.g + color.b) / 3.0 >= minLum)
			sum += color * strength; // 0.015;

		poisson++;
		if ((++x) >= kernel_radius)
		{
			x = -kernel_radius;
			y++;
		}
	}

	/*vec2 multiplier = (frame * (0.5 * kernel_radius));
	for(int i = 0; i < kernel_radius; i++)
	{
		vec2 sampleTexCoord = texcoord + (poissonDisk[i] * multiplier);
		vec3 color = texture(sampler, sampleTexCoord).rgb;

		if((color.r + color.g + color.b) / 3.0 >= minLum)
			sum += color;// * strength;//0.015;
	}*/

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

vec4 radialBlur(vec4 color, vec2 texcoord, sampler2D sampler, float sampleDist,
				float sampleStrength)
{
	float samples[10] = float[](-0.08, -0.05, -0.03, -0.02, -0.01, 0.01, 0.02,
								0.03, 0.05, 0.08);

	vec2 dir = vec2(0.5, 0.5) - texcoord;

	float dist = sqrt(dir.x * dir.x + dir.y * dir.y);

	dir = dir / dist;

	vec4 sum = color;
	for (int i = 0; i < 10; i++)
	{
		sum += texture2D(sampler, texcoord + dir * samples[i] * sampleDist);
	}

	sum *= 1.0 / 11.0;

	float t = dist * sampleStrength;
	t = clamp(t, 0.0, 1.0); // 0 &lt;= t &lt;= 1

	return mix(color, sum, t);
}

float linearize_depth(float z, float near, float far)
{
	return (2.0 * near) / (far + near - z * (far - near));
}

float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// SSAO
#define SSAO_ITERATIONS 12
float ssao(vec2 texcoord, vec2 filterRadius, sampler2D depthTexture,
		   sampler2D positionTexture, sampler2D normalTexture)
{
	const int samples = min(SSAO_ITERATIONS, 64);
	const float distanceThreshold = 5.0;
	const vec3 lumcoeff = vec3(0.299, 0.587, 0.114);

	float occlusion = 0;
	vec3 viewPos = texture2D(positionTexture, texcoord).xyz;
	vec3 viewNormal = texture2D(normalTexture, texcoord).xyz;
	vec2 multiplier = filterRadius * max((16.0f/viewPos.z), 1.0f);
	
	for (int i = 0; i < samples; i++)
	{
		vec2 sampleTexCoord = texcoord + (poissonDisk[i] * multiplier);
		vec3 samplePos = texture2D(positionTexture, sampleTexCoord).xyz;
		vec3 sampleDir = normalize(samplePos - viewPos);

		float NdotS = max(dot(viewNormal, sampleDir), 0);
		float VPdistSP = distance(viewPos, samplePos); 

		float a = 1.0 - smoothstep(distanceThreshold, distanceThreshold * 2, VPdistSP);
		//float b = NdotS;

		occlusion = (a * NdotS) + occlusion; 
	}

	float lum = dot(FragColor.rgb, lumcoeff);
	return mix(/*pow(*/1.0 - (occlusion / samples)/*, 2.0)*/, 1.0, lum);
}

void main(void)
{
	FragColor = texture2D(Textures[0], texCoord);
	float transparency = FragColor.a;
	// FragColor.a = 1.0;
	
	if (PostEffects == 1)
	{
		// FragColor = texture2D(LightData, texCoord);
		// return;

		vec4 normal = texture2D(Textures[1], texCoord);

		float depth =
			linearize_depth(texture2D(Textures[4], texCoord).r, Near, Far);
		float depthCenter = linearize_depth(
			texture2D(Textures[4], vec2(0.5, 0.5)).r, Near, Far);
		vec2 frame = vec2(1.0 / Width, 1.0 / Height);

		// FragColor.rgb = vec3(ssao(texCoord, frame * 30.0, Textures[4],
		// Textures[2], Textures[1]));
		// return;

		// float dofAmount = dofStrength * (depth - ((dofAutofocus == 1) ?
		// depthCenter : dofFocus));

		FragColor.rgb = fxaa(Textures[0], texCoord, frame);
		// FragColor.rgb = bloom(Textures[0], texCoord, FragColor.rgb, frame,
		// 0.04, 1.0);

		// FragColor.rgb = mix(FragColor.rgb, blur(FragColor, texCoord,
		// Textures[0], dofAmount).rgb, dofStrength*abs(dofAmount));

		//FragColor.rgb = vec3(ssao(texCoord, frame * 13.0, Textures[4],
		//						   Textures[2], Textures[1]));
		//return;

		if (normal.a == 0 && transparency == 1.0)
		{
			FragColor.rgb *= ssao(texCoord, frame * 12.0, Textures[4],
								  Textures[2], Textures[1]);
		}

		FragColor = transparency * gammaCorrection(FragColor, 1.2);
		return;
	}

	/*if(texCoord.y <= 0.5)
		if(texCoord.x > 0.5)
			FragColor = texture2D(Textures[1], texCoord);
		else
			FragColor = texture2D(Textures[3], texCoord);

	if(texCoord.y > 0.5)
		if(texCoord.x > 0.5)
			FragColor = texture2D(Textures[2], texCoord);*/

	// return;

	if (FragColor.a == 0.0)
		discard;

	vec4 data = texture2D(Textures[3], texCoord);
	vec4 startColor = FragColor;
	vec4 n = texture2D(Textures[1], texCoord);

	if (FragColor.a > 0 && n.a == 0)
	{
		vec4 p = texture2D(Textures[2], texCoord);
		FragColor = shadingAlgorithm(p.xyz, n.rgb, FragColor, p.a) +
					vec4(startColor.rgb * data.rgb, 0.0) + vec4(AmbientLight, 1.0);
		FragColor.a = transparency;
	}
}
