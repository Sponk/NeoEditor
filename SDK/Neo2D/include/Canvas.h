#ifndef NEO2D_CANVAS_H
#define NEO2D_CANVAS_H

#include <Neo2D.h>
#include <Object2D.h>
#include <memory>
#include <vector>

namespace Neo2D
{
class NEO2D_EXPORT Canvas
{
	std::vector<std::shared_ptr<Object2D>> m_objects;
	uint32_t m_priority;
	Neo::Vector2 m_size;

public:

	void addObject2D(std::shared_ptr<Object2D> obj) { m_objects.push_back(obj); }
	void setPriority(uint32_t p) { m_priority = p; }
	uint32_t getPriority() { return m_priority; }

	void update(float delta)
	{
#pragma omp parallel for
		for(int i = 0; i < m_objects.size(); i++)
			m_objects[i]->update(delta);
	}

	void draw()
	{
		for(auto o : m_objects)
			o->draw();
	}
};
}

#endif //NEO2D_CANVAS_H
