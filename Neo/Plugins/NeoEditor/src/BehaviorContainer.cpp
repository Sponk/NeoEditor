#include "BehaviorContainer.h"
#include <Label.h>
#include <EditField.h>
#include <CheckButton.h>
#include <Translator.h>
#include "VectorEdit.h"

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
		clear();
		m_numBehaviors = 0;
		m_currentObject = nullptr;
		return;
	}

	// Only update if it changed
	if(o == m_currentObject
	   && o->getBehaviorsNumber() == m_numBehaviors)
		return;

	clear();

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

	static auto behaviorDeleter = [this](Widget& w, void* d) {
		Behavior* b = static_cast<Behavior*>(d);
		Object3d* parent = b->getParentObject();

		// Find behavior ID
		for(int i = 0; i < parent->getBehaviorsNumber(); i++)
			if(parent->getBehavior(i) == b)
				parent->deleteBehavior(i);

		m_requestUpdate = true;
	};

	for(int i = 0; i < o->getBehaviorsNumber(); i++)
	{		
		Behavior* b = o->getBehavior(i);

		auto label = make_shared<Label>(0, 0, getSize().x, 10, b->getName(), shared_from_this());
		label->setColor(Vector4(0,0,0,1));
		addWidget(label);
		
		for(int j = 0; j < b->getVariablesNumber(); j++)
		{
			NeoVariable var = b->getVariable(j);

			if(var.getType() != M_VARIABLE_BOOL)
			{
				label = make_shared<Label>(0, 0, getSize().x, 10, var.getName(), shared_from_this());
				label->setColor(Vector4(0,0,0.5,1));
				addWidget(label);
			}
			
			switch(var.getType())
			{
			case M_VARIABLE_STRING:
				{
					auto edit = make_shared<EditField>(0, 0, getSize().x, 20,
													   variable_cast<Neo::String>(var)->getSafeString(), shared_from_this());
					edit->setCallback(stringSetter, var.getPointer());
					addWidget(edit);
				}
				break;

				case M_VARIABLE_FLOAT:
				{
					auto edit = make_shared<EditField>(0, 0, getSize().x, 20,
													   std::to_string(*variable_cast<float>(var)).c_str(), shared_from_this());
					edit->setCallback(floatSetter, var.getPointer());

					addWidget(edit);
				}
				break;

				case M_VARIABLE_VEC4:
				{
					auto edit = make_shared<Vector4Edit>(0, 0, getSize().x, 20, "", shared_from_this());
					edit->setCallback(vec4Setter, var.getPointer());
					edit->setVector(*variable_cast<Vector4>(var));

					addWidget(edit);
				}
				break;

				case M_VARIABLE_VEC3:
				{
					auto edit = make_shared<Vector3Edit>(0, 0, getSize().x, 20, "",
														 shared_from_this());
					edit->setCallback(vec3Setter, var.getPointer());
					edit->setVector(*variable_cast<Vector3>(var));

					addWidget(edit);
				}
				break;

				case M_VARIABLE_BOOL:
				{
					auto edit = make_shared<CheckButton>(0, 0, getSize().x, 20, var.getName(),
														 shared_from_this());
					edit->setCallback(boolSetter, var.getPointer());
					edit->setValue(*variable_cast<bool>(var));

					addWidget(edit);
				}
				break;

				default:
					MLOG_WARNING("Found unknown behavior variable " << var.getName() << " of type " << var.getType());
			}
		}

		auto removeButton = make_shared<Button>(0, 0, getSize().x, 20, tr("Delete"), shared_from_this());
		removeButton->setCallback(behaviorDeleter, b);
		addWidget(removeButton);
	}

	updateLayout();
	m_currentObject = o;
	m_numBehaviors = o->getBehaviorsNumber();
}

void BehaviorContainer::update(float dt)
{
	Container::update(dt);

	if(m_requestUpdate)
	{
		displayObject(m_currentObject);
		m_requestUpdate = false;
	}
}
