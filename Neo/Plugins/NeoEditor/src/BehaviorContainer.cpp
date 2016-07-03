#include "BehaviorContainer.h"
#include <Label.h>
#include <EditField.h>
#include <CheckButton.h>
#include "VectorEdit.cpp"

using namespace Neo;
using namespace Neo2D;
using namespace Gui;

static NeoVariable findVariable(Behavior* b, const char* name)
{
	for(int i = 0; i < b->getVariablesNumber(); i++)
	{
	    NeoVariable v = b->getVariable(i);
		if(!strcmp(name, v.getName()))
			return v;
	}

	return NeoVariable();
}

void BehaviorContainer::displayObject(Object3d* o)
{
	if(o == nullptr)
	{
		m_container->clear();
		m_numBehaviors = 0;
		m_currentObject = nullptr;
		return;
	}

	// Only update if it changed
	if(o == m_currentObject
	   && o->getBehaviorsNumber() == m_numBehaviors)
		return;

	m_container->clear();

	static auto stringSetter = [this](Widget& w, void* d) {
		static_cast<Neo::String*>(d)->set(w.getLabel());
	};

	static auto floatSetter = [this](Widget& w, void* d) {
		*static_cast<float*>(d) = std::stof(w.getLabel());
	};

	static auto vec3Setter = [this](Widget& w, void* d) {
		*static_cast<Vector3*>(d) = static_cast<Vector3Edit&>(w).getVector();
	};

	static auto vec4Setter = [this](Widget& w, void* d) {
		*static_cast<Vector4*>(d) = static_cast<Vector4Edit&>(w).getVector();
	};

	static auto boolSetter = [this](Widget& w, void* d) {
		*static_cast<bool*>(d) = static_cast<CheckButton&>(w).getValue();
	};

	for(int i = 0; i < o->getBehaviorsNumber(); i++)
	{		
		Behavior* b = o->getBehavior(i);

		auto label = make_shared<Label>(0, 0, getSize().x, 10, b->getName(), m_container);
		label->setColor(Vector4(0,0,0,1));
		m_container->addWidget(label);
		
		for(int j = 0; j < b->getVariablesNumber(); j++)
		{
			NeoVariable var = b->getVariable(j);

			if(var.getType() != M_VARIABLE_BOOL)
			{
				label = make_shared<Label>(0, 0, getSize().x, 10, var.getName(), m_container);
				label->setColor(Vector4(0,0,0.5,1));
				m_container->addWidget(label);
			}
			
			switch(var.getType())
			{
			case M_VARIABLE_STRING:
				{
					auto edit = make_shared<EditField>(0, 0, getSize().x, 20,
													   variable_cast<Neo::String>(var)->getSafeString(), m_container);
					edit->setCallback(stringSetter, var.getPointer());
					m_container->addWidget(edit);
				}
				break;

				case M_VARIABLE_FLOAT:
				{
					auto edit = make_shared<EditField>(0, 0, getSize().x, 20,
													   std::to_string(*variable_cast<float>(var)).c_str(), m_container);
					edit->setCallback(floatSetter, var.getPointer());

					m_container->addWidget(edit);
				}
				break;

				case M_VARIABLE_VEC4:
				{
					auto edit = make_shared<Vector4Edit>(0, 0, getSize().x, 20, "", m_container);
					edit->setCallback(vec4Setter, var.getPointer());
					edit->setVector(*variable_cast<Vector4>(var));

					m_container->addWidget(edit);
				}
				break;

				case M_VARIABLE_VEC3:
				{
					auto edit = make_shared<Vector3Edit>(0, 0, getSize().x, 20, "",
														 m_container);
					edit->setCallback(vec3Setter, var.getPointer());
					edit->setVector(*variable_cast<Vector3>(var));

					m_container->addWidget(edit);
				}
				break;

				case M_VARIABLE_BOOL:
				{
					auto edit = make_shared<CheckButton>(0, 0, getSize().x, 20, var.getName(),
														 m_container);
					edit->setCallback(boolSetter, var.getPointer());
					edit->setValue(*variable_cast<bool>(var));

					m_container->addWidget(edit);
				}
				break;

				//case M_VARIABLE_VECTOR2:
			default:
				{
					//auto edit = make_shared<Vector2Edit>(0, 0, getSize().x, 20,
					//									 shared_from_this());
					//addWidget(edit);
				}
				break;
			}
		}
	}

	m_container->updateLayout();
	m_currentObject = o;
	m_numBehaviors = o->getBehaviorsNumber();
}
