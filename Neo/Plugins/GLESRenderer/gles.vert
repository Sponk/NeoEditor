precision mediump float;

attribute vec3 Vertex;
attribute vec2 TexCoord;

attribute vec3 Normal;
attribute vec3 Color;
attribute vec3 Tangent;

uniform mat4 ProjModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat4 NormalMatrix;

varying vec3 color;
varying vec3 normal;
varying vec2 texCoord;
varying vec3 tangent;
varying vec4 position;

void main()
{
	gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);
	position = ViewMatrix * vec4(Vertex, 1.0);

	normal = normalize(mat3(NormalMatrix) * Normal);
	texCoord = TexCoord;
	tangent = mat3(NormalMatrix)*Tangent;
	color = Color;
}
