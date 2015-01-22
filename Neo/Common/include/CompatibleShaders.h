/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MStandardShaders.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

namespace Neo
{
// vert header
string compat_vertHeader =

"attribute vec3 Vertex;"
"attribute vec3 Normal;"
"attribute vec3 Tangent;"
"attribute vec3 Color;"

"uniform bool LightShadow[3];"
"uniform mat4 LightShadowMatrix[3];"

"uniform mat4 TextureMatrix[4];"
"uniform mat4 ModelViewMatrix;"
"uniform mat4 ProjectionMatrix;"
"uniform mat4 NormalMatrix;"
"uniform mat4 ProjModelViewMatrix;"

"varying vec4 texCoord[2];"
"varying vec4 shadowCoord[3];"
"varying vec4 position, normal, tangent;";


// frag header
string compat_fragHeader =

"uniform bool AlphaTest;"
"uniform vec4 FogColor;"
"uniform float FogEnd;"
"uniform float FogScale;"

"uniform vec3 MaterialEmit;"
"uniform float MaterialShininess;"
"uniform float MaterialOpacity;"

"uniform vec4 LightPosition[4];"
"uniform vec3 LightDiffuseProduct[4];"
"uniform vec3 LightSpecularProduct[4];"
"uniform vec3 LightSpotDirection[4];"
"uniform float LightConstantAttenuation[4];"
"uniform float LightQuadraticAttenuation[4];"
"uniform float LightSpotCosCutoff[4];"
"uniform float LightSpotExponent[4];"
"uniform bool LightActive[4];"

"uniform sampler2D LightShadowMap[3];"
"uniform bool LightShadow[3];"
"uniform float LightShadowBias[3];"
"uniform float LightShadowBlur[3];"

"uniform sampler2D Texture[4];"
"uniform sampler2D RandTexture;"

"varying vec4 texCoord[2];"
"varying vec4 shadowCoord[3];"
"varying vec4 position, normal, tangent;";
							
								
// functions
string compat_functionsShader = string(

"float lookup(vec4 shadCoord, sampler2D shadMap, vec2 offSet)"
"{"
		"float distanceFromLight = texture2D(shadMap, shadCoord.xy + offSet).z;"
		"return step(shadCoord.z, distanceFromLight);"
"}"			
								
"float computeShadow(bool shad, vec4 shadCoord, sampler2D shadMap, float shadBias, float shadBlur)"
"{"
	"float shadow = 1.0;"
	"if(shad)"
	"{"
		"vec4 shadowCoordinateWdivide = shadCoord;"
		"shadowCoordinateWdivide.z -= shadBias;"

		"shadowCoordinateWdivide /= shadowCoordinateWdivide.w;"
										
		//"shadow = lookup(shadowCoordinateWdivide, shadMap, vec2(0.0, 0.0));"
		
		/*
		"shadow = lookup(shadowCoordinateWdivide, shadMap, vec2(0.0, 0.0));"
		"shadow += lookup(shadowCoordinateWdivide, shadMap, vec2(-0.001, 0.001)*shadBlur);"
		"shadow += lookup(shadowCoordinateWdivide, shadMap, vec2( 0.001, 0.001)*shadBlur);"
		"shadow += lookup(shadowCoordinateWdivide, shadMap, vec2(-0.001, -0.001)*shadBlur);"
		"shadow += lookup(shadowCoordinateWdivide, shadMap, vec2( 0.001, -0.001)*shadBlur);"
		"shadow /= 5.0;"*/
		
		 
        "float blur = (shadBlur*0.0001);"
		"vec4 rand = texture2D(RandTexture, (shadowCoordinateWdivide.xy)*(500.0/(shadBlur+1.0)))*2.0 - 1.0;"
		
		"vec2 d = rand.xy;"
		"d = normalize(d)*blur;"
								
		"vec2 dp = vec2(d.y, -d.x);"
		
		"shadow = lookup(shadowCoordinateWdivide, shadMap, rand.zw*blur);"
		"shadow += lookup(shadowCoordinateWdivide, shadMap,  d);"
		"shadow += lookup(shadowCoordinateWdivide, shadMap, -d);"
		"shadow += lookup(shadowCoordinateWdivide, shadMap,  dp);"
		"shadow += lookup(shadowCoordinateWdivide, shadMap, -dp);"
		"shadow *= 0.2;"

	"}"					
	"return shadow;"
"}"
								
"void computeLight(vec3 lightPosition, float constantAttenuation, float quadraticAttenuation, vec3 lightDiffuse, vec3 lightSpecular, vec3 spotDir, float spotCos, float spotExp, bool shad, vec4 shadCoord, sampler2D shadMap, float shadBias, float shadBlur)"
"{"
	"vec3 lightDir = lightPosition - position.xyz;"
	"vec3 L = normalize(lightDir);"

	"float lambertTerm = max(dot(N, L), 0.0);"
	"if(lambertTerm > 0.0)"
	"{"
        "if(spotCos > 0.0 && spotCos < 1.0)"
		"{"
			"float spot = dot(spotDir, -L);"
			
			"if(spot > spotCos)"
			"{"
				"float shadow = computeShadow(shad, shadCoord, shadMap, shadBias, shadBlur);"
								
				"spot = clamp(pow(spot, spotExp), 0.0, 1.0);"
								
				"float lightDirLength2 = dot(lightDir, lightDir);"
				"float attenuation = (spot / (constantAttenuation + (lightDirLength2 * quadraticAttenuation)))*shadow;"

				"diffuse = diffuse + (lightDiffuse * lambertTerm * attenuation);"

				"vec3 S = normalize(E + L);"
				"float spec = pow(max(dot(S, N), 0.0), MaterialShininess) * attenuation;"
				"specular = specular + (lightSpecular * spec);"
			"}"
		"}"
        "else if(spotCos >= 1.0)"
        "{"
                 "float shadow = computeShadow(shad, shadCoord, shadMap, shadBias, shadBlur);"

                 "float lightDirLength2 = dot(lightDir, lightDir);"
                 "float attenuation = (1.0 / (constantAttenuation + (lightDirLength2 * quadraticAttenuation)))*shadow;"

                 "diffuse = diffuse + (lightDiffuse * lambertTerm * attenuation);"

                 "vec3 S = normalize(E + L);"
                 "float spec = pow(max(dot(S, N), 0.0), MaterialShininess) * attenuation;"
                 "specular = specular + (lightSpecular * spec);"
        "}"
		"else"
		"{"
			"float lightDirLength2 = dot(lightDir, lightDir);"
			"float attenuation = (1.0 / (constantAttenuation + (lightDirLength2 * quadraticAttenuation)));"

			"diffuse = diffuse + (lightDiffuse * lambertTerm * attenuation);"

			"vec3 S = normalize(E + L);"
			"float spec = pow(max(dot(S, N), 0.0), MaterialShininess) * attenuation;"
			"specular = specular + (lightSpecular * spec);"
		"}"
	"}"
"}"

"void computeLightNoShadow(vec3 lightPosition, float constantAttenuation, float quadraticAttenuation, vec3 lightDiffuse, vec3 lightSpecular, vec3 spotDir, float spotCos, float spotExp)"
"{"
	"vec3 lightDir = lightPosition - position.xyz;"
	"vec3 L = normalize(lightDir);"

	"float lambertTerm = max(dot(N, L), 0.0);"
	"if(lambertTerm > 0.0)"
	"{"
        "if(spotCos > 0.0 && spotCos < 1.0)"
		"{"
			"float spot = dot(spotDir, -L);"
			
			"if(spot > spotCos)"
			"{"			
				"spot = clamp(pow(spot, spotExp), 0.0, 1.0);"
								
				"float lightDirLength2 = dot(lightDir, lightDir);"
				"float attenuation = (spot / (constantAttenuation + (lightDirLength2 * quadraticAttenuation)));"

				"diffuse = diffuse + (lightDiffuse * lambertTerm * attenuation);"

				"vec3 S = normalize(E + L);"
				"float spec = pow(max(dot(S, N), 0.0), MaterialShininess) * attenuation;"
				"specular = specular + (lightSpecular * spec);"
			"}"
		"}"
        "else if(spotCos >= 1.0)"
        "{"
             "float lightDirLength2 = dot(lightDir, lightDir);"
             "float attenuation = (1.0 / (constantAttenuation + (lightDirLength2 * quadraticAttenuation)));"

             "diffuse = diffuse + (lightDiffuse * lambertTerm * attenuation);"

             "vec3 S = normalize(E + L);"
             "float spec = pow(max(dot(S, N), 0.0), MaterialShininess) * attenuation;"
             "specular = specular + (lightSpecular * spec);"
        "}"
		"else"
		"{"
			"float lightDirLength2 = dot(lightDir, lightDir);"
			"float attenuation = (1.0 / (constantAttenuation + (lightDirLength2 * quadraticAttenuation)));"

			"diffuse = diffuse + (lightDiffuse * lambertTerm * attenuation);"

			"vec3 S = normalize(E + L);"
			"float spec = pow(max(dot(S, N), 0.0), MaterialShininess) * attenuation;"
			"specular = specular + (lightSpecular * spec);"
		"}"
	"}"
"}");






// lights
string compat_lightShader = string(
									
"if(LightActive[0])"
"{"
	"computeLight("
		"LightPosition[0].xyz,"
		"LightConstantAttenuation[0],"
		"LightQuadraticAttenuation[0],"
		"LightDiffuseProduct[0],"
		"LightSpecularProduct[0],"
		"LightSpotDirection[0],"
		"LightSpotCosCutoff[0],"
		"LightSpotExponent[0],"
		"LightShadow[0],"
		"shadowCoord[0],"
		"LightShadowMap[0],"
		"LightShadowBias[0],"
		"LightShadowBlur[0]"
	");"

	"if(LightActive[1])"
	"{"
		"computeLight("
			"LightPosition[1].xyz,"
			"LightConstantAttenuation[1],"
			"LightQuadraticAttenuation[1],"
			"LightDiffuseProduct[1],"
			"LightSpecularProduct[1],"
			"LightSpotDirection[1],"
			"LightSpotCosCutoff[1],"
			"LightSpotExponent[1],"
			"LightShadow[1],"
			"shadowCoord[1],"
			"LightShadowMap[1],"
			"LightShadowBias[1],"
			"LightShadowBlur[1]"
		");"

		"if(LightActive[2])"
		"{"
			"computeLight("
				"LightPosition[2].xyz,"
				"LightConstantAttenuation[2],"
				"LightQuadraticAttenuation[2],"
				"LightDiffuseProduct[2],"
				"LightSpecularProduct[2],"
				"LightSpotDirection[2],"
				"LightSpotCosCutoff[2],"
				"LightSpotExponent[2],"
				"LightShadow[2],"
				"shadowCoord[2],"
				"LightShadowMap[2],"
				"LightShadowBias[2],"
				"LightShadowBlur[2]"
			");"

			"if(LightActive[3])"
			"{"
				"computeLightNoShadow("
					"LightPosition[3].xyz,"
					"LightConstantAttenuation[2],"
					"LightQuadraticAttenuation[3],"
					"LightDiffuseProduct[3],"
					"LightSpecularProduct[3],"
					"LightSpotDirection[3],"
					"LightSpotCosCutoff[3],"
					"LightSpotExponent[3]"
				");"
			"}"
		"}"
	"}"
"}");






// Basic
string compat_vertShader0 = string(
"attribute vec3 Vertex;"
"uniform mat4 ProjModelViewMatrix;"
							
"void main(void)"
"{"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
"}");

string compat_fragShader0 = string(
"void main(void)"
"{"
	"gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);"
"}");


// Basic with texture
string compat_vertShader7 = string(
"attribute vec3 Vertex;"
"attribute vec2 TexCoord0;"
"uniform mat4 TextureMatrix[8];"
"uniform mat4 ProjModelViewMatrix;"
"varying vec2 texCoord[1];"
							
"void main(void)"
"{"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
	"texCoord[0] = (TextureMatrix[0] * vec4(TexCoord0, 1.0, 1.0)).xy;"
"}");

string compat_fragShader7 = string(
"uniform bool AlphaTest;"
"uniform float MaterialOpacity;"
"uniform sampler2D Texture[8];"
"varying vec2 texCoord[1];"
"void main(void)"
"{"
	"float w = texture2D(Texture[0], texCoord[0]).w;"
	"if(AlphaTest)"
		"if(w < 0.5) discard;"
	"gl_FragColor = vec4(1.0, 1.0, 1.0, w * MaterialOpacity);"
"}");




// Text FX
string compat_vertShader8 = string(
"attribute vec3 Vertex;"
"attribute vec2 TexCoord;"
"uniform mat4 ProjModelViewMatrix;"
"varying vec4 position;"
"varying vec2 texCoord;"
							
"void main(void)"
"{"
	"position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
	"gl_Position = position;"
	"texCoord = TexCoord;"
"}");

string compat_fragShader8 = string(
"uniform sampler2D Texture0;"
"uniform vec4 Color;"
"varying vec4 position;"
"varying vec2 texCoord;"

"void main(void)" // TODO: filtering for sharp zoom and better antialiasing ?
"{"
	"float w = texture2D(Texture0, texCoord).w;"
	/*"float size = 0.0005;"
	"float w = step(0.3, texture2D(Texture0, texCoord).w);"
	"w += step(0.3, texture2D(Texture0, texCoord + vec2(size, 0.0)).w);"
	"w += step(0.3, texture2D(Texture0, texCoord - vec2(size, 0.0)).w);"
	"w += step(0.3, texture2D(Texture0, texCoord + vec2(0.0, size)).w);"
	"w += step(0.3, texture2D(Texture0, texCoord - vec2(0.0, size)).w);"
	"w *= 0.2;"*/
							
	"gl_FragColor = vec4(Color.xyz, w*Color.w);"
"}");




// Standard simple
string compat_vertShader1 = string(
compat_vertHeader +

"void main(void)"
"{"
	"if(LightShadow[0]) shadowCoord[0] = LightShadowMatrix[0] * vec4(Vertex, 1.0);"
	"if(LightShadow[1]) shadowCoord[1] = LightShadowMatrix[1] * vec4(Vertex, 1.0);"
	"if(LightShadow[2]) shadowCoord[2] = LightShadowMatrix[2] * vec4(Vertex, 1.0);"

	"normal = NormalMatrix * vec4(Normal, 1.0);"
	"position = ModelViewMatrix * vec4(Vertex, 1.0);"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
"}");

string compat_fragShader1 = string(
compat_fragHeader +

"vec3 diffuse = MaterialEmit;"
"vec3 specular = vec3(0.0, 0.0, 0.0);"

"vec3 N = normalize(normal.xyz);"
"vec3 E = normalize(-position.xyz);"

+ compat_functionsShader +

"void main(void)"
"{"
    + compat_lightShader +

	"vec4 finalColor = vec4(diffuse + specular, MaterialOpacity);"
	"float fogFactor = clamp((FogEnd + position.z) * FogScale, 0.0, 1.0);"
	"gl_FragColor = mix(FogColor, finalColor, fogFactor);"
"}");


// Standard diffuse
string compat_vertShader2 = string(
compat_vertHeader +

"attribute vec2 TexCoord0;"
							
"void main(void)"
"{"
	"if(LightShadow[0]) shadowCoord[0] = LightShadowMatrix[0] * vec4(Vertex, 1.0);"
	"if(LightShadow[1]) shadowCoord[1] = LightShadowMatrix[1] * vec4(Vertex, 1.0);"
	"if(LightShadow[2]) shadowCoord[2] = LightShadowMatrix[2] * vec4(Vertex, 1.0);"

	"normal = NormalMatrix * vec4(Normal, 1.0);"
	"position = ModelViewMatrix * vec4(Vertex, 1.0);"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
	"texCoord[0].xy = (TextureMatrix[0] * vec4(TexCoord0, 1.0, 1.0)).xy;"
"}");

string compat_fragShader2 = string(
compat_fragHeader +
							
"vec4 texture0 = texture2D(Texture[0], texCoord[0].xy);"

"vec3 diffuse = MaterialEmit;"
"vec3 specular = vec3(0.0, 0.0, 0.0);"

"vec3 N = normalize(normal.xyz);"
"vec3 E = normalize(-position.xyz);"

+ compat_functionsShader +

"void main(void)"
"{"			
    + compat_lightShader +

	"vec4 finalColor = vec4(diffuse+specular, MaterialOpacity)*texture0;"
	"float fogFactor = clamp((FogEnd + position.z) * FogScale, 0.0, 1.0);"
	"gl_FragColor = mix(FogColor, finalColor, fogFactor);"
"}");


// Standard diffuse+specular
string compat_vertShader3 = string(
compat_vertHeader +

"attribute vec2 TexCoord0;"
"attribute vec2 TexCoord1;"
							
"void main(void)"
"{"
	"if(LightShadow[0]) shadowCoord[0] = LightShadowMatrix[0] * vec4(Vertex, 1.0);"
	"if(LightShadow[1]) shadowCoord[1] = LightShadowMatrix[1] * vec4(Vertex, 1.0);"
	"if(LightShadow[2]) shadowCoord[2] = LightShadowMatrix[2] * vec4(Vertex, 1.0);"

	"normal = NormalMatrix * vec4(Normal, 1.0);"
	"position = ModelViewMatrix * vec4(Vertex, 1.0);"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
	"texCoord[0].xy = (TextureMatrix[0] * vec4(TexCoord0, 1.0, 1.0)).xy;"
	"texCoord[0].zw = (TextureMatrix[1] * vec4(TexCoord1, 1.0, 1.0)).xy;"
"}");

string compat_fragShader3 = string(
compat_fragHeader +
							
"vec4 texture0 = texture2D(Texture[0], texCoord[0].xy);"						
"vec4 texture1 = texture2D(Texture[1], texCoord[0].zw);"

"vec3 diffuse = MaterialEmit;"
"vec3 specular = vec3(0.0, 0.0, 0.0);"

"vec3 N = normalize(normal.xyz);"
"vec3 E = normalize(-position.xyz);"

+ compat_functionsShader +

"void main(void)"
"{"				
    + compat_lightShader +

	"vec4 finalColor = vec4(diffuse*texture0.xyz + specular*texture1.xyz, MaterialOpacity*texture0.w);"
	"float fogFactor = clamp((FogEnd + position.z) * FogScale, 0.0, 1.0);"
	"gl_FragColor = mix(FogColor, finalColor, fogFactor);"
"}");


// Standard diffuse+specular+normal
string compat_vertShader4 = string(
compat_vertHeader +
		
"attribute vec2 TexCoord0;"
"attribute vec2 TexCoord1;"
"attribute vec2 TexCoord2;"

"void main(void)"
"{"
	"if(LightShadow[0]) shadowCoord[0] = LightShadowMatrix[0] * vec4(Vertex, 1.0);"
	"if(LightShadow[1]) shadowCoord[1] = LightShadowMatrix[1] * vec4(Vertex, 1.0);"
	"if(LightShadow[2]) shadowCoord[2] = LightShadowMatrix[2] * vec4(Vertex, 1.0);"

	"normal = NormalMatrix * vec4(Normal, 1.0);"
	"position = ModelViewMatrix * vec4(Vertex, 1.0);"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
	"texCoord[0].xy = (TextureMatrix[0] * vec4(TexCoord0, 1.0, 1.0)).xy;"
	"texCoord[0].zw = (TextureMatrix[1] * vec4(TexCoord1, 1.0, 1.0)).xy;"
	"texCoord[1].xy = (TextureMatrix[2] * vec4(TexCoord2, 1.0, 1.0)).xy;"

	"tangent = NormalMatrix * vec4(Tangent, 1.0);"
"}");

string compat_fragShader4 = string(
compat_fragHeader +
							
"vec4 texture0 = texture2D(Texture[0], texCoord[0].xy);"						
"vec4 texture1 = texture2D(Texture[1], texCoord[0].zw);"

"vec3 diffuse = MaterialEmit;"
"vec3 specular = vec3(0.0, 0.0, 0.0);"

"vec3 nor = normalize(normal.xyz);"
"vec3 bi = normalize(cross(normal.xyz, tangent.xyz));"
"vec3 tan = normalize(tangent.xyz);"

"vec3 bump = normalize(texture2D(Texture[2], texCoord[1].xy).xyz * 2.0 - 1.0);"

"vec3 N = normalize(tan*bump.x + bi*bump.y + nor*bump.z);"
"vec3 E = normalize(-position.xyz);"

+ compat_functionsShader +

"void main(void)"
"{"					
    + compat_lightShader +

	"vec4 finalColor = vec4(diffuse*texture0.xyz + specular*texture1.xyz, MaterialOpacity*texture0.w);"
	"float fogFactor = clamp((FogEnd + position.z) * FogScale, 0.0, 1.0);"
	"gl_FragColor = mix(FogColor, finalColor, fogFactor);"
"}");


// Standard diffuse+specular+emit
string compat_vertShader5 = string(
compat_vertHeader +

"attribute vec2 TexCoord0;"
"attribute vec2 TexCoord1;"
"attribute vec2 TexCoord2;"
"attribute vec2 TexCoord3;"
							
"void main(void)"
"{"
	"if(LightShadow[0]) shadowCoord[0] = LightShadowMatrix[0] * vec4(Vertex, 1.0);"
	"if(LightShadow[1]) shadowCoord[1] = LightShadowMatrix[1] * vec4(Vertex, 1.0);"
	"if(LightShadow[2]) shadowCoord[2] = LightShadowMatrix[2] * vec4(Vertex, 1.0);"

	"normal = NormalMatrix * vec4(Normal, 1.0);"
	"position = ModelViewMatrix * vec4(Vertex, 1.0);"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
	"texCoord[0].xy = (TextureMatrix[0] * vec4(TexCoord0, 1.0, 1.0)).xy;"
	"texCoord[0].zw = (TextureMatrix[1] * vec4(TexCoord1, 1.0, 1.0)).xy;"
	"texCoord[1].zw = (TextureMatrix[3] * vec4(TexCoord3, 1.0, 1.0)).xy;"
"}");

string compat_fragShader5 = string(
compat_fragHeader +
							
"vec4 texture0 = texture2D(Texture[0], texCoord[0].xy);"						
"vec4 texture1 = texture2D(Texture[1], texCoord[0].zw);"
"vec4 texture3 = texture2D(Texture[3], texCoord[1].zw);"

"vec3 diffuse = MaterialEmit*texture3.xyz;"
"vec3 specular = vec3(0.0, 0.0, 0.0);"

"vec3 N = normalize(normal.xyz);"
"vec3 E = normalize(-position.xyz);"

+ compat_functionsShader +

"void main(void)"
"{"					
    + compat_lightShader +

	"vec4 finalColor = vec4(diffuse*texture0.xyz + specular*texture1.xyz, MaterialOpacity*texture0.w);"
	"float fogFactor = clamp((FogEnd + position.z) * FogScale, 0.0, 1.0);"
	"gl_FragColor = mix(FogColor, finalColor, fogFactor);"
"}");


// Standard diffuse+specular+normal+emit
string compat_vertShader6 = string(
compat_vertHeader +
		
"attribute vec2 TexCoord0;"
"attribute vec2 TexCoord1;"
"attribute vec2 TexCoord2;"
"attribute vec2 TexCoord3;"

"void main(void)"
"{"
	"if(LightShadow[0]) shadowCoord[0] = LightShadowMatrix[0] * vec4(Vertex, 1.0);"
	"if(LightShadow[1]) shadowCoord[1] = LightShadowMatrix[1] * vec4(Vertex, 1.0);"
	"if(LightShadow[2]) shadowCoord[2] = LightShadowMatrix[2] * vec4(Vertex, 1.0);"

	"normal = NormalMatrix * vec4(Normal, 1.0);"
	"position = ModelViewMatrix * vec4(Vertex, 1.0);"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
	"texCoord[0].xy = (TextureMatrix[0] * vec4(TexCoord0, 1.0, 1.0)).xy;"
	"texCoord[0].zw = (TextureMatrix[1] * vec4(TexCoord1, 1.0, 1.0)).xy;"
	"texCoord[1].xy = (TextureMatrix[2] * vec4(TexCoord2, 1.0, 1.0)).xy;"
	"texCoord[1].zw = (TextureMatrix[3] * vec4(TexCoord3, 1.0, 1.0)).xy;"
		
	"tangent = NormalMatrix * vec4(Tangent, 1.0);"
"}");

string compat_fragShader6 = string(
compat_fragHeader +
							
"vec4 texture0 = texture2D(Texture[0], texCoord[0].xy);"						
"vec4 texture1 = texture2D(Texture[1], texCoord[0].zw);"
"vec4 texture3 = texture2D(Texture[3], texCoord[1].zw);"

"vec3 diffuse = MaterialEmit*texture3.xyz;"
"vec3 specular = vec3(0.0, 0.0, 0.0);"

"vec3 nor = normalize(normal.xyz);"
"vec3 bi = normalize(cross(normal.xyz, tangent.xyz));"
"vec3 tan = normalize(tangent.xyz);"

"vec3 bump = normalize(texture2D(Texture[2], texCoord[1].xy).xyz * 2.0 - 1.0);"

"vec3 N = normalize(tan*bump.x + bi*bump.y + nor*bump.z);"
"vec3 E = normalize(-position.xyz);"

+ compat_functionsShader +

"void main(void)"
"{"
    + compat_lightShader +

	"vec4 finalColor = vec4(diffuse*texture0.xyz + specular*texture1.xyz, MaterialOpacity*texture0.w);"
	"float fogFactor = clamp((FogEnd + position.z) * FogScale, 0.0, 1.0);"
	"gl_FragColor = mix(FogColor, finalColor, fogFactor);"
"}");
}
