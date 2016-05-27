#ifndef NEO2D_CANVAS_H
#define NEO2D_CANVAS_H

#include <Neo2D.h>
#include <Object2D.h>
#include <vector>

namespace Neo2D
{
/**
 * @brief Implements a Canvas that contains Object2Ds.
 *
 * A Canvas provides mechanisms for grouping Object2D together to draw and update them.
 */
class NEO2D_EXPORT Canvas
{
	std::vector<shared_ptr<Object2D>> m_objects;
	uint32_t m_priority;
	Neo::Vector2 m_size;

public:

	/**
	 * @brief Adds a new Object2D to the internal object list.
	 * @param obj The new Object2D.
	 */
	void addObject2D(shared_ptr<Object2D> obj) { m_objects.push_back(obj); }

	/**
	 * @brief Changes the rendering priority.
	 * @param p The new priority.
	 */
	void setPriority(uint32_t p) { m_priority = p; }

	/**
	 * @brief Retrieves the rendering priority.
	 * @return The priority.
	 */
	uint32_t getPriority() { return m_priority; }

	/**
	 * @brief Updates all Object2D's contained.
	 * @param delta The frame delta value.
	 */
	void update(float delta)
	{
// #pragma omp parallel for schedule(dynamic)
		for(int i = 0; i < m_objects.size(); i++)
			m_objects[i]->update(delta);
	}

	/**
	 * @brief Draws all objects contained.
	 */
	void draw()
	{
		for(auto o : m_objects)
			o->draw();
	}
};
}

#endif //NEO2D_CANVAS_H
