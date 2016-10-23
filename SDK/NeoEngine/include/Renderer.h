//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __RENDERER_H
#define __RENDERER_H

namespace Neo
{
/**
 * @brief Provides methods to execute higher level graphics operations.
 *
 * Higher level means that it does not operate on OpenGL/etc. API specifics but
 * on Neo::Mesh, Neo::Scene etc. so the caller does not need any knowledge about
 * the underlying framework.
 */
class NEO_ENGINE_EXPORT Renderer
{
public:

	Renderer();
	virtual ~Renderer();

public:

	/**
	 * @brief Destroys the renderer object and frees all resources
	 * occupied by the object.
	 */
	virtual void destroy() = 0;

	/**
	 * @brief Initialized the renderer and all underlying frameworks.
	 */
	virtual void initialize() = 0;

	/**
	 * @brief Retrieves the name of this renderer type.
	 */
	virtual const char* getName() = 0;

	/**
	 * @brief Draws a scene as seen from the perspective of the given camera.
	 *
	 * @param scene The scene to display.
	 * @param camera The camera.
	 */
	virtual void drawScene(Scene* scene, OCamera* camera) = 0;

	/**
	 * @brief Draws a text object in 3D space.
	 *
	 * @param text The text to draw.
	 */
	virtual void drawText(OText* text) = 0;

	/**
	 * @brief Draws an entity on the screen as seen from the perspective of the given camera.
	 *
	 * @param entity The entity to draw.
	 * @param camera The camera.
	 */
	virtual void drawEntity(OEntity* entity, OCamera* camera) = 0;

	/**
	 * @brief Draws a colored 2D quad to the screen.
	 *
	 * @param position The 2D coordinates of the top left vertex.
	 * @param size The 2D width/height of the quad.
	 * @param color The color to draw with.
	 * @param rotation The angle to rotate around.
	 */
	virtual void drawColoredQuad(const Vector2& position, const Vector2& size, const Vector4& color,
		float rotation) = 0;

	/**
	* @brief Draws a textured 2D quad to the screen.
	*
	* @param position The 2D coordinates of the top left vertex.
	* @param size The 2D width/height of the quad.
	* @param texture The texture to use.
	* @param rotation The angle to rotate around.
	* @param scale The scale of the texture.
	* @param flip A vector indicating mirroring around several axis.
	* @param texcoords The texture coordinates to use.
	*/
	virtual void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale, const Vector2& flip, const Vector4& texcoords) = 0;

	/**
	* @brief Draws a textured 2D quad to the screen.
	*
	* @param position The 2D coordinates of the top left vertex.
	* @param size The 2D width/height of the quad.
	* @param texture The texture to use.
	* @param rotation The angle to rotate around.
	* @param scale The scale of the texture.
	* @param flip A vector indicating mirroring around several axis.
	*/
	virtual void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale, const Vector2& flip) = 0;

	/**
	* @brief Draws a textured 2D quad to the screen.
	*
	* @param position The 2D coordinates of the top left vertex.
	* @param size The 2D width/height of the quad.
	* @param texture The texture to use.
	* @param rotation The angle to rotate around.
	* @param scale The scale of the texture.
	*/
	virtual void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale) = 0;

	/**
	* @brief Draws a textured 2D quad to the screen.
	*
	* @param position The 2D coordinates of the top left vertex.
	* @param size The 2D width/height of the quad.
	* @param texture The texture to use.
	* @param rotation The angle to rotate around.
	*/
	virtual void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture, float rotation) = 0;

	/**
	* @brief Sets a 2D viewport with the given width and height.
	*
	* @param size The 2D vector containing the width and height.
	*/
	virtual void set2D(const Vector2& size) = 0;

	/**
	 * @brief Sends texture data to the graphics hardware.
	 *
	 * @param id The texture ID.
	 * @param image The image data to send.
	 * @param mipMap Indicates if mip-maps should be created.
	 * @param filter Indicates if the image should be filtered.
	 * @param compress Indicates if the image is compressed.
	 */
	virtual void sendTexture(unsigned int id, Image* image, bool mipMap, bool filter, bool compress) = 0;

	/**
	 * @brief Registers a texture image with the graphics engine.
	 * @return The texture ID used for referencing the texture in future.
	 */
	virtual unsigned int createTexture() = 0;

	/**
	 * @brief Un-registers a texture image from the graphics engine.
	 * @param id The texture to remove.
	 */
	virtual void destroyTexture(unsigned int id) = 0;
	virtual void clearScreen(const Vector3& color) = 0;

	virtual void drawText2D(Neo::OText* text, float x, float y, float rotation) = 0;

	virtual void enableScissors(unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
	virtual void disableScissors() = 0;
	
	virtual void enableDepthTest(bool value) = 0;
	
	virtual void setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
	
	virtual void stopThreads() {}
	virtual void startThreads() {}
	virtual void KillThreads() {}
};

class RendererDummy : public Renderer
{
public:
	virtual void destroy() override	{}
	virtual void initialize() override {}
	virtual const char* getName() override { return "DUMMY"; }
	virtual void drawScene(Scene* scene, OCamera* camera) override {}
	virtual void drawText(OText* text) override {}
	virtual void drawEntity(OEntity* entity, OCamera* camera) override {}
	virtual void drawColoredQuad(const Vector2& position,
								 const Vector2& size,
								 const Vector4& color,
								 float rotation) override {}

	virtual void drawTexturedQuad(const Vector2& position,
								  const Vector2& size,
								  int texture,
								  float rotation,
								  const Vector2& scale,
								  const Vector2& flip,
								  const Vector4& texcoords) override {}

	virtual void drawTexturedQuad(const Vector2& position,
								  const Vector2& size,
								  int texture,
								  float rotation,
								  const Vector2& scale,
								  const Vector2& flip) override {}

	virtual void drawTexturedQuad(const Vector2& position,
								  const Vector2& size,
								  int texture,
								  float rotation,
								  const Vector2& scale) override {}

	virtual void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture, float rotation) override {}

	virtual void drawText2D(Neo::OText* text, float x, float y, float rotation) override {};

	virtual void set2D(const Vector2& size) override {}
	virtual void stopThreads() override
	{
		Renderer::stopThreads();
	}

	virtual void startThreads() override
	{
		Renderer::startThreads();
	}

	virtual void KillThreads() override
	{
		Renderer::KillThreads();
	}

	virtual unsigned int createTexture() override { return 1; }
	virtual void sendTexture(unsigned int id, Image* image, bool mipMap, bool filter, bool compress) override {}
	virtual void destroyTexture(unsigned int id) override {}
	virtual void clearScreen(const Vector3& color) override {}
	
	virtual void enableScissors(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {}
	virtual void disableScissors() {}
	
	virtual void enableDepthTest(bool value) {}
	
	virtual void setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {}
};

}
#endif
