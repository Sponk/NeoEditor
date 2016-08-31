precision mediump float;

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

varying vec4 position;
varying vec3 color;
varying vec3 normal;
varying vec2 texCoord;
varying vec3 tangent;

uniform sampler2D Textures[5];
uniform int TextureMode;

uniform vec3 Emit;
uniform vec3 Diffuse;
uniform vec3 Specular;
uniform float Shininess;
uniform float Opacity;

uniform LightInfo PixelLights[4];
uniform int PixelLightsCount;

uniform vec3 AmbientLight;

vec4 calculatePhongLight(LightInfo light, vec3 p, vec3 n, float shininess)
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

	return attenuation * vec4(light.Intensity * (light.Diffuse * max(dot(s, n), 0.0) + light.Diffuse * Specular * pow(max(dot(h, n), 0.0), shininess)), 1.0);
}

void main()
{
	vec3 norm = normal;
	float shininess = Shininess;

	if(TextureMode == 7)
	{
		gl_FragColor.rgb = Diffuse;
		gl_FragColor.a = texture2D(Textures[0], texCoord).a;
		return;
	}
	else if(TextureMode == 0)
	{
		gl_FragColor.rgb = Diffuse;
		gl_FragColor.a = Opacity;
	}
	else if(TextureMode == 1)
	{
		gl_FragColor = texture2D(Textures[0], texCoord);
	}
	else if(TextureMode >= 2)
   	{
		gl_FragColor = texture2D(Textures[0], texCoord);
		vec3 spec = texture2D(Textures[1], texCoord).rgb;
		shininess = (spec.r + spec.b + spec.g) / 3.0 * Shininess;

		if(TextureMode == 3)
		{
			vec3 bi = normalize(cross(normal, tangent));
			vec3 tang = normalize(tangent.xyz);

			vec3 bump = normalize(texture2D(Textures[2], texCoord).xyz * 2.0 - 1.0);
			norm = vec3(normalize(tang*bump.x + bi*bump.y + normal*bump.z));
		}
   	}
   	/*else if(TextureMode == 4)
  	{
		gl_FragColor = texture2D(Textures[0], texCoord);

   	}
   	else if(TextureMode == 5)
   	{
		gl_FragColor = texture2D(Textures[0], texCoord);
   	}*/
   	else if(TextureMode == 6)
   	{
   		gl_FragColor.rgb = color;
   		gl_FragColor.a = Opacity;
   	}

	if(Opacity >= 1.0)
	{
		// Only display if it's worth it
		if(gl_FragColor.a < 0.5)
        	discard;

        gl_FragColor.a = 1.0; // This prevents those clear color artifacts
	}

	vec4 accum = vec4(0,0,0,0);
	if(PixelLightsCount >= 1)
	{
		accum = accum + calculatePhongLight(PixelLights[0], position.xyz,
    							norm, 2.0*shininess);

    	if(PixelLightsCount >= 2)
        {
        	accum = accum + calculatePhongLight(PixelLights[1], position.xyz,
                      							norm, 2.0*shininess);

            if(PixelLightsCount >= 3)
            {
            	accum = accum + calculatePhongLight(PixelLights[2], position.xyz,
                            							norm,  2.0*shininess);

                if(PixelLightsCount >= 4)
                {
                   	accum = accum + calculatePhongLight(PixelLights[3], position.xyz,
                                  							norm,  2.0*shininess);
                }

            }
        }
	}

   	/*for(int i = 0; i < 4; i++)
   	{
   		accum = accum + calculatePhongLight(Lights[i], position.xyz,
							norm, gl_FragColor, 2.0*shininess);
   	}*/

   	gl_FragColor.rgb = AmbientLight + gl_FragColor.rgb * Emit + gl_FragColor.rgb * (color + accum.rgb);
}
