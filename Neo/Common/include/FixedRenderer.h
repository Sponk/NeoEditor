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


#ifndef _M_FIXED_RENDERER_H
#define _M_FIXED_RENDERER_H

namespace Neo
{
// Fixed Renderer
class FixedRenderer : public Renderer
{
public:
	
	FixedRenderer(void);
	~FixedRenderer(void);
	
private:
	
	// skin cache
	unsigned int m_verticesNumber;
	unsigned int m_normalsNumber;
	Vector3 * m_vertices;
	Vector3 * m_normals;
	
private:
	
	void updateSkinning(Mesh * mesh, Armature * armature);
	void drawDisplay(SubMesh * subMesh, MaterialDisplay * display, Vector3 * vertices, Vector3 * normals, Color * colors);
	void drawDisplayTriangles(SubMesh * subMesh, MaterialDisplay * display, Vector3 * vertices);
	void drawOpaques(SubMesh * subMesh, Armature * armature);
	void drawTransparents(SubMesh * subMesh, Armature * armature);
	void updateVisibility(Scene * scene, OCamera * camera);
	void enableFog(OCamera * camera);
	
	float getDistanceToCam(OCamera * camera, const Vector3 & pos);
	
	// skin cache
	Vector3 * getVertices(unsigned int size);
	Vector3 * getNormals(unsigned int size);
	
public:
	
	// destroy
	void destroy(void);
	
	// get new
	static Renderer * getNew(void);

	// name
	static const char * getStaticName(void){ return "FixedRenderer"; }
	const char * getName(void){ return getStaticName(); }
	
	// draw
	void drawScene(Scene * scene, OCamera * camera);
	
	// extra
	void drawText(OText * textObj);
};
}

#endif
