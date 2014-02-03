/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MaratisUI.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================
// Object-list by Mario Pispek, 2011


#ifdef _WIN32
#define SCRIPT_READER "SciTe.exe"
#elif __APPLE__
#define SCRIPT_READER "/Applications/TextEdit.app/Contents/MacOS/TextEdit"
#elif LINUX
#define SCRIPT_READER "gvim"
#endif


#include "MaratisUI.h"
#include <MGameWinEvents.h>

#include <MFileManager/MLevelSave.h>
#include <MFileManager/MLevelLoad.h>
#include <tinyxml.h>


MaratisUI::MaratisUI(void):
m_needToUpdateEdit(false),
m_play(false),
m_pause(true),
m_mouse(false),
m_position(true),
m_rotation(false),
m_scale(false),
m_worldMenu(0),
m_editedBehavior(0),
m_editedObject(NULL),
m_specialEvent(0),
m_guiColor(0.2f, 0.3f, 0.4f),
m_fileBrowser(NULL)
{
    loadSettings();
    createGUI();
	m_text = new MGuiText("", MVector2(), 16, MVector4(1, 1, 1, 0.5f));
	m_infoText = new MGuiText("", MVector2(), 16, MVector4(1, 1, 1, 0.8f));
	m_positionImage = new MGuiImage("gui/buttonPosition.tga", MVector2(), MVector2(10, 10), MVector4(1, 1, 1, 0.5f));
	m_rotationImage = new MGuiImage("gui/buttonRotation.tga", MVector2(), MVector2(10, 10), MVector4(1, 1, 1, 0.5f));
	m_scaleImage = new MGuiImage("gui/buttonScale.tga", MVector2(), MVector2(10, 10), MVector4(1, 1, 1, 0.5f));
}

MaratisUI::~MaratisUI(void)
{
	clear();
}

void MaratisUI::clear(void)
{
	SAFE_DELETE(m_text);
	SAFE_DELETE(m_infoText);
	SAFE_DELETE(m_positionImage);
	SAFE_DELETE(m_rotationImage);
	SAFE_DELETE(m_scaleImage);
	SAFE_DELETE(m_fileBrowser);
}

void MaratisUI::setSpecialEvent(int specialEvent, const char * info)
{
	m_specialEvent = specialEvent;
	m_specialEventInfo = info;
}

void MaratisUI::specialEvents(void)
{
	/*switch(m_specialEvent)
     {
     case 1:
     // do something
     m_specialEvent = 0;
     break;
     }*/
}

void MaratisUI::updateAnimIdText(MOEntity * entity)
{
	MMesh * mesh = entity->getMesh();
	if(mesh)
	{
		char text[64];
		sprintf(text, "%d/%d", entity->getAnimationId(), mesh->getAnimsRangesNumber() - 1);
		m_animIdText->setText(text);
		m_animIdText->autoScaleFromText();
		m_animIdText->setXPosition(69 - (m_animIdText->getScale().x * 0.5f));
	}
}

void MaratisUI::addVariableName(MGuiWindow * window, const char * variableName, MVector2 * position)
{
	float size = 16;
	float ySpace = 15;
	MVector4 variableColor(0.66f, 0.76f, 0.87f, 1.0f);

	MGuiText * text = new MGuiText(variableName, *position, size, variableColor);
	window->addText(text);
	*position += MVector2(0, ySpace);
}

void MaratisUI::addValue(MGuiWindow * window, const char * valueName, M_VAR_TYPE varType, void * pointer, MVector2 * position)
{
	float size = 16;
	float ySpace = 15;
	MVector4 color(1, 1, 1, 1);
	MVector4 valueColor(1.0f, 0.95f, 0.45f, 0.85f); // 1.0 0.97 0.33
	MVector4 variableColor(0.66f, 0.76f, 0.87f, 1.0f);

	if(varType == M_VAR_BOOL)
	{
		MGuiButton * button = new MGuiButton(*position + MVector2(28, 0), MVector2(16, 16), color, dVarButtonEvents);
		button->setMode(MGUI_BUTTON_CHECK);
		button->setNormalTexture("gui/button.tga");
		button->setHighLightColor(variableColor);
		button->setPressedColor(valueColor);
		button->enableVariable(pointer, varType);
		window->addButton(button);

		*position += MVector2(0, ySpace);
		return;
	}

	int x = 10;
	if(valueName)
	{
		MGuiText * text = new MGuiText(valueName, *position + MVector2(12, 0), size, valueColor);
		text->autoScaleFromText();
		window->addText(text);

        x += (int)text->getScale().x;
	}

	if(varType == M_VAR_NONE)
	{
		*position += MVector2(0, ySpace);
		return;
	}

	// value
	MGuiEditText * editText = new MGuiEditText("", *position + MVector2((float)x, 0.0f), size, color, editEvents);
	editText->setSingleLine(true);
	editText->enableVariable(pointer, varType);
	window->addEditText(editText);

	*position += MVector2(0, ySpace);
}

void MaratisUI::editProject(void)
{
	MWindow * window = MWindow::getInstance();

	m_editWin->clear();
	setNeedToUpdateEdit(false);

	float winHeight = (float)window->getHeight();
	float headHeight = m_headWin->getScale().y;
	float viewPropHeight = m_viewProp->getScale().y;

	MVector4 color(1, 1, 1, 1);
	MVector4 variableColor(0.66f, 0.76f, 0.87f, 1.0f);
	MVector4 highLightColor(0.8f, 0.9f, 1, 1);
	MVector4 menuColor(0.32f, 0.53f, 0.70f, 1.0f);
	MVector4 pressColor(1, 1, 0.5f, 1);

	MVector2 position = MVector2(15, 40);


	// init window
	m_cameraWin->setVisible(false);
	m_editWin->setYPosition(headHeight + viewPropHeight*2);
	m_editWin->setYScale(winHeight - m_editWin->getPosition().y);
	m_editTopWin->setYScale(viewPropHeight*2);

	m_editedObject = NULL;
	m_tabImage->setVisible(false);
	m_objectEdit->setVisible(false);
	m_cameraWin->setVisible(false);
	m_tabTransform->setVisible(false);
	m_tabProperties->setVisible(false);
	m_tabBehavior->setVisible(false);
    m_tabObjects->setVisible(false);
	m_sceneProperties->setVisible(false);

	m_tabImage->setVisible(true);
	m_projProperties->setVisible(true);
	m_tabText->setVisible(true);


	m_editWin->resizeScroll();
}

void MaratisUI::editScene(void)
{
	MWindow * window = MWindow::getInstance();

	m_editWin->clear();
	setNeedToUpdateEdit(false);

	float winHeight = (float)window->getHeight();
	float headHeight = m_headWin->getScale().y;
	float viewPropHeight = m_viewProp->getScale().y;

	float size = 16;
	float ySpace = 15;
	MVector4 color(1, 1, 1, 1);
	MVector4 variableColor(0.66f, 0.76f, 0.87f, 1.0f);
	MVector4 highLightColor(0.8f, 0.9f, 1, 1);
	MVector4 menuColor(0.32f, 0.53f, 0.70f, 1.0f);
	MVector4 pressColor(1, 1, 0.5f, 1);

	MVector2 position = MVector2(15, 40);


	// scene properties
	MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

	m_cameraWin->setVisible(false);
	m_editWin->setYPosition(headHeight + viewPropHeight*2);
	m_editWin->setYScale(winHeight - m_editWin->getPosition().y);
	m_editTopWin->setYScale(viewPropHeight*2);

	m_editedObject = NULL;
	m_objectListImage->setVisible(false);
	m_tabImage->setVisible(false);
	m_objectEdit->setVisible(false);
	m_cameraWin->setVisible(false);
	m_tabTransform->setVisible(false);
	m_tabProperties->setVisible(false);
	m_tabBehavior->setVisible(false);
    m_tabObjects->setVisible(false);
	m_projProperties->setVisible(false);
	m_tabText->setVisible(false);

	m_tabImage->setVisible(true);
	m_sceneProperties->setVisible(true);

	m_objectEdit->setVisible(true);
	m_objectEdit->setText(scene->getName());

	// script
	{
		// color
		MVector4 imgColor = MVector4(0, 0, 0, 0.2f);
		imgColor = MVector4(1, 0.97f, 0.33f, 0.2f);

		// script
		addVariableName(m_editWin, "script", &position);
		position += MVector2(0, 5);

		// text
		int textLength = 0;
		MGuiText * text;

		const char * script = scene->getScriptFilename();
		if(script)
		{
			char rep[256];
			char name[256];

			getRepertory(rep, script);
			getLocalFilename(name, rep, script);

			text = new MGuiText(name, MVector2(28, position.y), size, color);
			textLength = (int)text->getScale().x + 50 + 20;
		}
		else
		{
			text = new MGuiText("none", MVector2(28, position.y), size, color);
			textLength = (int)text->getScale().x + 50;
		}

		// image
		MGuiImage * image = new MGuiImage(NULL, MVector2(0, position.y-2), MVector2((float)textLength, 20), imgColor);
		m_editWin->addImage(image);

		// edit script
		{
			MGuiButton * button = new MGuiButton(MVector2(10, position.y-2), MVector2((float)textLength - 50, 20), MVector4(0.0f, 0.0f, 0.0f, 0.0f), editScriptEvents);
			m_editWin->addButton(button);
		}

		image = new MGuiImage("gui/imgCapsuleRight.tga", MVector2((float)textLength, position.y - 8), MVector2(8, 32), imgColor);
		m_editWin->addImage(image);

		// add text
		m_editWin->addText(text);

		// delete script
		float x = textLength - 14.0f;
		if(script)
		{
			MGuiButton * button = new MGuiButton(MVector2(x, position.y), MVector2(16, 16), color, deleteScriptEvents);
			button->setHighLightColor(highLightColor);
			button->setPressedColor(pressColor);
			button->setNormalTexture("gui/buttonDelete.tga");
			m_editWin->addButton(button);
			x -= 20;
		}

		// open script
		MGuiButton * button = new MGuiButton(MVector2(x, position.y-2), MVector2(16, 16), color, loadScriptEvents);
		button->setHighLightColor(highLightColor);
		button->setPressedColor(pressColor);
		button->setNormalTexture("gui/buttonOpen.tga");
		m_editWin->addButton(button);

		position += MVector2(0, ySpace*3);
	}

	// dataMode
	addVariableName(m_editWin, "data", &position);

	MGuiMenu * dataMenu = new MGuiMenu(position + MVector2(28, 0), MVector2(16, 16), MVector4(1, 1, 1, 0), dataMenuEvents);

	dataMenu->setHighLightColor(MVector4(0.8f, 0.9f, 1, 0.2f));
	dataMenu->setTextColor(MVector4(1, 1, 1, 1));

	dataMenu->addSimpleButton("Static", NULL);
	dataMenu->addSimpleButton("Dynamic", NULL);
	//dataMenu->addSimpleButton("Stream", NULL);

	dataMenu->setMenuId((unsigned int)scene->getDataMode());
	dataMenu->getWindowMenu()->setShadow(true);
	dataMenu->getWindowMenu()->setNormalColor(menuColor);

	m_editWin->addMenu(dataMenu);
	position += MVector2(0, ySpace*2);

	// gravity
	m_editGravity = scene->getGravity();
	addVariableName(m_editWin, "gravity", &position);
	addValue(m_editWin, "x", M_VAR_FLOAT, &m_editGravity.x, &position);
	addValue(m_editWin, "y", M_VAR_FLOAT, &m_editGravity.y, &position);
	addValue(m_editWin, "z", M_VAR_FLOAT, &m_editGravity.z, &position);
	position += MVector2(0, ySpace);

	m_editWin->resizeScroll();
}

void MaratisUI::editObject(MObject3d * object)
{
	MWindow * window = MWindow::getInstance();

	m_editWin->clear();
	setNeedToUpdateEdit(false);

	float winHeight = (float)window->getHeight();
	float headHeight = m_headWin->getScale().y;
	float viewPropHeight = m_viewProp->getScale().y;

	float size = 16;
	float ySpace = 15;
	MVector4 color(1, 1, 1, 1);
	MVector4 variableColor(0.66f, 0.76f, 0.87f, 1.0f);
	MVector4 highLightColor(0.8f, 0.9f, 1, 1);
	MVector4 menuColor(0.32f, 0.53f, 0.70f, 1.0f);
	MVector4 pressColor(1, 1, 0.5f, 1);

	MVector2 position = MVector2(15, 40);

	// other properties
	m_objectListImage->setVisible(false);
	m_sceneProperties->setVisible(false);
	m_projProperties->setVisible(false);
	m_objectEdit->setVisible(false);
	m_tabText->setVisible(false);

    // should always be visible
    m_tabImage->setVisible(true);

	// NULL
    if(! object)
    {
        m_editedObject = NULL;
        m_objectEdit->setVisible(false);
        m_cameraWin->setVisible(false);
        m_tabTransform->setVisible(false);
        m_tabProperties->setVisible(false);
        m_tabBehavior->setVisible(false);
		m_tabObjects->setVisible(false);

        //m_tabObjects->unPressGroupButtons();
        //m_tabObjects->setPressed(true);
		m_tabText->setVisible(true);
		m_objectListImage->setVisible(true);

        m_cameraWin->setVisible(false);
        m_editWin->setYPosition(headHeight + viewPropHeight*2);
        m_editWin->setYScale(winHeight - m_editWin->getPosition().y);
        m_editTopWin->setYScale(viewPropHeight*2);
    }
    else
    {
        // object properties
        if(object != m_editedObject)
        {
            m_editedBehavior = object->getBehaviorsNumber();
            m_editedObject = object;
        }

		if(m_tabObjects->isPressed())
		{
			// object-list title
			m_tabText->setVisible(true);
		}
		else
		{
			// object name
			const char * name = object->getName();
			if(name)
				m_objectEdit->setText(name);

			m_objectEdit->setVisible(true);
		}

		// tabs
        m_tabTransform->setVisible(true);
        m_tabProperties->setVisible(true);
        m_tabBehavior->setVisible(true);
		m_tabObjects->setVisible(true);


        if(object->getType() == M_OBJECT3D_CAMERA && !m_tabObjects->isPressed())
        {
            int previewHeight = 230;

            m_cameraWin->setVisible(true);
            m_editWin->setYPosition(headHeight + viewPropHeight*2 + previewHeight);
            m_editWin->setYScale(winHeight - m_editWin->getPosition().y);
            m_editTopWin->setYScale(viewPropHeight*2 + previewHeight);

            m_cameraWin->setNormalColor(((MOCamera*)object)->getClearColor());
        }
        else
        {
            m_cameraWin->setVisible(false);
            m_editWin->setYPosition(headHeight + viewPropHeight*2);
            m_editWin->setYScale(winHeight - m_editWin->getPosition().y);
            m_editTopWin->setYScale(viewPropHeight*2);
        }


		// transform
		if(m_tabTransform->isPressed())
		{
			// parent
			MObject3d *parent = object->getParent();
			m_editParent = parent == NULL ? "..." : parent->getName();
			addVariableName(m_editWin, "parent", &position);
			addValue(m_editWin, "", M_VAR_STRING, &m_editParent, &position);
			position += MVector2(0, ySpace);

			m_editPosition = object->getPosition();
			addVariableName(m_editWin, "position", &position);
			addValue(m_editWin, "x", M_VAR_FLOAT, &m_editPosition.x, &position);
			addValue(m_editWin, "y", M_VAR_FLOAT, &m_editPosition.y, &position);
			addValue(m_editWin, "z", M_VAR_FLOAT, &m_editPosition.z, &position);
			position += MVector2(0, ySpace);

			m_editRotation = object->getEulerRotation();
			addVariableName(m_editWin, "rotation", &position);
			addValue(m_editWin, "x", M_VAR_FLOAT, &m_editRotation.x, &position);
			addValue(m_editWin, "y", M_VAR_FLOAT, &m_editRotation.y, &position);
			addValue(m_editWin, "z", M_VAR_FLOAT, &m_editRotation.z, &position);
			position += MVector2(0, ySpace);

			m_editScale = object->getScale();
			addVariableName(m_editWin, "scale", &position);
			addValue(m_editWin, "x", M_VAR_FLOAT, &m_editScale.x, &position);
			addValue(m_editWin, "y", M_VAR_FLOAT, &m_editScale.y, &position);
			addValue(m_editWin, "z", M_VAR_FLOAT, &m_editScale.z, &position);
			position += MVector2(0, ySpace);

			m_editWin->resizeScroll();
			return;
		}

		if(m_tabProperties->isPressed())
		{
			switch(object->getType())
			{
				case M_OBJECT3D_ENTITY:
				{
					MOEntity * entity = (MOEntity *)object;
					MMeshRef * meshRef = entity->getMeshRef();
					MMesh * mesh = meshRef->getMesh();

					// data ref
					if(meshRef)
					{
						char rep[256];
						char name[256];
						getRepertory(rep, meshRef->getFilename());
						getLocalFilename(name, rep, meshRef->getFilename());

						addVariableName(m_editWin, "Mesh", &position);
						addValue(m_editWin, name, M_VAR_NONE, NULL, &position);

						position += MVector2(0, ySpace);
					}

					// anims
					if(mesh)
					{
						if(mesh->getAnimsRangesNumber() > 0)
						{
							// text
							MGuiText * text = new MGuiText("animation", position, size, variableColor);
							m_editWin->addText(text);
							position += MVector2(0, ySpace);

							// image
							MGuiImage * image = new MGuiImage("gui/imgAnim.tga", MVector2(5, position.y), MVector2(128, 32), MVector4(0, 0, 0, 0.2f));
							m_editWin->addImage(image);

							// left button
							MGuiButton * button = new MGuiButton(MVector2(21, position.y + 8), MVector2(16, 16), color, leftAnimIdEvents);
							button->setHighLightColor(highLightColor);
							button->setPressedColor(pressColor);
							button->setNormalTexture("gui/buttonLeft.tga");
							m_editWin->addButton(button);

							// right button
							button = new MGuiButton(MVector2(101, position.y + 8), MVector2(16, 16), color, rightAnimIdEvents);
							button->setHighLightColor(highLightColor);
							button->setPressedColor(pressColor);
							button->setNormalTexture("gui/buttonRight.tga");
							m_editWin->addButton(button);

							// id
							m_animIdText = new MGuiText("", MVector2(40, position.y + 8), size, color);
							updateAnimIdText(entity);
							m_editWin->addText(m_animIdText);

							position += MVector2(0, ySpace*3);
						}
						else
						{
							MVector4 grayColor(0.5f, 0.5f, 0.5f, 1);

							// text
							MGuiText * text = new MGuiText("animation", position, size, grayColor);
							m_editWin->addText(text);
							position += MVector2(0, ySpace);

							// image
							MGuiImage * image = new MGuiImage("gui/imgAnim.tga", MVector2(5, position.y), MVector2(128, 32), MVector4(0.5f, 0.5f, 0.5f, 0.2f));
							m_editWin->addImage(image);

							// left button
							image = new MGuiImage("gui/buttonLeft.tga", MVector2(21, position.y + 8), MVector2(16, 16), grayColor);
							m_editWin->addImage(image);

							// right button
							image = new MGuiImage("gui/buttonRight.tga", MVector2(101, position.y + 8), MVector2(16, 16), grayColor);
							m_editWin->addImage(image);

							// id
							m_animIdText = new MGuiText("0/0", MVector2(53, position.y + 8), size, grayColor);
							m_editWin->addText(m_animIdText);

							position += MVector2(0, ySpace*3);
						}
					}

					// invisible
					m_editInvisible = entity->isInvisible();
					addVariableName(m_editWin, "invisible", &position);
					addValue(m_editWin, "", M_VAR_BOOL, &m_editInvisible, &position);
					position += MVector2(0, ySpace);

					// physics
					{
						MPhysicsProperties * phyProps = entity->getPhysicsProperties();

						m_editPhysics = (phyProps != NULL);
						addVariableName(m_editWin, "Physics", &position);
						addValue(m_editWin, "", M_VAR_BOOL, &m_editPhysics, &position);
						position += MVector2(0, ySpace);

						if(phyProps)
						{
							// shape
							MGuiText * text = new MGuiText("shape", position, size, variableColor);
							m_editWin->addText(text);
							position += MVector2(0, ySpace);

							MGuiMenu * menu = new MGuiMenu(MVector2(position.x+10, position.y), MVector2(10, 10), MVector4(1, 1, 1, 0), collisionShapeEvents);
							menu->setHighLightColor(MVector4(0.8f, 0.9f, 1, 0.2f));
							menu->setTextColor(MVector4(1, 1, 1, 1));
							menu->addSimpleButton(" Box ", NULL);
							menu->addSimpleButton(" Sphere ", NULL);
							menu->addSimpleButton(" Cone ", NULL);
							menu->addSimpleButton(" Capsule ", NULL);
							menu->addSimpleButton(" Cylinder ", NULL);
							menu->addSimpleButton(" Convex-hull ", NULL);
							menu->addSimpleButton(" Triangle-mesh ", NULL);
							menu->getWindowMenu()->setShadow(true);
							menu->getWindowMenu()->setNormalColor(menuColor);
							m_editWin->addMenu(menu);

							menu->setMenuId((int)phyProps->getCollisionShape());
							position += MVector2(0, ySpace*2);

							// ghost
							m_editGhost = phyProps->isGhost();
							addVariableName(m_editWin, "ghost", &position);
							addValue(m_editWin, "", M_VAR_BOOL, &m_editGhost, &position);
							position += MVector2(0, ySpace);

							if(! m_editGhost)
							{
								m_editMass = phyProps->getMass();
								m_editFriction = phyProps->getFriction();
								m_editRestitution = phyProps->getRestitution();

								addVariableName(m_editWin, "properties", &position);
								addValue(m_editWin, "mass", M_VAR_FLOAT, &m_editMass, &position);
								addValue(m_editWin, "fric", M_VAR_FLOAT, &m_editFriction, &position);
								addValue(m_editWin, "rest", M_VAR_FLOAT, &m_editRestitution, &position);

								position += MVector2(0, ySpace);

								// damping
								m_editLinearDamping = phyProps->getLinearDamping();
								m_editAngularDamping = phyProps->getAngularDamping();
								addVariableName(m_editWin, "damping", &position);
								addValue(m_editWin, "lin", M_VAR_FLOAT, &m_editLinearDamping, &position);
								addValue(m_editWin, "ang", M_VAR_FLOAT, &m_editAngularDamping, &position);
								position += MVector2(0, ySpace);

								// angular factor
								m_editAngularFactor = phyProps->getAngularFactor();
								addVariableName(m_editWin, "angularFactor", &position);
								addValue(m_editWin, "", M_VAR_FLOAT, &m_editAngularFactor, &position);
								position += MVector2(0, ySpace);

								// linear factor
								MVector3 * linearFactor = phyProps->getLinearFactor();
								addVariableName(m_editWin, "linearFactor", &position);
								addValue(m_editWin, "x", M_VAR_FLOAT, &linearFactor->x, &position);
								addValue(m_editWin, "y", M_VAR_FLOAT, &linearFactor->y, &position);
								addValue(m_editWin, "z", M_VAR_FLOAT, &linearFactor->z, &position);
								position += MVector2(0, ySpace);

								// constraint
								{
									MPhysicsConstraint * constraint = phyProps->getConstraint();

									m_editConstraint = (constraint != NULL);
									addVariableName(m_editWin, "Constraint", &position);
									addValue(m_editWin, "", M_VAR_BOOL, &m_editConstraint, &position);
									position += MVector2(0, ySpace);

									if(constraint)
									{
										MVector3 pivot;
										MVector3 lowerLinearLimit;
										MVector3 upperLinearLimit;
										MVector3 lowerAngularLimit;
										MVector3 upperAngularLimit;

										// parent
										addVariableName(m_editWin, "parent", &position);
										addValue(m_editWin, "", M_VAR_STRING, &constraint->parentName, &position);
										position += MVector2(0, ySpace);

										// pivot
										addVariableName(m_editWin, "pivot", &position);
										addValue(m_editWin, "x", M_VAR_FLOAT, &constraint->pivot.x, &position);
										addValue(m_editWin, "y", M_VAR_FLOAT, &constraint->pivot.y, &position);
										addValue(m_editWin, "z", M_VAR_FLOAT, &constraint->pivot.z, &position);
										position += MVector2(0, ySpace);

										// linearLimits
										addVariableName(m_editWin, "linearLimits", &position);
										addValue(m_editWin, "loX", M_VAR_FLOAT, &constraint->lowerLinearLimit.x, &position);
										addValue(m_editWin, "loY", M_VAR_FLOAT, &constraint->lowerLinearLimit.y, &position);
										addValue(m_editWin, "loZ", M_VAR_FLOAT, &constraint->lowerLinearLimit.z, &position);
										addValue(m_editWin, "upX", M_VAR_FLOAT, &constraint->upperLinearLimit.x, &position);
										addValue(m_editWin, "upY", M_VAR_FLOAT, &constraint->upperLinearLimit.y, &position);
										addValue(m_editWin, "upZ", M_VAR_FLOAT, &constraint->upperLinearLimit.z, &position);
										position += MVector2(0, ySpace);

										// angularLimits
										addVariableName(m_editWin, "angularLimits", &position);
										addValue(m_editWin, "loX", M_VAR_FLOAT, &constraint->lowerAngularLimit.x, &position);
										addValue(m_editWin, "loY", M_VAR_FLOAT, &constraint->lowerAngularLimit.y, &position);
										addValue(m_editWin, "loZ", M_VAR_FLOAT, &constraint->lowerAngularLimit.z, &position);
										addValue(m_editWin, "upX", M_VAR_FLOAT, &constraint->upperAngularLimit.x, &position);
										addValue(m_editWin, "upY", M_VAR_FLOAT, &constraint->upperAngularLimit.y, &position);
										addValue(m_editWin, "upZ", M_VAR_FLOAT, &constraint->upperAngularLimit.z, &position);
										position += MVector2(0, ySpace);

										// disableParentCollision
										addVariableName(m_editWin, "disableParentCollision", &position);
										addValue(m_editWin, "", M_VAR_BOOL, &constraint->disableParentCollision, &position);
										position += MVector2(0, ySpace);
									}
								}
							}
						}
					}
				}
					break;
				case M_OBJECT3D_LIGHT:
				{
					MOLight * light = (MOLight *)object;

					m_editRadius = light->getRadius();
					addVariableName(m_editWin, "radius", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editRadius, &position);
					position += MVector2(0, ySpace);

					m_editLightColor = light->getColor();
					addVariableName(m_editWin, "color", &position);
					addValue(m_editWin, "r", M_VAR_FLOAT, &m_editLightColor.x, &position);
					addValue(m_editWin, "v", M_VAR_FLOAT, &m_editLightColor.y, &position);
					addValue(m_editWin, "b", M_VAR_FLOAT, &m_editLightColor.z, &position);
					position += MVector2(0, ySpace);

					m_editIntensity = light->getIntensity();
					addVariableName(m_editWin, "intensity", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editIntensity, &position);
					position += MVector2(0, ySpace);

					m_editSpot = (light->getSpotAngle() >= 1.0f && light->getSpotAngle() <= 90.0f);
					addVariableName(m_editWin, "spot", &position);
					addValue(m_editWin, "", M_VAR_BOOL, &m_editSpot, &position);
					position += MVector2(0, ySpace);

					if(m_editSpot)
					{
						m_editSpotAngle = light->getSpotAngle();
						addVariableName(m_editWin, "spotAngle", &position);
						addValue(m_editWin, "", M_VAR_FLOAT, &m_editSpotAngle, &position);
						position += MVector2(0, ySpace);

						m_editSpotExponent = light->getSpotExponent();
						addVariableName(m_editWin, "spotExponent", &position);
						addValue(m_editWin, "", M_VAR_FLOAT, &m_editSpotExponent, &position);
						position += MVector2(0, ySpace);

						m_editShadow = light->isCastingShadow();
						addVariableName(m_editWin, "shadow", &position);
						addValue(m_editWin, "", M_VAR_BOOL, &m_editShadow, &position);
						position += MVector2(0, ySpace);

						if(m_editShadow)
						{
							m_editShadowBias = light->getShadowBias();
							addVariableName(m_editWin, "shadowBias", &position);
							addValue(m_editWin, "", M_VAR_FLOAT, &m_editShadowBias, &position);
							position += MVector2(0, ySpace);

							m_editShadowBlur = light->getShadowBlur();
							addVariableName(m_editWin, "shadowBlur", &position);
							addValue(m_editWin, "", M_VAR_FLOAT, &m_editShadowBlur, &position);
							position += MVector2(0, ySpace);

							m_editShadowQuality = light->getShadowQuality();
							addVariableName(m_editWin, "shadowQuality", &position);
							addValue(m_editWin, "", M_VAR_UINT, &m_editShadowQuality, &position);
							position += MVector2(0, ySpace);
						}
					}
				}
					break;
				case M_OBJECT3D_CAMERA:
				{
					MOCamera * camera = (MOCamera *)object;

					m_editCameraColor = camera->getClearColor();
					addVariableName(m_editWin, "clearColor", &position);
					addValue(m_editWin, "r", M_VAR_FLOAT, &m_editCameraColor.x, &position);
					addValue(m_editWin, "v", M_VAR_FLOAT, &m_editCameraColor.y, &position);
					addValue(m_editWin, "b", M_VAR_FLOAT, &m_editCameraColor.z, &position);
					position += MVector2(0, ySpace);

					m_editOrtho = camera->isOrtho();
					addVariableName(m_editWin, "ortho", &position);
					addValue(m_editWin, "", M_VAR_BOOL, &m_editOrtho, &position);
					position += MVector2(0, ySpace);

					m_editFov = camera->getFov();
					addVariableName(m_editWin, "fov", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editFov, &position);
					position += MVector2(0, ySpace);

					m_editClipNear = camera->getClippingNear();
					addVariableName(m_editWin, "clippingNear", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editClipNear, &position);
					position += MVector2(0, ySpace);

					m_editClipFar = camera->getClippingFar();
					addVariableName(m_editWin, "clippingFar", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editClipFar, &position);
					position += MVector2(0, ySpace);

					m_editFog = camera->hasFog();
					addVariableName(m_editWin, "fog", &position);
					addValue(m_editWin, "", M_VAR_BOOL, &m_editFog, &position);
					position += MVector2(0, ySpace);

					m_editFogDistance = camera->getFogDistance();
					addVariableName(m_editWin, "fogDistance", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editFogDistance, &position);
					position += MVector2(0, ySpace);
				}
					break;
				case M_OBJECT3D_SOUND:
				{
					MOSound * sound = (MOSound *)object;
					MSoundRef * soundRef = sound->getSoundRef();

					// data ref
					if(soundRef)
					{
						char rep[256];
						char name[256];
						getRepertory(rep, soundRef->getFilename());
						getLocalFilename(name, rep, soundRef->getFilename());

						addVariableName(m_editWin, "Sound", &position);
						addValue(m_editWin, name, M_VAR_NONE, NULL, &position);

						position += MVector2(0, ySpace);
					}

					m_editLoop = sound->isLooping();
					addVariableName(m_editWin, "loop", &position);
					addValue(m_editWin, "", M_VAR_BOOL, &m_editLoop, &position);
					position += MVector2(0, ySpace);

					m_editPitch = sound->getPitch();
					addVariableName(m_editWin, "pitch", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editPitch, &position);
					position += MVector2(0, ySpace);

					m_editGain = sound->getGain();
					addVariableName(m_editWin, "gain", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editGain, &position);
					position += MVector2(0, ySpace);

					m_editSndRadius = sound->getRadius();
					addVariableName(m_editWin, "radius", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editSndRadius, &position);
					position += MVector2(0, ySpace);

					m_editRolloff = sound->getRolloff();
					addVariableName(m_editWin, "rolloff", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editRolloff, &position);
					position += MVector2(0, ySpace);

					m_editRelative = sound->isRelative();
					addVariableName(m_editWin, "relative", &position);
					addValue(m_editWin, "", M_VAR_BOOL, &m_editRelative, &position);
					position += MVector2(0, ySpace);
				}
					break;
				case M_OBJECT3D_TEXT:
				{
					MEngine * engine = MEngine::getInstance();
					MLevel * level = engine->getLevel();
					MDataManager * fontManager = level->getFontManager();
					MOText * text = (MOText *)object;

					int textLength = 0;
					MGuiText * guiText = NULL;

					addVariableName(m_editWin, "font", &position);
					position += MVector2(0, 4);

					// font
					MFont * font = text->getFont();
					if(font)
					{
						const char * fontFilename = NULL;
						unsigned int r;
						unsigned int rSize = fontManager->getRefsNumber();

						for(r=0; r<rSize; r++)
						{
							MFontRef * ref = (MFontRef*)fontManager->getRef(r);
							if(ref->getFont() == font){
								fontFilename = ref->getFilename();
								break;
							}

						}

						if(fontFilename)
						{
							char rep[256];
							char name[256];

							getRepertory(rep, fontFilename);
							getLocalFilename(name, rep, fontFilename);

							guiText = new MGuiText(name, MVector2(28, position.y), size, color);
						}
					}

					if(! guiText)
						guiText = new MGuiText("none", MVector2(28, position.y), size, color);

					textLength = (int)guiText->getScale().x + 50 + 20;
					float x = textLength - 14.0f;

					// image
					MVector4 imgColor = MVector4(1, 0.97f, 0.33f, 0.2f);
					MGuiImage * image = new MGuiImage(NULL, MVector2(0, position.y-2), MVector2((float)textLength, 20), imgColor);
					m_editWin->addImage(image);

					image = new MGuiImage("gui/imgCapsuleRight.tga", MVector2((float)textLength, position.y - 8), MVector2(8, 32), imgColor);
					m_editWin->addImage(image);

					// add text
					m_editWin->addText(guiText);

					// open font
					MGuiButton * button = new MGuiButton(MVector2(x, position.y-2), MVector2(16, 16), color, loadFontEvents);
					button->setHighLightColor(highLightColor);
					button->setNormalTexture("gui/buttonOpen.tga");
					button->setPressedColor(pressColor);
					m_editWin->addButton(button);

					position += MVector2(0, ySpace*2);

					// size
					m_editSize = text->getSize();
					addVariableName(m_editWin, "size", &position);
					addValue(m_editWin, "", M_VAR_FLOAT, &m_editSize, &position);
					position += MVector2(0, ySpace);

					// align
					addVariableName(m_editWin, "align", &position);

					MGuiMenu * alignMenu = new MGuiMenu(position + MVector2(28, 0), MVector2(16, 16), MVector4(1, 1, 1, 0), textAlignMenuEvents);

					alignMenu->setHighLightColor(MVector4(0.8f, 0.9f, 1, 0.2f));
					alignMenu->setTextColor(MVector4(1, 1, 1, 1));

					alignMenu->addSimpleButton("Left", NULL);
					alignMenu->addSimpleButton("Right", NULL);
					alignMenu->addSimpleButton("Center", NULL);

					alignMenu->setMenuId((unsigned int)text->getAlign());
					alignMenu->getWindowMenu()->setShadow(true);
					alignMenu->getWindowMenu()->setNormalColor(menuColor);

					m_editWin->addMenu(alignMenu);
					position += MVector2(0, ySpace*2);

					// color
					m_editTextColor = text->getColor();
					addVariableName(m_editWin, "color", &position);
					addValue(m_editWin, "r", M_VAR_FLOAT, &m_editTextColor.x, &position);
					addValue(m_editWin, "v", M_VAR_FLOAT, &m_editTextColor.y, &position);
					addValue(m_editWin, "b", M_VAR_FLOAT, &m_editTextColor.z, &position);
					addValue(m_editWin, "a", M_VAR_FLOAT, &m_editTextColor.w, &position);
					position += MVector2(0, ySpace);

					//text
					m_editText = text->getText();
					addVariableName(m_editWin, "text", &position);

					//addValue(m_editWin, "", M_VAR_STRING, &m_editText, &position);
					MGuiEditText * editText = new MGuiEditText("", position + MVector2(24, 0), size, color, editEvents);
					editText->enableVariable(&m_editText, M_VAR_STRING);
					m_editWin->addEditText(editText);
					position += MVector2(0, ySpace);

					position += MVector2(0, ySpace);

				}
					break;
			}
			m_editWin->resizeScroll();
			return;
		}

		if(m_tabBehavior->isPressed())
		{
			// behaviors
			unsigned int i;
			unsigned int bSize = object->getBehaviorsNumber();
			for(i=0; i<bSize; i++)
			{
				// color
				MVector4 imgColor = MVector4(0, 0, 0, 0.2f);
				if(i+1 == m_editedBehavior)
					imgColor = MVector4(1, 0.97f, 0.33f, 0.2f);

				// behavior
				MBehavior * behavior = object->getBehavior(i);

				// image
				MGuiImage * image = new MGuiImage("gui/imgBehavior.tga", MVector2(0, position.y - 8), MVector2(256, 32), imgColor);
				m_editWin->addImage(image);

				// text
				string name("- ");
				name += behavior->getName();
				MGuiText * text = new MGuiText(name.c_str(), position, size, color);
				m_editWin->addText(text);

				// current behavior
				if(i+1 == m_editedBehavior)
				{
					// buttons
					int x = 218;
					MGuiButton * button;

					if((i + 1) != bSize)
					{
						button = new MGuiButton(MVector2(218, position.y + 4), MVector2(8, 8), color, downBehaviorEvents);
						button->setHighLightColor(highLightColor);
						button->setPressedColor(pressColor);
						button->setNormalTexture("gui/buttonDown.tga");
						m_editWin->addButton(button);
						x -= 20;
					}

					if(i != 0)
					{
						button = new MGuiButton(MVector2((float)x, position.y + 4), MVector2(8, 8), color, upBehaviorEvents);
						button->setHighLightColor(highLightColor);
						button->setPressedColor(pressColor);
						button->setNormalTexture("gui/buttonUp.tga");
						m_editWin->addButton(button);
					}

					button = new MGuiButton(MVector2(233, position.y), MVector2(16, 16), color, deleteBehaviorEvents);
					button->setHighLightColor(highLightColor);
					button->setPressedColor(pressColor);
					button->setNormalTexture("gui/buttonDelete.tga");
					m_editWin->addButton(button);

					// behavior properties
					position += MVector2(0, 30);

					unsigned int v;
					unsigned int vSize = behavior->getVariablesNumber();
					for(v=0; v<vSize; v++)
					{
						MVariable variable = behavior->getVariable(v);
						M_VARIABLE_TYPE varType = variable.getType();

						if(varType == M_VARIABLE_NULL)
							continue;

						addVariableName(m_editWin, variable.getName(), &position);

						switch(varType)
						{
							default:
								break;

							case M_VARIABLE_BOOL:
								addValue(m_editWin, "", M_VAR_BOOL, variable.getPointer(), &position);
								position += MVector2(0, ySpace);
								break;

							case M_VARIABLE_INT:
								addValue(m_editWin, "", M_VAR_INT, variable.getPointer(), &position);
								position += MVector2(0, ySpace);
								break;

							case M_VARIABLE_UINT:
								addValue(m_editWin, "", M_VAR_UINT, variable.getPointer(), &position);
								position += MVector2(0, ySpace);
								break;

							case M_VARIABLE_FLOAT:
								addValue(m_editWin, "", M_VAR_FLOAT, variable.getPointer(), &position);
								position += MVector2(0, ySpace);
								break;

							case M_VARIABLE_STRING:
								addValue(m_editWin, "", M_VAR_STRING, variable.getPointer(), &position);
								position += MVector2(0, ySpace);
								break;

							case M_VARIABLE_VEC2:
								addValue(m_editWin, "x", M_VAR_FLOAT, &((MVector2*)variable.getPointer())->x, &position);
								position += MVector2(0, ySpace);
								addValue(m_editWin, "y", M_VAR_FLOAT, &((MVector2*)variable.getPointer())->x, &position);
								position += MVector2(0, ySpace);
								break;

							case M_VARIABLE_VEC3:
								addValue(m_editWin, "x", M_VAR_FLOAT, &((MVector3*)variable.getPointer())->x, &position);
								addValue(m_editWin, "y", M_VAR_FLOAT, &((MVector3*)variable.getPointer())->y, &position);
								addValue(m_editWin, "z", M_VAR_FLOAT, &((MVector3*)variable.getPointer())->z, &position);
								position += MVector2(0, ySpace);
								break;

							case M_VARIABLE_VEC4:
								addValue(m_editWin, "x", M_VAR_FLOAT, &((MVector4*)variable.getPointer())->x, &position);
								addValue(m_editWin, "y", M_VAR_FLOAT, &((MVector4*)variable.getPointer())->y, &position);
								addValue(m_editWin, "z", M_VAR_FLOAT, &((MVector4*)variable.getPointer())->z, &position);
								addValue(m_editWin, "w", M_VAR_FLOAT, &((MVector4*)variable.getPointer())->w, &position);
								position += MVector2(0, ySpace);
								break;

							case M_VARIABLE_TEXTURE_REF:
							{
								MTextureRef** textureRef = (MTextureRef**)variable.getPointer();

								button = new MGuiButton(position + MVector2(17, 4), MVector2(16, 16), MVector4(1), loadTextureEvents);
								button->setNormalTexture("gui/buttonOpen.tga");
								button->setHighLightColor(highLightColor);
								button->enableVariable(variable.getPointer(), M_VAR_NONE);
								m_editWin->addButton(button);

								if(*textureRef)
								{
									char path[256] = "";
									char name[64] = "";

									const char * filename = (*textureRef)->getFilename();
									getRepertory(path, filename);
									getLocalFilename(name, path, filename);

									// texture file
									MVector2 pos = position + MVector2(40, 5);
									text = new MGuiText(name, pos, 16, MVector4(1, 1, 0, 1));
									m_editWin->addText(text);
								}

								position += MVector2(0, 24);
								position += MVector2(0, ySpace);
								break;
							}
						}
					}

					position += MVector2(0, 30);
				}
				else
				{
					position += MVector2(0, 24);
				}
			}

			// behaviors menu
			MGuiImage * behaviorImage = new MGuiImage("gui/imgAddBehavior.tga", MVector2(0, position.y - 8), MVector2(32, 32), MVector4(0, 0, 0, 0.2f));

			MBehaviorManager * bManager = MEngine::getInstance()->getBehaviorManager();
			MGuiMenu * behaviorMenu = new MGuiMenu(position, MVector2(16, 16), color, behaviorMenuEvents);

			behaviorMenu->setNormalTexture("gui/buttonSup.tga");
			behaviorMenu->setDrawingText(false);

			unsigned int y = 10;

			bSize = bManager->getBehaviorsNumber();
			for(i=0; i<bSize; i++)
			{
				MBehaviorCreator * behavior = bManager->getBehaviorByIndex(i);
				if((behavior->getObjectFilter() != M_OBJECT3D) && (behavior->getObjectFilter() != object->getType()))
					continue;

				addTextButtonToMenu(behaviorMenu, behavior->getName(), MVector2(0, (float)y), NULL);
				y += 20;
			}

			addTextToMenu(behaviorMenu, "", MVector2(0, (float)y-10));

			behaviorMenu->setMenuId(bSize);
			behaviorMenu->getWindowMenu()->setShadow(true);
			behaviorMenu->getWindowMenu()->setNormalColor(menuColor);

			m_editWin->addImage(behaviorImage);
			m_editWin->addMenu(behaviorMenu);
			m_editWin->resizeScroll();
			return;
		}
	}

    if(m_tabObjects->isPressed() || (! object))
	{
        MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();
        Maratis * maratis = Maratis::getInstance();
        MObject3d * objectAtButton;
        int camerasNumber = scene->getCamerasNumber();
        int entitiesNumber = scene->getEntitiesNumber();
        int lightsNumber = scene->getLightsNumber();
        int soundsNumber = scene->getSoundsNumber();
        int textsNumber = scene->getTextsNumber();
        const char * name;

        if (camerasNumber > 0)
        {
            addVariableName(m_editWin, "cameras", &position);
            MVector2 buttonPosition = position + MVector2(12, 5);
            for (int i = 0; i < camerasNumber; i++)
            {
                objectAtButton = scene->getCameraByIndex(i);
                name = objectAtButton->getName();
                MGuiButton *objectButton = new MGuiButton(
                                                          buttonPosition,
                                                          MVector2(48, 20),
                                                          MVector4(1, 1, 1, 1),
                                                          &objectButtonEvents);
                objectButton->setMode(MGUI_BUTTON_SIMPLE);
                objectButton->setText(name);
                objectButton->setNormalColor(MVector4(0, 0, 0, 0));
                objectButton->setHighLightColor(MVector4(0.30, 0.48, 0.62, 1.0));
                if (maratis->isObjectSelected(objectAtButton))
                    objectButton->setTextColor(MVector4(1, 1, 0, 1));
                else
                    objectButton->setTextColor(MVector4(1, 1, 1, 1));
                m_editWin->addButton(objectButton);
                buttonPosition += MVector2(0, ySpace+1);
            }
            position =+ MVector2(15, buttonPosition.y + ySpace);
        }

        if (entitiesNumber > 0)
        {
            addVariableName(m_editWin, "entities", &position);
            MVector2 buttonPosition = position + MVector2(12, 5);
            for (int i = 0; i < entitiesNumber; i++)
            {
                objectAtButton = scene->getEntityByIndex(i);
                name = objectAtButton->getName();
                MGuiButton *objectButton = new MGuiButton(
                                                          buttonPosition,
                                                          MVector2(48, 20),
                                                          MVector4(1, 1, 1, 1),
                                                          &objectButtonEvents);
                objectButton->setMode(MGUI_BUTTON_SIMPLE);
                objectButton->setText(name);
                objectButton->setNormalColor(MVector4(0, 0, 0, 0));
                objectButton->setHighLightColor(MVector4(0.30, 0.48, 0.62, 1.0));
                if (maratis->isObjectSelected(objectAtButton))
                    objectButton->setTextColor(MVector4(1, 1, 0, 1));
                else
                    objectButton->setTextColor(MVector4(1, 1, 1, 1));
                m_editWin->addButton(objectButton);
                buttonPosition += MVector2(0, ySpace+1);
            }
            position =+ MVector2(15, buttonPosition.y + ySpace);
        }

        if (lightsNumber > 0)
        {
            addVariableName(m_editWin, "lights", &position);
            MVector2 buttonPosition = position + MVector2(12, 5);
            for (int i = 0; i < lightsNumber; i++)
            {
                objectAtButton = scene->getLightByIndex(i);
                name = objectAtButton->getName();
                MGuiButton *objectButton = new MGuiButton(
                                                          buttonPosition,
                                                          MVector2(48, 20),
                                                          MVector4(1, 1, 1, 1),
                                                          &objectButtonEvents);
                objectButton->setMode(MGUI_BUTTON_SIMPLE);
                objectButton->setText(name);
                objectButton->setNormalColor(MVector4(0, 0, 0, 0));
                objectButton->setHighLightColor(MVector4(0.30, 0.48, 0.62, 1.0));
                if (maratis->isObjectSelected(objectAtButton))
                    objectButton->setTextColor(MVector4(1, 1, 0, 1));
                else
                    objectButton->setTextColor(MVector4(1, 1, 1, 1));
                m_editWin->addButton(objectButton);
                buttonPosition += MVector2(0, ySpace+1);
            }
            position =+ MVector2(15, buttonPosition.y + ySpace);
        }

        if (soundsNumber > 0)
        {
            addVariableName(m_editWin, "sounds", &position);
            MVector2 buttonPosition = position + MVector2(12, 5);
            for (int i = 0; i < soundsNumber; i++)
            {
                objectAtButton = scene->getSoundByIndex(i);
                name = objectAtButton->getName();
                MGuiButton *objectButton = new MGuiButton(
                                                          buttonPosition,
                                                          MVector2(48, 20),
                                                          MVector4(1, 1, 1, 1),
                                                          &objectButtonEvents);
                objectButton->setMode(MGUI_BUTTON_SIMPLE);
                objectButton->setText(name);
                objectButton->setNormalColor(MVector4(0, 0, 0, 0));
                objectButton->setHighLightColor(MVector4(0.30, 0.48, 0.62, 1.0));
                if (maratis->isObjectSelected(objectAtButton))
                    objectButton->setTextColor(MVector4(1, 1, 0, 1));
                else
                    objectButton->setTextColor(MVector4(1, 1, 1, 1));
                m_editWin->addButton(objectButton);
                buttonPosition += MVector2(0, ySpace+1);
            }
            position =+ MVector2(15, buttonPosition.y + ySpace);
        }

        if (textsNumber > 0)
        {
            addVariableName(m_editWin, "texts", &position);
            MVector2 buttonPosition = position + MVector2(12, 5);
            for (int i = 0; i < textsNumber; i++)
            {
                objectAtButton = scene->getTextByIndex(i);
                name = objectAtButton->getName();
                MGuiButton *objectButton = new MGuiButton(
                                                          buttonPosition,
                                                          MVector2(48, 20),
                                                          MVector4(1, 1, 1, 1),
                                                          &objectButtonEvents);
                objectButton->setMode(MGUI_BUTTON_SIMPLE);
                objectButton->setText(name);
                objectButton->setNormalColor(MVector4(0, 0, 0, 0));
                objectButton->setHighLightColor(MVector4(0.30, 0.48, 0.62, 1.0));
                if (maratis->isObjectSelected(objectAtButton))
                    objectButton->setTextColor(MVector4(1, 1, 0, 1));
                else
                    objectButton->setTextColor(MVector4(1, 1, 1, 1));
                m_editWin->addButton(objectButton);
                buttonPosition += MVector2(0, ySpace+1);
            }
            position =+ MVector2(15, buttonPosition.y + ySpace);
        }
        m_editWin->resizeScroll();
        return;
    }
}

void MaratisUI::drawText(int x, int y, const char * text, const MVector4 & color)
{
	m_text->setNormalColor(color);
	m_text->setText(text);
	m_text->setPosition(MVector2((float)x, (float)y));
	m_text->draw();
}

void MaratisUI::drawInfoText(void)
{
	MWindow * window = MWindow::getInstance();
	MMouse * mouse = MMouse::getInstance();

	unsigned int wWidth = window->getWidth();
	unsigned int wHeight = window->getHeight();

	int mX = mouse->getXPosition();
	int mY = mouse->getYPosition();

	float sX = m_infoText->getScale().x;
	float sY = m_infoText->getScale().y;

	float pX = (float)(mX - 20);
	float pY = (float)(mY + 20);

	if(pX < 0)
		pX = 0;

	if(pY < 0)
		pY = 0;

	if((pX + sX) > wWidth)
		pX = wWidth - sX;

	if((pY + sY) >  wHeight)
		pY = wHeight - sY;

	m_infoText->setPosition(MVector2(pX, pY));
	m_infoText->draw();
	m_infoText->setText("");
}

void MaratisUI::drawInfoTransform(void)
{
    /*
     MMouse * mouse = MMouse::getInstance();

     int mX = mouse->getXPosition();
     int mY = mouse->getYPosition();

     switch(m_transformMode)
     {
     case M_TRANSFORM_POSITION:
     m_positionImage->setPosition(MVector2(mX, mY + 24));
     m_positionImage->draw();
     break;
     case M_TRANSFORM_ROTATION:
     m_rotationImage->setPosition(MVector2(mX, mY + 24));
     m_rotationImage->draw();
     break;
     case M_TRANSFORM_SCALE:
     m_scaleImage->setPosition(MVector2(mX, mY + 24));
     m_scaleImage->draw();
     break;
     }
     */
}

void MaratisUI::setTransformMode(M_TRANSFORM_MODE mode)
{
	Maratis::getInstance()->setTransformMode(mode);

	switch(mode)
	{
        case M_TRANSFORM_MOUSE:
            m_mouse = true;
            m_position = false;
            m_rotation = false;
            m_scale = false;
            break;
        case M_TRANSFORM_POSITION:
            m_mouse = false;
            m_position = true;
            m_rotation = false;
            m_scale = false;
            break;
        case M_TRANSFORM_ROTATION:
            m_mouse = false;
            m_position = false;
            m_rotation = true;
            m_scale = false;
            break;
        case M_TRANSFORM_SCALE:
            m_mouse = false;
            m_position = false;
            m_rotation = false;
            m_scale = true;
            break;
	}
}

void MaratisUI::addTextButtonToMenu(MGuiMenu * menu, const char * text, MVector2 position, void (* buttonPointerEvent)(MGuiButton * button, MGuiEvent * guiEvents))
{
	MVector4 normalColor(0.8f, 0.9f, 1, 0.0f);
	MVector4 highLightColor(0.8f, 0.9f, 1, 0.2f);

	MGuiButton * newButton = new MGuiButton(
                                            position, MVector2(), normalColor,
                                            buttonPointerEvent);

	newButton->setText(text);
	newButton->setPressedColor(highLightColor);
	newButton->setHighLightColor(highLightColor);
	newButton->setTextColor(MVector4(1, 1, 1, 1));

	menu->getWindowMenu()->addButton(newButton);
}

void MaratisUI::addTextToMenu(MGuiMenu * menu, const char * text, MVector2 position)
{
	MGuiText * newText = new MGuiText(
                                      text, position, 16, MVector4(1, 1, 1, 0.5f));

	menu->getWindowMenu()->addText(newText);
}

MGuiButton * MaratisUI::addButtonToWindow(MGuiWindow * window, const char * texture, MVector2 position, void (* buttonPointerEvent)(MGuiButton * button, MGuiEvent * guiEvents))
{
	// colors
	MVector4 normalColor(1, 1, 1, 1);
	MVector4 highLightColor(0.8f, 0.9f, 1, 1);
	MVector4 pressColor(1, 1, 0.5f, 1);

	MGuiButton * newButton = new MGuiButton(
                                            position, MVector2(16, 16), normalColor,
                                            buttonPointerEvent);

	newButton->setNormalTexture(texture);
	newButton->setHighLightColor(highLightColor);
	newButton->setPressedColor(pressColor);

	window->addButton(newButton);

	return newButton;
}

void MaratisUI::updateViewMenu(void)
{
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();
	MScene * scene = level->getCurrentScene();

	m_viewMenu->clearWindowMenu();

	// scenes
	float y = 10;

	addTextButtonToMenu(m_viewMenu, "Perspective ", MVector2(0, y), NULL); y += 20;
	addTextButtonToMenu(m_viewMenu, "Ortho ", MVector2(0, y), NULL); y += 20;
	y += 10;
	addTextButtonToMenu(m_viewMenu, "Face ", MVector2(0, y), NULL); y += 20;
	addTextButtonToMenu(m_viewMenu, "Right ", MVector2(0, y), NULL); y += 20;
	addTextButtonToMenu(m_viewMenu, "Top ", MVector2(0, y), NULL); y += 20;
	addTextButtonToMenu(m_viewMenu, "Bottom ", MVector2(0, y), NULL); y += 20;

	unsigned int i;
	unsigned int cSize = scene->getCamerasNumber();

	if(cSize > 0)
		y += 10;

	for(i=0; i<cSize; i++)
	{
		MOCamera * camera = scene->getCameraByIndex(i);
		addTextButtonToMenu(m_viewMenu, camera->getName(), MVector2(0, y), NULL);
		y += 20;
	}

	addTextToMenu(m_viewMenu, "", MVector2(0, y-10));
}

void MaratisUI::updateSceneMenu(void)
{
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	m_sceneMenu->clearWindowMenu();

	// scenes
	float y = 10;
	unsigned int i;
	unsigned int sSize = level->getScenesNumber();
	for(i=0; i<sSize; i++)
	{
		MScene * scn = level->getSceneByIndex(i);
		addTextButtonToMenu(m_sceneMenu, scn->getName(), MVector2(0, y), NULL);
		y += 20;
	}

	// add new scene
	addTextButtonToMenu(m_sceneMenu, "Add New ", MVector2(0, y+10), newSceneEvents);
	addTextToMenu(m_sceneMenu, "", MVector2(0, y+20));

	if(level)
	{
		m_sceneMenu->setMenuId(level->getCurrentSceneId());

		if(level->getScenesNumber() > 0)
		{
			MScene * scene = level->getCurrentScene();
			MGuiEditText * sceneEdit = getSceneEdit();
			sceneEdit->setText(scene->getName());
		}
	}
}

void MaratisUI::closePopupButton(void)
{
	MWinEvent winEvent;
	winEvent.type = MWIN_EVENT_MOUSE_MOVE;

	m_popupWin->setVisible(false);
	MGui::getInstance()->onEvent(&winEvent);
}

void MaratisUI::setPopupButton(const char * text, void (* buttonPointerEvent)(MGuiButton * button, MGuiEvent * guiEvents))
{
	MWindow * window = MWindow::getInstance();

	MMouse * mouse = MMouse::getInstance();
	int mX = mouse->getXPosition();
	int mY = mouse->getYPosition();

	MGuiButton * pButton = m_popupWin->getButton(0);

	pButton->setText(text);
	pButton->setPointerEvent(buttonPointerEvent);
	MGui::getInstance()->unHighLightAllWindows();
	m_popupWin->autoScale();
	m_popupWin->setHighLight(true);
	m_popupWin->setVisible(true);

	float sX = m_popupWin->getScale().x;
	float sY = m_popupWin->getScale().y;
	float pX = mX - sX*0.5f;
	float pY = mY - sY*0.5f;

	if(pX < 0)
		pX = 0;

	if(pY < 0)
		pY = 0;

	if((pX + sX) > window->getWidth())
		pX = window->getWidth() - sX;

	if((pY + sY) >  window->getHeight())
		pY = window->getHeight() - sY;

	m_popupWin->setPosition(MVector2(pX-1, pY-1));
}

void MaratisUI::createGUI(void)
{
	Maratis * maratis = Maratis::getInstance();
	MWindow * window = MWindow::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();
	MScene * scene = level->getCurrentScene();

	MGui * gui = MGui::getInstance();

	// sizes
	float winWidth  = (float)window->getWidth();
	float winHeight = (float)window->getHeight();

	float headHeight = 50;
	float editWidth = 270; //270
	float keysHeight = 55;
	float timelineHeight = 30;
	float timeBlockHeight = keysHeight + timelineHeight;
	float timeLeftWidth = 90;
	float timeRightWidth = 90;
	float viewPropHeight = 23;

	// colors
	MVector4 normalColor(1, 1, 1, 1);
	MVector4 highLightColor(0.8f, 0.9f, 1, 1);
	MVector4 pressColor(1, 1, 0.5f, 1);

	MVector4 normalColorA(1, 1, 1, 0.2f);

	MVector4 menuColor(0.32f, 0.53f, 0.70f, 1.0f);

	// head
	m_headWin = new MGuiWindow(
                               MVector2(0, 0),
                               MVector2(winWidth, headHeight),
                               MVector4(1, 1, 1, 1),
                               MaratisUI::headWinEvents);

	float x = 10;

	// file menu
	MGuiMenu * fileMenu = new MGuiMenu(
		MVector2(x, 9),
		MVector2(64, 32),
        normalColor,
        NULL);

	x += 8;

	MGuiText * fileText = new MGuiText("File", MVector2(x, 17), 16, MVector4(1, 1, 1, 1));

	fileMenu->setHighLightColor(highLightColor);
	fileMenu->setPressedColor(pressColor);
	fileMenu->setNormalTexture("gui/menuShort.tga");
	fileMenu->setDrawingText(false);

	addTextButtonToMenu(fileMenu, " New Project", MVector2(0, 10), newProjectEvents);
	addTextButtonToMenu(fileMenu, " Open Project", MVector2(0, 30), openProjectEvents);
	addTextButtonToMenu(fileMenu, " New Level", MVector2(0, 60), newLevelEvents);
	addTextButtonToMenu(fileMenu, " Open Level", MVector2(0, 80), openLevelEvents);
	addTextButtonToMenu(fileMenu, " Save", MVector2(0, 110), saveEvents);
	addTextButtonToMenu(fileMenu, " Save as", MVector2(0, 130), saveAsEvents);
	
	addTextButtonToMenu(fileMenu, " Import 3d model", MVector2(0, 160), importExternalEvents);
	
    addTextButtonToMenu(fileMenu, " Publish Project", MVector2(0, 190), publishEvents);

	addTextButtonToMenu(fileMenu, " Quit", MVector2(0, 220), quitEvents);

	addTextToMenu(fileMenu, "Ctrl O ", MVector2(155, 80));
	addTextToMenu(fileMenu, "Ctrl S ", MVector2(155, 110));
	addTextToMenu(fileMenu, "Ctrl Q ", MVector2(155, 220));
	addTextToMenu(fileMenu, "", MVector2(0, 240));

	fileMenu->getWindowMenu()->setShadow(true);
	fileMenu->getWindowMenu()->setNormalColor(menuColor);

	// edit menu
	x = 84;
	MGuiMenu * editMenu = new MGuiMenu(
                                       MVector2(x, 9),
                                       MVector2(64, 32),
                                       normalColor,
                                       NULL);

	x += 8;
	MGuiText * editText = new MGuiText(
                                       "Edit", MVector2(x, 17), 16, MVector4(1, 1, 1, 1));

	editMenu->setHighLightColor(highLightColor);
	editMenu->setPressedColor(pressColor);
	editMenu->setNormalTexture("gui/menuShort.tga");
	editMenu->setDrawingText(false);

	addTextButtonToMenu(editMenu, " Undo", MVector2(0, 10), undoEvents);
	addTextButtonToMenu(editMenu, " Redo", MVector2(0, 30), redoEvents);
	addTextButtonToMenu(editMenu, " Select All", MVector2(0, 60), selectAllEvents);
	addTextButtonToMenu(editMenu, " Select Same Mesh", MVector2(0, 80), selectSameMeshEvents);
	addTextButtonToMenu(editMenu, " Link Selection", MVector2(0, 110), linkEvents);
	addTextButtonToMenu(editMenu, " Unlink Selection", MVector2(0, 130), unlinkEvents);
	addTextButtonToMenu(editMenu, " Activate/Deactivate", MVector2(0, 160), activeEvents);
	addTextButtonToMenu(editMenu, " Duplicate", MVector2(0, 190), duplicateEvents);
	addTextButtonToMenu(editMenu, " Delete", MVector2(0, 220), deleteEvents);

	addTextToMenu(editMenu, "Ctrl Z ", MVector2(200, 10));
	addTextToMenu(editMenu, "Ctrl Y ", MVector2(200, 30));
	addTextToMenu(editMenu, "Ctrl A ", MVector2(200, 60));
	addTextToMenu(editMenu, "Ctrl D ", MVector2(200, 190));
	addTextToMenu(editMenu, "Suppr ", MVector2(200, 220));
	addTextToMenu(editMenu, "", MVector2(0, 230));

	editMenu->getWindowMenu()->setShadow(true);
	editMenu->getWindowMenu()->setNormalColor(menuColor);


	// mouse button
	int buttonSpace = 20;
	//x = 158;
	x = 280;

	MGuiImage * image = new MGuiImage("gui/sep.png", MVector2(270, 0), MVector2(2, 50), MVector4(1, 1, 1, 0.75f));
	m_headWin->addImage(image);

	m_mouseButton = addButtonToWindow(
                                      m_headWin, "gui/buttonMouse.tga", MVector2(x, 17), mouseButtonEvents);

	m_mouseButton->setMode(MGUI_BUTTON_GROUP);
	m_mouseButton->enableVariable(&m_mouse, M_VAR_BOOL);

	// position
	x += buttonSpace;
	m_positionButton = addButtonToWindow(
                                         m_headWin, "gui/buttonPosition.tga", MVector2(x, 17), positionButtonEvents);
	m_positionButton->setMode(MGUI_BUTTON_GROUP);
	m_positionButton->enableVariable(&m_position, M_VAR_BOOL);

	// rotation
	x += buttonSpace;
	m_rotationButton = addButtonToWindow(
                                         m_headWin, "gui/buttonRotation.tga", MVector2(x, 17), rotationButtonEvents);
	m_rotationButton->setMode(MGUI_BUTTON_GROUP);
	m_rotationButton->enableVariable(&m_rotation, M_VAR_BOOL);

	// scale
	x += buttonSpace;
	m_scaleButton = addButtonToWindow(
                                      m_headWin, "gui/buttonScale.tga", MVector2(x, 17), scaleButtonEvents);
	m_scaleButton->setMode(MGUI_BUTTON_GROUP);
	m_scaleButton->enableVariable(&m_scale, M_VAR_BOOL);

	// world / local menu
	x += buttonSpace + 2;
	MGuiMenu * worldMenu = new MGuiMenu(
                                        MVector2(x, 17),
                                        MVector2(64, 32),
                                        MVector4(1, 1, 1, 0),
                                        NULL);

	worldMenu->enableVariable(&m_worldMenu, M_VAR_INT);
	worldMenu->setHighLightColor(MVector4(0.8f, 0.9f, 1, 0.2f));
	worldMenu->setTextColor(MVector4(1, 1, 1, 1));
	worldMenu->addSimpleButton(" Global ", worldButtonEvents);
	worldMenu->addSimpleButton(" Local ", localButtonEvents);

	worldMenu->getWindowMenu()->setShadow(true);
	worldMenu->getWindowMenu()->setNormalColor(menuColor);

	// camera
	x += 75;
	buttonSpace = 24;
	addButtonToWindow(m_headWin, "gui/buttonCamera.tga", MVector2(x, 17), cameraButtonEvents);

	// entity
	x += buttonSpace;
	addButtonToWindow(m_headWin, "gui/buttonEntity.tga", MVector2(x, 17), entityButtonEvents);

	// particles
	//x += buttonSpace;
	//addButtonToWindow(m_headWin, "gui/buttonParticles.tga", MVector2(x, 17), particlesButtonEvents);

	// light
	x += buttonSpace;
	addButtonToWindow(m_headWin, "gui/buttonLight.tga", MVector2(x, 17), lightButtonEvents);

	// sound
	x += buttonSpace;
	addButtonToWindow(m_headWin, "gui/buttonSound.tga", MVector2(x, 17), soundButtonEvents);

	// text
	x += buttonSpace;
	addButtonToWindow(m_headWin, "gui/buttonText.tga", MVector2(x, 17), textButtonEvents);


	// scene menu
	x += 39;
	MGuiImage * sceneMenuImg = new MGuiImage(
                                             "gui/menuLong.tga",
                                             MVector2(x, 9),
                                             MVector2(128, 32),
                                             normalColor);

	x += 5;
	m_sceneMenu = new MGuiMenu(
                               MVector2(x, 9),
                               MVector2(16, 32),
                               normalColor,
                               sceneMenuEvents);

	m_sceneMenu->setHighLightColor(highLightColor);
	m_sceneMenu->setPressedColor(pressColor);
	m_sceneMenu->setNormalTexture("gui/buttonList.tga");
	m_sceneMenu->setDrawingText(false);

	m_sceneMenu->getWindowMenu()->setShadow(true);
	m_sceneMenu->getWindowMenu()->setNormalColor(menuColor);

	// scene edit
	x += 17;
	m_sceneEdit = new MGuiEditText(
                                   scene->getName(),
                                   MVector2(x, 17),
                                   16,
                                   normalColor,
                                   sceneNameEvents);

	m_sceneEdit->setSingleLine(true);
	m_sceneEdit->limitLength(9);

	// scene delete
	x += 82;
	addButtonToWindow(m_headWin, "gui/buttonDelete.tga", MVector2(x, 17), deleteSceneEvents);

	// level properties
	/*x += 45;
     addButtonToWindow(
     m_headWin, "gui/buttonLevel.tga", MVector2(x, 17), levelButtonEvents)->setXScale(32);
     */
	// scene properties
	x += buttonSpace+16;
	addButtonToWindow(
                      m_headWin, "gui/buttonScene.tga", MVector2(x, 17), sceneButtonEvents)->setXScale(32);

	// game
	x += 54;
	addButtonToWindow(
                      m_headWin, "gui/buttonGame.tga", MVector2(x, 17), gameButtonEvents);

	// renderer menu
	x += 32;
	MGuiMenu * m_rendererMenu = new MGuiMenu(
                                             MVector2(x, 17),
                                             MVector2(64, 32),
                                             MVector4(1, 1, 1, 0),
                                             rendererMenuEvents);

	m_rendererMenu->setHighLightColor(MVector4(0.8f, 0.9f, 1, 0.2f));
	m_rendererMenu->setTextColor(MVector4(1, 1, 1, 1));
	m_rendererMenu->getWindowMenu()->setShadow(true);
	m_rendererMenu->getWindowMenu()->setNormalColor(menuColor);


	m_headWin->addMenu(fileMenu);
	m_headWin->addMenu(editMenu);
	m_headWin->addMenu(worldMenu);
	m_headWin->addMenu(m_sceneMenu);
	m_headWin->addText(fileText);
	m_headWin->addText(editText);
	m_headWin->addImage(sceneMenuImg);
	m_headWin->addEditText(m_sceneEdit);
	m_headWin->addMenu(m_rendererMenu);
	m_headWin->setNormalTexture("gui/winHead.tga");
	gui->addWindow(m_headWin);

	// edit top
	m_editTopWin = new MGuiWindow(
                                  MVector2(0, headHeight),
                                  MVector2(editWidth, viewPropHeight*2),
                                  MVector4(40/255.0f, 95/255.0f, 138/255.0f, 1),
                                  editTopWinEvents);

	m_tabTransform = new MGuiButton(
                                    MVector2(0, 3),
                                    MVector2(48, 20),
                                    MVector4(1, 1, 1, 1),
                                    &tabTransformEvents);

	m_tabTransform->setNormalTexture("gui/tabTransform0.tga");
	m_tabTransform->setPressedTexture("gui/tabTransform1.tga");
	m_tabTransform->setMode(MGUI_BUTTON_GROUP);
	m_tabTransform->setPressed(true);
	m_tabTransform->setVisible(false);

	m_tabProperties = new MGuiButton(
                                     MVector2(54, 3),
                                     MVector2(48, 20),
                                     MVector4(1, 1, 1, 1),
                                     &tabPropertiesEvents);

	m_tabProperties->setNormalTexture("gui/tabProperties0.tga");
	m_tabProperties->setPressedTexture("gui/tabProperties1.tga");
	m_tabProperties->setMode(MGUI_BUTTON_GROUP);
	m_tabProperties->setVisible(false);

	m_tabBehavior = new MGuiButton(
                                   MVector2(108, 3),
                                   MVector2(48, 20),
                                   MVector4(1, 1, 1, 1),
                                   &tabBehaviorEvents);

	m_tabBehavior->setNormalTexture("gui/tabBehavior0.tga");
	m_tabBehavior->setPressedTexture("gui/tabBehavior1.tga");
	m_tabBehavior->setMode(MGUI_BUTTON_GROUP);
	m_tabBehavior->setVisible(false);

    m_tabObjects = new MGuiButton(
                                  MVector2(editWidth-30, 3),
                                  MVector2(30, 20),
                                  MVector4(1, 1, 1, 1),
                                  &tabObjectsEvents);

	m_tabObjects->setNormalTexture("gui/tabObjects0.tga");
	m_tabObjects->setPressedTexture("gui/tabObjects1.tga");
	m_tabObjects->setMode(MGUI_BUTTON_GROUP);
	m_tabObjects->setVisible(false);

	m_objectListImage = new MGuiImage(
									  "gui/tabObjects1.tga",
									  MVector2(editWidth-30, 3),
									  MVector2(30, 20),
									  MVector4(1, 1, 1, 1));
	m_objectListImage->setVisible(false);


	m_tabImage = new MGuiImage(
                               NULL,
                               MVector2(0, viewPropHeight),
                               MVector2(editWidth, viewPropHeight),
                               MVector4(0.294f, 0.474f, 0.619f, 1));

	m_tabImage->setVisible(false);

	m_sceneProperties = new MGuiImage(
                                      "gui/tabScene.tga",
                                      MVector2(0, 3),
                                      MVector2(48, 20),
                                      MVector4(1, 1, 1, 1));
	m_sceneProperties->setVisible(false);

	m_projProperties = new MGuiImage(
                                     "gui/tabProject.tga",
                                     MVector2(0, 3),
                                     MVector2(48, 20),
                                     MVector4(1, 1, 1, 1));
	m_projProperties->setVisible(false);

	m_objectEdit = new MGuiEditText(
                                    "",
                                    MVector2(15, viewPropHeight + 4),
                                    16,
                                    normalColor,
                                    &objectEditEvents);

	m_tabText = new MGuiText(
                             "Objects list", MVector2(15, viewPropHeight + 4),
                             16,
                             normalColor);
	m_tabText->setVisible(false);

	m_objectEdit->setVisible(false);
	m_objectEdit->setSingleLine(true);
	m_objectEdit->limitLength(30);

	m_editTopWin->addButton(m_tabTransform);
	m_editTopWin->addButton(m_tabProperties);
	m_editTopWin->addButton(m_tabBehavior);
    m_editTopWin->addButton(m_tabObjects);
	m_editTopWin->addImage(m_tabImage);
	m_editTopWin->addImage(m_sceneProperties);
	m_editTopWin->addImage(m_projProperties);
	m_editTopWin->addImage(m_objectListImage);
	m_editTopWin->addEditText(m_objectEdit);
	m_editTopWin->addText(m_tabText);
	gui->addWindow(m_editTopWin);

	// edit
	m_editWin = new MGuiWindow(
                               MVector2(0, headHeight + viewPropHeight*2),
                               MVector2(editWidth, winHeight - headHeight - (viewPropHeight*2)),
                               MVector4(1, 1, 1, 1),
                               editWinEvents);

	m_editWin->setNormalTexture("gui/winEdit.tga");
	gui->addWindow(m_editWin);

	// camera win
	m_cameraWin = new MGuiWindow(
                                 MVector2(7, headHeight + viewPropHeight*2 + 30),
                                 MVector2(256, 192),
                                 m_guiColor,
                                 cameraWinEvents);

	m_cameraWin->setVisible(false);
	gui->addWindow(m_cameraWin);

	// text
	m_textWin = new MGuiWindow(
                               MVector2(editWidth, headHeight),
                               MVector2(winWidth - editWidth, winHeight - headHeight - timeBlockHeight),
                               MVector4(1, 1, 1, 1),
                               textWinEvents);

	m_textWin->setVisible(false);
	gui->addWindow(m_textWin);

	// time left
	m_timeLeftWin = new MGuiWindow(
                                   MVector2(editWidth, winHeight - timeBlockHeight),
                                   MVector2(timeLeftWidth, timeBlockHeight),
                                   MVector4(1, 1, 1, 1),
                                   MaratisUI::timeLeftWinEvents);

	MGuiImage * timeLeftImg = new MGuiImage(
                                            "gui/timeLeftEdit.tga",
                                            MVector2(0, 0),
                                            MVector2(timeLeftWidth, timelineHeight),
                                            MVector4(1, 1, 1, 1));

	// time min edit
	MGuiEditText * timeLeftEdit = new MGuiEditText(
                                                   "",
                                                   MVector2(20, 8),
                                                   16,
                                                   MVector4(1, 1, 1, 1),
                                                   timeLeftEditEvents);

	timeLeftEdit->enableVariable(maratis->getTimelineMinPtr(), M_VAR_INT);
	timeLeftEdit->setSingleLine(true);
	timeLeftEdit->limitLength(7);

	m_timeLeftWin->setNormalTexture("gui/winTime.tga");
	m_timeLeftWin->addImage(timeLeftImg);
	m_timeLeftWin->addEditText(timeLeftEdit);
	gui->addWindow(m_timeLeftWin);

	// time right
	m_timeRightWin = new MGuiWindow(
                                    MVector2(winWidth - timeRightWidth, winHeight - timeBlockHeight),
                                    MVector2(timeRightWidth, timeBlockHeight),
                                    MVector4(1, 1, 1, 1),
                                    timeRightWinEvents);

	MGuiImage * timeRightImg = new MGuiImage(
                                             "gui/timeRightEdit.tga",
                                             MVector2(0, 0),
                                             MVector2(timeRightWidth, timelineHeight),
                                             MVector4(1, 1, 1, 1));

	// time max edit
	MGuiEditText * timeRightEdit = new MGuiEditText(
                                                    "",
                                                    MVector2(5, 8),
                                                    16,
                                                    MVector4(1, 1, 1, 1),
                                                    timeRightEditEvents);

	timeRightEdit->enableVariable(maratis->getTimelineMaxPtr(), M_VAR_INT);
	timeRightEdit->setSingleLine(true);
	timeRightEdit->limitLength(7);

	// key button
	MGuiButton * keyButton = new MGuiButton(
                                            MVector2(30, 75),
                                            MVector2(32, 32),
                                            normalColor,
                                            keyButtonEvents);

	keyButton->setHighLightColor(highLightColor);
	keyButton->setPressedColor(pressColor);
	keyButton->setNormalTexture("gui/buttonKey.tga");

	// play button
	MGuiButton * playButton = new MGuiButton(
                                             MVector2(30, 49),
                                             MVector2(16, 16),
                                             normalColor,
                                             playButtonEvents);

	playButton->enableVariable(&m_play, M_VAR_BOOL);
	playButton->setHighLightColor(highLightColor);
	playButton->setPressedColor(pressColor);
	playButton->setNormalTexture("gui/buttonPlay.tga");
	playButton->setMode(MGUI_BUTTON_GROUP);

	// pause button
	MGuiButton * pauseButton = new MGuiButton(
                                              MVector2(30 + 16, 49),
                                              MVector2(16, 16),
                                              normalColor,
                                              pauseButtonEvents);

	pauseButton->enableVariable(&m_pause, M_VAR_BOOL);
	pauseButton->setHighLightColor(highLightColor);
	pauseButton->setPressedColor(pressColor);
	pauseButton->setNormalTexture("gui/buttonPause.tga");
	pauseButton->setMode(MGUI_BUTTON_GROUP);

	m_timeRightWin->addImage(timeRightImg);
	m_timeRightWin->addButton(playButton);
	m_timeRightWin->addButton(pauseButton);
	//m_timeRightWin->addButton(keyButton);
	m_timeRightWin->addEditText(timeRightEdit);
	m_timeRightWin->setNormalTexture("gui/winTime.tga");
	gui->addWindow(m_timeRightWin);

	// timeline
	float timelineWidth = winWidth - timeRightWidth - timeLeftWidth - editWidth;

	m_timelineWin = new MGuiWindow(
                                   MVector2(editWidth + timeLeftWidth, winHeight - timeBlockHeight),
                                   MVector2(timelineWidth, timelineHeight),
                                   MVector4(1, 1, 1, 1),
                                   timelineWinEvents);

	// timeline slide
	m_timelineSlide = new MGuiSlide(
                                    MVector2(1, 1),
                                    MVector2(8, timelineHeight-2),
                                    normalColor,
                                    timelineSlideEvents);

	m_timelineSlide->enableVariable(maratis->getTimelinePosPtr(), M_VAR_INT);
	m_timelineSlide->setMinValue((float)maratis->getTimelineMin());
	m_timelineSlide->setMaxValue((float)maratis->getTimelineMax());
	m_timelineSlide->setHighLightColor(highLightColor);
	m_timelineSlide->setPressedColor(pressColor);
	m_timelineSlide->setNormalTexture("gui/slideTimeline.tga");
	m_timelineSlide->setDirection(MVector2(timelineWidth - m_timelineSlide->getButtonScale().x - 2, 0));

	// time pos edit
	m_timePosEdit = new MGuiEditText(
                                     "",
                                     MVector2(8, 8),
                                     16,
                                     MVector4(1, 1, 1, 1),
                                     timePosEditEvents);

	m_timePosEdit->enableVariable(maratis->getTimelinePosPtr(), M_VAR_INT);
	m_timePosEdit->setSingleLine(true);
	m_timePosEdit->limitLength(7);

	m_timelineWin->addEditText(m_timePosEdit);
	m_timelineWin->addSlide(m_timelineSlide);
	m_timelineWin->setNormalTexture("gui/winTimeline.tga");
	gui->addWindow(m_timelineWin);

	// keys
	m_keysWin = new MGuiWindow(
                               MVector2(editWidth + timeLeftWidth, winHeight - keysHeight),
                               MVector2(winWidth - timeRightWidth - timeLeftWidth - editWidth, keysHeight),
                               *getGUIColor(),
                               keysWinEvents);

	gui->addWindow(m_keysWin);

	// view properties
	{
		m_viewProp = new MGuiWindow(
                                    MVector2(editWidth, headHeight),
                                    MVector2(winWidth - editWidth, viewPropHeight),
                                    MVector4(0.2f, 0.27f, 0.35f, 1.0f),
                                    NULL);

		// view menu
		m_viewMenu = new MGuiMenu(
                                  MVector2(10, 0),
                                  MVector2(16, 23),
                                  normalColor,
                                  viewMenuEvents);

		m_viewMenu->setHighLightColor(highLightColor);
		m_viewMenu->setPressedColor(pressColor);
		m_viewMenu->setNormalTexture("gui/buttonList.tga");
		m_viewMenu->setDrawingText(false);

		m_viewMenu->getWindowMenu()->setShadow(true);
		m_viewMenu->getWindowMenu()->setNormalColor(menuColor);

		m_viewProp->addMenu(m_viewMenu);
		gui->addWindow(m_viewProp);
	}

	// view
	m_view = new MGuiWindow(
                            MVector2(editWidth, headHeight + viewPropHeight),
                            MVector2(winWidth - editWidth, winHeight - headHeight - viewPropHeight - timeBlockHeight),
                            m_guiColor,
                            viewEvents);

	gui->addWindow(m_view);

	// popup
	m_popupWin = new MGuiWindow(
                                MVector2(30, 30),
                                MVector2(150, 16),
                                MVector4(1.0f, 0.95f, 0.3f, 0.8f),
                                popupWinEvents);

	MGuiButton * okButton = new MGuiButton(
                                           MVector2(0, 0),
                                           MVector2(),
                                           MVector4(1, 1, 1, 0),
                                           NULL);

	okButton->setTextColor(MVector4(0, 0, 0, 1));

	m_popupWin->addButton(okButton);
	m_popupWin->autoScale();
	m_popupWin->setVisible(false);

	gui->addWindow(m_popupWin);

	updateSceneMenu();
	updateViewMenu();

	// file browser
	if(m_fileBrowser == NULL)
	{
		MVector2 fbPos = m_viewProp->getPosition();
		MVector2 fbScale = MVector2(m_view->getScale().x, m_view->getScale().y + m_viewProp->getScale().y);

		m_fileBrowser = new MGuiFileBrowser(fbPos, fbScale);
	}
}

void MaratisUI::fileBrowserEvents(MGuiFileBrowser * fileBrowser, MGUI_FILE_BROWSER_EVENT_TYPE event)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(event)
	{
		case MGUI_FILE_BROWSER_EVENT_CANCEL:
			UI->m_view->setVisible(true);
			break;

		case MGUI_FILE_BROWSER_EVENT_OK:
		{
			char filename[256];
			const char * curDir = fileBrowser->getCurrentDirectory();
			const char * curFile = fileBrowser->getCurrentFile();
			getGlobalFilename(filename, curDir, curFile);

			UI->m_fileBrowserFuncPointer(filename);
			UI->m_view->setVisible(true);
			break;
		}
	}
}

void MaratisUI::openFileBrowser(const char * startPath, const char * startFile, const char * okName, void (* functionPointer)(const char * filename))
{
	if(m_fileBrowser)
	{
		m_fileBrowser->open(startPath, startFile, okName, fileBrowserEvents);
		m_view->setVisible(false);
		m_fileBrowserFuncPointer = functionPointer;
	}
}

void MaratisUI::resizeGUI(void)
{
	MWindow * window = MWindow::getInstance();

	// sizes
	float winWidth  = (float)window->getWidth();
	float winHeight = (float)window->getHeight();

	float headHeight = m_headWin->getScale().y;
	float editWidth = m_editWin->getScale().x;
	float keysHeight = m_keysWin->getScale().y;
	float timelineHeight = m_timelineWin->getScale().y;
	float timeBlockHeight = keysHeight + timelineHeight;
	float timeLeftWidth = m_timeLeftWin->getScale().x;
	float timeRightWidth = m_timeRightWin->getScale().x;
	float viewPropHeight = m_viewProp->getScale().y;

	// head
	m_headWin->setXScale(winWidth);

	// edit
	m_editWin->setYScale(winHeight - m_editWin->getPosition().y);

	// text
	m_textWin->setXScale(winWidth - editWidth);
	m_textWin->setYScale(winHeight - headHeight - timeBlockHeight);

	// time left
	m_timeLeftWin->setYPosition(winHeight - timeBlockHeight);

	// time right
	m_timeRightWin->setXPosition(winWidth - timeRightWidth);
	m_timeRightWin->setYPosition(winHeight - timeBlockHeight);

	// timeline
	float timelineWidth = winWidth - timeRightWidth - timeLeftWidth - editWidth;
	m_timelineWin->setYPosition(winHeight - timeBlockHeight);
	m_timelineWin->setXScale(timelineWidth);

	m_timelineSlide->setDirection(MVector2(timelineWidth - m_timelineSlide->getButtonScale().x - 2, 0));
	m_timelineSlide->onChange();

	// keys
	m_keysWin->setYPosition(winHeight - keysHeight);
	m_keysWin->setXScale(winWidth - timeRightWidth - timeLeftWidth - editWidth);

	// view properties
	m_viewProp->setXScale(winWidth - editWidth);

	// view
	m_view->setXScale(winWidth - editWidth);
	m_view->setYScale(winHeight - headHeight - viewPropHeight - timeBlockHeight);

	// file browser
	if(m_fileBrowser)
	{
		MVector2 fbPos = m_viewProp->getPosition();
		MVector2 fbScale = MVector2(m_view->getScale().x, m_view->getScale().y + m_viewProp->getScale().y);
		m_fileBrowser->resize(fbPos, fbScale);
	}
}

// timeline slide
void MaratisUI::timelineSlideEvents(MGuiSlide * slide, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();
	MGuiEditText * timePosEdit = UI->getTimePosEdit();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_ON_CHANGE:
		{
			timePosEdit->updateFromVariable();

			float tWidth = timePosEdit->getScale().x;
			float sXPos = slide->getButtonPosition().x;
			float sWidth = slide->getButtonScale().x;

			float value = slide->getNormalizedValue();

			if(value <= 0.5f)
				timePosEdit->setXPosition(sXPos + sWidth + 1);
			else
				timePosEdit->setXPosition(sXPos - tWidth - 1);
		}
            break;

        default:
            break;
	}
}

// edit
void MaratisUI::editEvents(MGuiEditText * edit, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();
	MObject3d * object = UI->getEditedObject();
	
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	
	switch(guiEvents->type)
	{
			
		case MGUI_EVENT_ON_CHANGE:
			
			// text
			if(edit->getVariablePointer() == &UI->m_editText)
				((MOText *)object)->setText(edit->getText());
			
			break;
			
        case MGUI_EVENT_SEND_VARIABLE:

            maratis->autoSave();

            // parent
            if(edit->getVariablePointer() == &UI->m_editParent)
            {
				bool link = true;
				if(UI->m_editParent.getData())
				{
					if(UI->m_editParent.getData()[0] == '\0' || strcmp(UI->m_editParent.getData(), "...") == 0)
						link = false;
				}
				else
				{
					link = false;
				}
				
				// Either reset curren object's parent to NULL or try setting a new parent
				if(! link)
				{
					Maratis::getInstance()->unlinkTwoObjects(object->getParent(), object);
				}
				else
				{
					MObject3d *parent = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(UI->m_editParent.getData());
					if(parent != NULL)
						 Maratis::getInstance()->linkTwoObjects(parent, object);
				}
            }

            // position
			else if(edit->getVariablePointer() == &UI->m_editPosition.x ||
				    edit->getVariablePointer() == &UI->m_editPosition.y ||
				    edit->getVariablePointer() == &UI->m_editPosition.z)
            {
                object->setPosition(UI->m_editPosition);
            }

            // rotation
            else if(edit->getVariablePointer() == &UI->m_editRotation.x ||
                    edit->getVariablePointer() == &UI->m_editRotation.y ||
                    edit->getVariablePointer() == &UI->m_editRotation.z)
			{
				object->setEulerRotation(UI->m_editRotation);
			}

            // scale
			else if(edit->getVariablePointer() == &UI->m_editScale.x ||
					edit->getVariablePointer() == &UI->m_editScale.y ||
					edit->getVariablePointer() == &UI->m_editScale.z)
			{
				object->setScale(UI->m_editScale);
			}

			// light color
			else if(edit->getVariablePointer() == &UI->m_editLightColor.x ||
					edit->getVariablePointer() == &UI->m_editLightColor.y ||
					edit->getVariablePointer() == &UI->m_editLightColor.z)
				((MOLight *)object)->setColor(UI->m_editLightColor);
            // radius
			else if(edit->getVariablePointer() == &UI->m_editRadius)
				((MOLight *)object)->setRadius(UI->m_editRadius);
            // intensity
			else if(edit->getVariablePointer() == &UI->m_editIntensity)
				((MOLight *)object)->setIntensity(UI->m_editIntensity);
            // spotAngle
			else if(edit->getVariablePointer() == &UI->m_editSpotAngle)
			{
				float angle = CLAMP(UI->m_editSpotAngle, 1.0f, 90.0f);
				((MOLight *)object)->setSpotAngle(angle);
				UI->m_editSpotAngle = angle;
			}
            // spotExponent
			else if(edit->getVariablePointer() == &UI->m_editSpotExponent)
			{
				((MOLight *)object)->setSpotExponent(UI->m_editSpotExponent);
				UI->m_editSpotExponent = ((MOLight *)object)->getSpotExponent();
			}
            // shadowBias
			else if(edit->getVariablePointer() == &UI->m_editShadowBias)
			{
				((MOLight *)object)->setShadowBias(UI->m_editShadowBias);
			}
            // shadowBlur
			else if(edit->getVariablePointer() == &UI->m_editShadowBlur)
			{
				((MOLight *)object)->setShadowBlur(UI->m_editShadowBlur);
			}
            // shadowQuality
			else if(edit->getVariablePointer() == &UI->m_editShadowQuality)
			{
				UI->m_editShadowQuality = MIN(4096, /*getNextPowerOfTwo*/(UI->m_editShadowQuality));
				((MOLight *)object)->setShadowQuality(UI->m_editShadowQuality);
			}

            // fov
			else if(edit->getVariablePointer() == &UI->m_editFov)
				((MOCamera *)object)->setFov(UI->m_editFov);
            // clip near
            else if(edit->getVariablePointer() == &UI->m_editClipNear)
				((MOCamera *)object)->setClippingNear(UI->m_editClipNear);
            // clip far
            else if(edit->getVariablePointer() == &UI->m_editClipFar)
				((MOCamera *)object)->setClippingFar(UI->m_editClipFar);
            // fog distance
            else if(edit->getVariablePointer() == &UI->m_editFogDistance)
				((MOCamera *)object)->setFogDistance(UI->m_editFogDistance);
			// clear color
			else if(edit->getVariablePointer() == &UI->m_editCameraColor.x ||
					edit->getVariablePointer() == &UI->m_editCameraColor.y ||
					edit->getVariablePointer() == &UI->m_editCameraColor.z)
				((MOCamera *)object)->setClearColor(UI->m_editCameraColor);

            // pitch
            else if(edit->getVariablePointer() == &UI->m_editPitch)
				((MOSound *)object)->setPitch(UI->m_editPitch);
            // gain
			else if(edit->getVariablePointer() == &UI->m_editGain)
				((MOSound *)object)->setGain(UI->m_editGain = CLAMP(UI->m_editGain, 0.0f, 1.0f));
            // radius
            else if(edit->getVariablePointer() == &UI->m_editSndRadius)
				((MOSound *)object)->setRadius(UI->m_editSndRadius);
            // rolloff
            else if(edit->getVariablePointer() == &UI->m_editRolloff)
				((MOSound *)object)->setRolloff(UI->m_editRolloff);

            // mass
            else if(edit->getVariablePointer() == &UI->m_editMass)
				((MOEntity *)object)->getPhysicsProperties()->setMass(UI->m_editMass);
            // friction
            else if(edit->getVariablePointer() == &UI->m_editFriction)
				((MOEntity *)object)->getPhysicsProperties()->setFriction(UI->m_editFriction);
            // restitution
            else if(edit->getVariablePointer() == &UI->m_editRestitution)
				((MOEntity *)object)->getPhysicsProperties()->setRestitution(UI->m_editRestitution);
            // linear damping
            else if(edit->getVariablePointer() == &UI->m_editLinearDamping)
				((MOEntity *)object)->getPhysicsProperties()->setLinearDamping(UI->m_editLinearDamping);
            // angular damping
			else if(edit->getVariablePointer() == &UI->m_editAngularDamping)
				((MOEntity *)object)->getPhysicsProperties()->setAngularDamping(UI->m_editAngularDamping);
            // angular factor
            else if(edit->getVariablePointer() == &UI->m_editAngularFactor)
				((MOEntity *)object)->getPhysicsProperties()->setAngularFactor(UI->m_editAngularFactor);

			// text color
			else if(edit->getVariablePointer() == &UI->m_editTextColor.x ||
					edit->getVariablePointer() == &UI->m_editTextColor.y ||
					edit->getVariablePointer() == &UI->m_editTextColor.z ||
					edit->getVariablePointer() == &UI->m_editTextColor.w)
				((MOText *)object)->setColor(UI->m_editTextColor);
            // text size
            else if(edit->getVariablePointer() == &UI->m_editSize)
				((MOText *)object)->setSize(UI->m_editSize);
			
			// gravity
			else if(edit->getVariablePointer() == &UI->m_editGravity.x ||
					edit->getVariablePointer() == &UI->m_editGravity.y ||
					edit->getVariablePointer() == &UI->m_editGravity.z)
				scene->setGravity(UI->m_editGravity);

            break;
        case MGUI_EVENT_DRAW:
            if(! edit->isPressed())
            {
                // position
                if(	edit->getVariablePointer() == &UI->m_editPosition.x ||
                    edit->getVariablePointer() == &UI->m_editPosition.y ||
                    edit->getVariablePointer() == &UI->m_editPosition.z)
                {
                    UI->m_editPosition = object->getPosition();
                }

                // rotation
                else if(edit->getVariablePointer() == &UI->m_editRotation.x ||
                        edit->getVariablePointer() == &UI->m_editRotation.y ||
                        edit->getVariablePointer() == &UI->m_editRotation.z)
				{
					UI->m_editRotation = object->getEulerRotation();
				}

                // scale
				else if(edit->getVariablePointer() == &UI->m_editScale.x ||
						edit->getVariablePointer() == &UI->m_editScale.y ||
						edit->getVariablePointer() == &UI->m_editScale.z)
				{
					UI->m_editScale = object->getScale();
				}
            }
            break;

        default:
            break;
	}
}

// object
void MaratisUI::objectEditEvents(MGuiEditText * edit, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_SEND_VARIABLE:
        {
            maratis->autoSave();
            MObject3d * object = UI->getEditedObject();
            if(object)
            {
                const char * name = edit->getText();
                if(name)
                {
                    int size = scene->getObjectsNumber();
                    for(int i=0; i<size; i++)
                    {
                        MObject3d * obj = scene->getObjectByIndex(i);
                        if(obj->getName())
                            if(strcmp(name, obj->getName()) == 0)
                            {
                                edit->setText(object->getName());
                                return;
                            }
                    }

                    object->setName(name);
                    UI->editObject(object);
                    UI->updateViewMenu();
                }
            }
            else
            {
                MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

                const char * name = edit->getText();
                if(name)
                {
                    unsigned int i;
                    unsigned int sSize = level->getScenesNumber();
                    for(i=0; i<sSize; i++)
                    {
                        MScene * scn = level->getSceneByIndex(i);
                        if(scn != scene)
                        {
                            if(! strcmp(name, scn->getName()))
                            {
                                // name already exist
                                edit->setText(scene->getName());
                                return;
                            }
                        }
                    }

                    scene->setName(name);

                    // menu
                    UI->updateSceneMenu();
                    UI->getSceneEdit()->setText(name);
                }
            }
        }
            break;

        default:
            break;
	}
}

// time min edit
void MaratisUI::timeLeftEditEvents(MGuiEditText * edit, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	int min = maratis->getTimelineMin();
	int max = maratis->getTimelineMax();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_SEND_VARIABLE:
            if(min >= max)
            {
                maratis->setTimelineMin(max - 1);
                edit->updateFromVariable();
            }

            UI->getTimelineSlide()->setMinValue((float)maratis->getTimelineMin());
            UI->getTimelineSlide()->onChange();
            break;

        default:
            break;
	}
}

// time max edit
void MaratisUI::timeRightEditEvents(MGuiEditText * edit, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	int min = maratis->getTimelineMin();
	int max = maratis->getTimelineMax();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_SEND_VARIABLE:
            if(max <= min)
            {
                maratis->setTimelineMax(min + 1);
                edit->updateFromVariable();
            }

            UI->getTimelineSlide()->setMaxValue((float)maratis->getTimelineMax());
            UI->getTimelineSlide()->onChange();
            break;

        default:
            break;
	}
}

// time pos edit
void MaratisUI::timePosEditEvents(MGuiEditText * edit, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();
	MGuiSlide * timelineSlide = UI->getTimelineSlide();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_SEND_VARIABLE:
		{
			int tPos = maratis->getTimelinePos();
			int min = maratis->getTimelineMin();
			int max = maratis->getTimelineMax();

			if(tPos < min)
				maratis->setTimelinePos(min);
			else if(tPos > max)
				maratis->setTimelinePos(max);

			edit->updateFromVariable();

			float tWidth = edit->getScale().x;
			float sXPos = timelineSlide->getButtonPosition().x;
			float value = timelineSlide->getNormalizedValue();

			if(value > 0.5f)
				edit->setXPosition(sXPos - tWidth - 1);
		}
            break;

        case MGUI_EVENT_ON_CHANGE:
		{
			float tWidth = edit->getScale().x;
			float sXPos = timelineSlide->getButtonPosition().x;
			float value = timelineSlide->getNormalizedValue();

			if(value > 0.5f)
				edit->setXPosition(sXPos - tWidth - 1);
		}
            break;

        default:
            break;
	}
}


// end game
void MaratisUI::endGame(void)
{
	MEngine * engine = MEngine::getInstance();
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	MGame * game = MEngine::getInstance()->getGame();
	if(game)
	{
		MLevel * level = engine->getLevel();
		MScene * scene = level->getCurrentScene();

		// stop sound
		scene->stopAllSounds();

		// show mouse
		MWindow::getInstance()->showCursor();

		// end game
		game->end();

		// load temp level
		const char * temp = maratis->getTempDir();
		if(temp)
		{
			string tempFile(temp);
			tempFile += "/";
			tempFile += "temp.level";
			M_loadLevel(tempFile.c_str(), level, false);
		}

		scene = level->getCurrentScene();

		maratis->clearSelectedObjects();
		UI->resizeGUI();
		UI->editObject(NULL);
		UI->updateSceneMenu();
		UI->updateViewMenu();

		// update matrices
		scene->updateObjectsMatrices();
	}
}

// window events
void MaratisUI::windowEvents(MWinEvent * windowEvents)
{
	MWindow * window = MWindow::getInstance();
	MMouse * mouse = MMouse::getInstance();
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

    UI->m_lastInput = windowEvents->data[0];


	// game events
	gameWinEvents(windowEvents);

    // game
	MGame * game = MEngine::getInstance()->getGame();
	if(game)
	{
		if(game->isRunning())
		{
			if((windowEvents->type == MWIN_EVENT_KEY_UP) && (windowEvents->data[0] == MKEY_ESCAPE))
			{
				UI->endGame();
				return;
			}

			return;
		}
	}

	// gui
	MGui * gui = MGui::getInstance();

	switch(windowEvents->type)
	{
        case MWIN_EVENT_MOUSE_BUTTON_UP:
            maratis->clearCurrentAxis();
            break;

        case MWIN_EVENT_MOUSE_MOVE:
		{
			MGuiWindow * popup = UI->getPopupWin();
			if(window->isMouseOverWindow() && (! popup->isMouseInside()))
				popup->setVisible(false);

			if(mouse->isLeftButtonPushed())
			{
				maratis->transformSelectedObjects();
				break;
			}

			if(UI->getView()->isScrolled() && mouse->isMiddleButtonPushed())
			{
				if(UI->isShortcutEngaged(&UI->m_scPanView, false))
				{
					maratis->panCurrentVue();
					break;
				}
                maratis->rotateCurrentVue();
			}
			
			break;
		}
	
        case MWIN_EVENT_MOUSE_WHEEL_MOVE:
		{
			if(UI->getView()->isHighLight() && !mouse->isMiddleButtonPushed())
				maratis->zoomCurrentVue();
			break;
		}

        case MWIN_EVENT_WINDOW_RESIZE:
            UI->resizeGUI();
            break;

		case MWIN_EVENT_KEY_UP:
			
			if(UI->isShortcutEngaged(&UI->m_scPanView))
			{
				maratis->updateViewCenter();
			}
			
            if(UI->isShortcutEngaged(&UI->m_scSave))
            {
                maratis->save();
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scUndo))
            {
                maratis->undo();
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scRedo))
            {
                maratis->redo();
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scLoadLevel))
            {
                maratis->loadLevel();
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scQuit))
            {
                UI->setPopupButton(" Ok To Quit ", okQuitEvents);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scDuplicate))
            {
                maratis->duplicateSelectedObjects();
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scSelectAll))
            {
                maratis->selectAll();
                UI->editObject(UI->getEditedObject());
                break;
            }
			
            if(UI->isShortcutEngaged(&UI->m_scOrthogonalView))
            {
                if(! gui->isSomethingEditing())
                    maratis->switchCurrentVueMode();
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scFaceView))
            {
                if(! gui->isSomethingEditing())
                    maratis->changeCurrentVue(1);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scRightView))
            {
                if(! gui->isSomethingEditing())
                    maratis->changeCurrentVue(3);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scTopView))
            {
                if(! gui->isSomethingEditing())
                    maratis->changeCurrentVue(7);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scBottomView))
            {
                if(! gui->isSomethingEditing())
                    maratis->changeCurrentVue(9);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scTransformMouse))
            {
                if(! gui->isSomethingEditing())
                    UI->setTransformMode(M_TRANSFORM_MOUSE);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scTransformPosition))
            {
                if(! gui->isSomethingEditing())
                    UI->setTransformMode(M_TRANSFORM_POSITION);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scTransformRotation))
            {
                if(! gui->isSomethingEditing())
                    UI->setTransformMode(M_TRANSFORM_ROTATION);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scTransformScale))
            {
                if(! gui->isSomethingEditing())
                    UI->setTransformMode(M_TRANSFORM_SCALE);
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scFocusSelection))
            {
                if(! gui->isSomethingEditing())
                    maratis->focusSelection();
                break;
            }
            if(UI->isShortcutEngaged(&UI->m_scDelete) || UI->isShortcutEngaged(&UI->m_scDelete))
            {
                if((! gui->isSomethingEditing()) && (maratis->getSelectedObjectsNumber() > 0))
                    UI->setPopupButton(" Ok To Delete ", okDeleteObjectsEvents);
                break;
            }
			break;

		case MWIN_EVENT_WINDOW_CLOSE:
             UI->setPopupButton(" Ok To Quit ", okQuitEvents);
             break;
    }

	gui->onEvent(windowEvents);
}

// dynamic variables button
void MaratisUI::dVarButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();
	MObject3d * object = UI->getEditedObject();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_ON_CHANGE:
		{
			maratis->autoSave();

			// ortho
			if(button->getVariablePointer() == &UI->m_editOrtho)
				((MOCamera *)object)->enableOrtho(UI->m_editOrtho);
			// spot
			else if(button->getVariablePointer() == &UI->m_editSpot)
			{
				if(UI->m_editSpot) ((MOLight *)object)->setSpotAngle(45.0f);
				else ((MOLight *)object)->setSpotAngle(180.0f);
				UI->setNeedToUpdateEdit(true);
			}
			// shadow
			else if(button->getVariablePointer() == &UI->m_editShadow)
			{
				((MOLight *)object)->castShadow(UI->m_editShadow);
				UI->setNeedToUpdateEdit(true);
			}
			// fog
			else if(button->getVariablePointer() == &UI->m_editFog)
				((MOCamera *)object)->enableFog(UI->m_editFog);
			// loop
			else if(button->getVariablePointer() == &UI->m_editLoop)
				((MOSound *)object)->setLooping(UI->m_editLoop);
			// relative
			else if(button->getVariablePointer() == &UI->m_editRelative)
				((MOSound *)object)->setRelative(UI->m_editRelative);
			// invisible
			else if(button->getVariablePointer() == &UI->m_editInvisible)
				((MOEntity *)object)->setInvisible(UI->m_editInvisible);
			// ghost
			else if(button->getVariablePointer() == &UI->m_editGhost)
			{
				((MOEntity *)object)->getPhysicsProperties()->setGhost(UI->m_editGhost);
				UI->setNeedToUpdateEdit(true);
			}
			// physics
			else if(button->getVariablePointer() == &UI->m_editPhysics)
			{
				MOEntity * entity = (MOEntity *)object;
				if(UI->m_editPhysics)
					entity->createPhysicsProperties();
				else
					entity->deletePhysicsProperties();
				UI->setNeedToUpdateEdit(true);
			}
			// constraint
			else if(button->getVariablePointer() == &UI->m_editConstraint)
			{
				MOEntity * entity = (MOEntity *)object;
				if(UI->m_editConstraint)
					entity->getPhysicsProperties()->createConstraint();
				else
					entity->getPhysicsProperties()->deleteConstraint();
				UI->setNeedToUpdateEdit(true);
			}
		}
            break;

        default:
            break;
	}
}

// play button
void MaratisUI::playButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("play");
            break;

        default:
            break;
	}
}

// pause button
void MaratisUI::pauseButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("pause");
            break;

        default:
            break;
	}
}

// key button
void MaratisUI::keyButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("make key");
            break;

        default:
            break;
	}
}

// mouse button
void MaratisUI::mouseButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
                UI->setTransformMode(M_TRANSFORM_MOUSE);
            break;

        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("mouse (M)");
            break;

        default:
            break;
	}
}

// position button
void MaratisUI::positionButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
                UI->setTransformMode(M_TRANSFORM_POSITION);
            break;

        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("translate (T)");
            break;

        default:
            break;
	}
}

// rotation button
void MaratisUI::rotationButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
                UI->setTransformMode(M_TRANSFORM_ROTATION);
            break;

        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("rotate (R)");
            break;

        default:
            break;
	}
}

// scale button
void MaratisUI::scaleButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
                UI->setTransformMode(M_TRANSFORM_SCALE);
            break;

        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("scale (S)");
            break;

        default:
            break;
	}
}

// world button
void MaratisUI::worldButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                maratis->setOrientationMode(M_ORIENTATION_WORLD);
            break;

        default:
            break;
	}
}

// local button
void MaratisUI::localButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                maratis->setOrientationMode(M_ORIENTATION_LOCAL);
            break;

        default:
            break;
	}
}

// camera button
void MaratisUI::cameraButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("add camera");
            break;

        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
                maratis->addCamera();
            break;

        default:
            break;
	}
}

// entity button
void MaratisUI::entityButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("add entity");
            break;

        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
                maratis->addEntity();
            break;

        default:
            break;
	}
}

// background button
void MaratisUI::backgroundButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("load background");
            break;

        default:
            break;
	}
}

// particles button
void MaratisUI::particlesButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("add particles");
            break;

        default:
            break;
	}
}

// light button
void MaratisUI::lightButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("add light");
            break;

        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
                maratis->addLight();
            break;

        default:
            break;
	}
}

// sound button
void MaratisUI::soundButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("add sound");
            break;

        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
                maratis->addSound();
            break;

        default:
            break;
	}
}

// text button
void MaratisUI::textButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("add text");
            break;

        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
                maratis->addText();
            break;

        default:
            break;
	}
}

// shape button
void MaratisUI::shapeButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("add shape");
            break;

        default:
            break;
	}
}

// level button
void MaratisUI::levelButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("level properties");
            break;

        default:
            break;
	}
}

// scene button
void MaratisUI::sceneButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
                UI->editScene();
            break;

        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("scene properties");
            break;

        default:
            break;
	}
}

// game button
void MaratisUI::gameButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MEngine * engine = MEngine::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();
	MGame * game = MEngine::getInstance()->getGame();


	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed() && game)
            {
                MLevel * level = engine->getLevel();
                MScene * scene = level->getCurrentScene();

                const char * temp = Maratis::getInstance()->getTempDir();
                if(temp)
                {
                    string tempFile(temp);
                    tempFile += "/";
                    tempFile += "temp.level";

                    // save temp level
                    xmlLevelSave(engine->getLevel(), tempFile.c_str());
                }

                // play sound
                scene->stopAllSounds();
                scene->playLoopSounds();

                // hide mouse
//                MWindow::getInstance()->hideCursor();

                // begin game
                game->begin();
            }
            break;
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("play game");
            break;

        default:
            break;
	}
}

// new scene
void MaratisUI::newSceneEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isMouseInside())
            {
                unsigned int id = level->getScenesNumber();
                unsigned int count = id;

                char name[256];
                sprintf(name, "Scene-%d", count);

                int i;
                int sSize = (int)level->getScenesNumber();
                for(i=0; i<sSize; i++)
                {
                    MScene * scn = level->getSceneByIndex(i);
                    if(! strcmp(name, scn->getName()))
                    {
                        // name already exist
                        count++;
                        sprintf(name, "Scene-%d", count);
                        i = -1;
                    }
                }

                maratis->autoSave();
                MScene * scene = level->addNewScene();
                scene->setName(name);

                level->setCurrentSceneId(id);

                MGuiEditText * sceneEdit = UI->getSceneEdit();
                sceneEdit->setText(scene->getName());

                // menu
                UI->updateSceneMenu();
                UI->updateViewMenu();
                UI->editObject(NULL);
                maratis->clearSelectedObjects();
            }
            break;

        default:
            break;
	}
}

// scene name
void MaratisUI::sceneNameEvents(MGuiEditText * edit, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_SEND_VARIABLE:
		{
			MScene * scene = level->getCurrentScene();

			const char * name = edit->getText();
			if(name)
			{
				unsigned int i;
				unsigned int sSize = level->getScenesNumber();
				for(i=0; i<sSize; i++)
				{
					MScene * scn = level->getSceneByIndex(i);
					if(scn != scene)
					{
						if(! strcmp(name, scn->getName()))
						{
							// name already exist
							edit->setText(scene->getName());
							return;
						}
					}
				}

				maratis->autoSave();
				scene->setName(name);

				// menu
				UI->updateSceneMenu();

				if(UI->getEditedObject() == NULL)
					UI->getObjectEdit()->setText(name);
			}
		}
            break;

        default:
            break;
	}
}

// scene data
void MaratisUI::dataMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents)
{
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_SEND_VARIABLE:
        {
            MScene * scene = level->getCurrentScene();
            scene->setDataMode((M_DATA_MODES)menu->getMenuId());
        }
            break;

        default:
            break;
	}
}

// up behavior
void MaratisUI::upBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
        {
            if(button->isMouseInside() && guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                maratis->autoSave();
                MObject3d * object = UI->getEditedObject();
                object->invertBehavior(UI->getEditedBehavior() - 2, UI->getEditedBehavior() - 1);
                UI->setEditedBehavior(UI->getEditedBehavior() - 1);
                UI->setNeedToUpdateEdit(true);
            }
        }
            break;

        default:
            break;
	}
}

// down behavior
void MaratisUI::downBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isMouseInside() && guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                maratis->autoSave();
                MObject3d * object = UI->getEditedObject();
                object->invertBehavior(UI->getEditedBehavior() - 1, UI->getEditedBehavior());
                UI->setEditedBehavior(UI->getEditedBehavior() + 1);
                UI->setNeedToUpdateEdit(true);
            }
            break;

        default:
            break;
	}
}

// delete behavior
void MaratisUI::deleteBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isMouseInside())
            {
                UI->setPopupButton(" Ok To Delete ", okDeleteBehaviorEvents);
            }
            break;

        default:
            break;
	}
}

void MaratisUI::okDeleteBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isMouseInside() && guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                maratis->autoSave();

                UI->closePopupButton();

                MObject3d * object = UI->getEditedObject();
                object->deleteBehavior(UI->getEditedBehavior() - 1);
                UI->setEditedBehavior(0);

                UI->editObject(object);
            }
            break;

        default:
            break;
	}
}

// text align mesnu
void MaratisUI::textAlignMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_SEND_VARIABLE:
        {
            MOText * text = (MOText *)UI->getEditedObject();
            if(text){
                text->setAlign((M_ALIGN_MODES)menu->getMenuId());
            }
        }
            break;

        default:
            break;
	}
}

// behavior menu
void MaratisUI::behaviorMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();
	MBehaviorManager * behaviorManager = MEngine::getInstance()->getBehaviorManager();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(menu->isHighLight() && !menu->isPressed())
                UI->getInfoText()->setText("add behavior");
            break;

        case MGUI_EVENT_SEND_VARIABLE:
        {
            const char * behaviorName = menu->getText();
            MObject3d * object = UI->getEditedObject();
            if(object && behaviorName)
            {
                MBehaviorCreator * behavior = behaviorManager->getBehaviorByName(behaviorName);
                object->addBehavior(behavior->getNewBehavior(object));
                UI->setNeedToUpdateEdit(true);
                UI->setEditedBehavior(object->getBehaviorsNumber());
            }
        }
            break;

        default:
            break;
	}
}

// scene menu
void MaratisUI::sceneMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_ON_CHANGE:
        {
            maratis->autoSave();
            level->setCurrentSceneId(menu->getMenuId());
            MScene * scene = level->getCurrentScene();

            // update scene name edit
            MGuiEditText * sceneEdit = UI->getSceneEdit();
            sceneEdit->setText(scene->getName());
            UI->editObject(NULL);
            maratis->clearSelectedObjects();
        }
            break;

        default:
            break;
	}
}

// delete scene
void MaratisUI::deleteSceneEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed() && (level->getScenesNumber() > 1))
            {
                UI->setPopupButton(" Ok To Delete ", okDeleteSceneEvents);
            }
            break;

        default:
            break;
	}
}

// ok delete scene
void MaratisUI::okDeleteSceneEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isMouseInside() && guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                maratis->autoSave();
                level->deleteScene(level->getCurrentSceneId());

                // update scene name edit
                MScene * scene = level->getCurrentScene();
                MGuiEditText * sceneEdit = UI->getSceneEdit();
                sceneEdit->setText(scene->getName());

                UI->closePopupButton();

                // update scene menu
                UI->updateSceneMenu();
                UI->updateViewMenu();

                UI->editObject(NULL);
                maratis->clearSelectedObjects();
            }
            break;

        default:
            break;
	}
}

// quit
void MaratisUI::quitEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isMouseInside())
            {
                UI->setPopupButton(" Ok To Quit ", okQuitEvents);
            }
            break;

        default:
            break;
	}
}

// ok quit
void MaratisUI::okQuitEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isMouseInside() && guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                MWindow::getInstance()->setActive(false);
            break;

        default:
            break;
	}
}

// new level
void MaratisUI::newLevelEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
			UI->setPopupButton(" Ok New Level ", okNewLevelEvents);
            break;

        default:
            break;
	}
}

void MaratisUI::okNewLevelEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isMouseInside() && guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                UI->closePopupButton();
                maratis->newLevel();
            }
            break;

        default:
            break;
	}
}

// open
void MaratisUI::openLevelEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            maratis->loadLevel();
            break;

        default:
            break;
	}
}

// save
void MaratisUI::saveEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            maratis->save();
            break;

        default:
            break;
	}
}

// save as
void MaratisUI::saveAsEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            maratis->saveAs();
            break;

        default:
            break;
	}
}

// publish
void MaratisUI::publishEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            maratis->publish();
            break;

        default:
            break;
	}
}

// import external
void MaratisUI::importExternalEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            maratis->importExternal();
            break;

        default:
            break;
	}
}

// new project
void MaratisUI::newProjectEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            maratis->newProject();
            break;

        default:
            break;
	}
}

// open project
void MaratisUI::openProjectEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            maratis->loadProject();
            break;

        default:
            break;
	}
}

// view
void MaratisUI::viewEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
		{
			MEngine * engine = MEngine::getInstance();
			MLevel * level = engine->getLevel();
			MScene * scene = level->getCurrentScene();
			MRenderingContext * render = engine->getRenderingContext();

			// viewport
			unsigned int w = (unsigned int)window->getScale().x;
			unsigned int h = (unsigned int)window->getScale().y;
			int x = (int)window->getPosition().x;
			int y = MWindow::getInstance()->getHeight() - h - (int)window->getPosition().y;
			render->setViewport(x, y, w, h);

			// draw
			maratis->drawMainView(scene);
		}
            break;
	case MGUI_EVENT_MOUSE_BUTTON_DOWN:
		if(window->isHighLight() && guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
		{
			MEngine * engine = MEngine::getInstance();
			MLevel * level = engine->getLevel();
			MScene * scene = level->getCurrentScene();

			maratis->selectObjectsInMainView(scene);
		}
		break;
	}
}

// ok delete objects
void MaratisUI::okDeleteObjectsEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isMouseInside() && guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                maratis->deleteSelectedObjects();
                UI->closePopupButton();

                // update scene menu
                UI->updateSceneMenu();
                UI->updateViewMenu();

                UI->editObject(NULL);
            }
            break;

        default:
            break;
	}
}

// left anim id events
void MaratisUI::leftAnimIdEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
            {
                MOEntity * entity = (MOEntity *)UI->getEditedObject();
                if(entity->getAnimationId() > 0)
                {
                    entity->changeAnimation(entity->getAnimationId() - 1);
                    UI->updateAnimIdText(entity);
                }
            }
            break;

        default:
            break;
	}
}

// right anim id events
void MaratisUI::rightAnimIdEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
            {
                MOEntity * entity = (MOEntity *)UI->getEditedObject();
                MMesh * mesh = entity->getMesh();
                if(mesh){
                    if(entity->getAnimationId() < (mesh->getAnimsRangesNumber() - 1))
                    {
                        entity->changeAnimation(entity->getAnimationId() + 1);
                        UI->updateAnimIdText(entity);
                    }
                }
            }
            break;

        default:
            break;
	}
}

void MaratisUI::tabTransformEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("transform");
            break;
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
                UI->setNeedToUpdateEdit(true);
			//UI->editObject(UI->getEditedObject());
            break;

        default:
            break;
	}
}

void MaratisUI::tabPropertiesEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("properties");
            break;
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
                UI->setNeedToUpdateEdit(true);
			//UI->editObject(UI->getEditedObject());
            break;

        default:
            break;
	}
}

void MaratisUI::tabBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("behaviors");
            break;
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
                UI->setNeedToUpdateEdit(true);
			//UI->editObject(UI->getEditedObject());
            break;

        default:
            break;
	}
}

void MaratisUI::tabObjectsEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(button->isHighLight() && !button->isPressed())
                UI->getInfoText()->setText("objects list");
            break;
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
                UI->setNeedToUpdateEdit(true);
			//UI->editObject(UI->getEditedObject());
            break;

        default:
            break;
	}
}

void MaratisUI::objectButtonEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(button->isPressed())
            {
                MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();
                Maratis * maratis = Maratis::getInstance();
                const char * name = button->getText();

				MObject3d * object = scene->getObjectByName(name);
				if(object)
				{
					if (UI->isShortcutEngaged(&UI->m_scMultipleSelection, false))
					{
						maratis->addSelectedObject(object);
						if(maratis->isObjectSelected(object))
						{
							UI->setEditedObject(object);
						}
						else
						{
							unsigned int nbSel = maratis->getSelectedObjectsNumber();
							if(nbSel > 0)
							{
								MObject3d * prevObject = maratis->getSelectedObjectByIndex(nbSel-1);
								UI->setEditedObject(prevObject);
							}
							else
							{
								UI->setEditedObject(NULL);
							}

						}
					}
					else
					{
						maratis->clearSelectedObjects();
						maratis->addSelectedObject(object);
						UI->setEditedObject(object);
					}

					// force object-list tab
					UI->m_tabObjects->unPressGroupButtons();
					UI->m_tabObjects->setPressed(true);

					// update edit
					UI->setNeedToUpdateEdit(true);
				}
            }
            break;

        default:
            break;
	}
}

void MaratisUI::viewPropEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

// view menu
void MaratisUI::viewMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();
	MScene * scene = level->getCurrentScene();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_SEND_VARIABLE:
		{
			MOCamera * vue = maratis->getPerspectiveVue();

			unsigned int menuId = menu->getMenuId();
			if(menuId > 5)
			{
				MOCamera * camera = scene->getCameraByIndex((int)menuId - 6);
				MVector3 rot = camera->getTransformedRotation();
				vue->enableOrtho(camera->isOrtho());
				vue->setPosition(camera->getTransformedPosition());
				vue->setRotation(MQuaternion(rot.x, rot.y, rot.z));
				vue->updateMatrix();
			}
			else switch(menuId)
			{
                case 0: // Perspective
                    if(vue->isOrtho())
                        maratis->switchCurrentVueMode();
                    break;
                case 1: // Ortho
                    if(! vue->isOrtho())
                        maratis->switchCurrentVueMode();
                    break;
                case 2:
                    maratis->changeCurrentVue(1);
                    break;
                case 3:
                    maratis->changeCurrentVue(3);
                    break;
                case 4:
                    maratis->changeCurrentVue(7);
                    break;
                case 5:
                    maratis->changeCurrentVue(9);
                    break;
			}
		}
            break;

        default:
            break;
	}
}

void MaratisUI::cameraWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
		{
			MObject3d * object = UI->getEditedObject();

			if(! object)
				break;

			if(object->getType() != M_OBJECT3D_CAMERA)
				break;

			MOCamera * camera = (MOCamera *)object;

			MEngine * engine = MEngine::getInstance();
			MLevel * level = engine->getLevel();
			MScene * scene = level->getCurrentScene();
			MRenderingContext * render = engine->getRenderingContext();

			// clear screen
			window->setNormalColor(camera->getClearColor());

			// viewport
			int x = (int)window->getPosition().x;
			int y = (int)window->getPosition().y;
			unsigned int w = (unsigned int)window->getScale().x;
			unsigned int h = (unsigned int)window->getScale().y;
			render->setViewport(x, MWindow::getInstance()->getHeight() - h - y, w, h);

			// draw
			maratis->drawCamera(scene, camera);
		}
            break;

        default:
            break;
	}
}

void MaratisUI::popupWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

void MaratisUI::headWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

void MaratisUI::editTopWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

void MaratisUI::editWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(UI->needToUpdateEdit())
                UI->editObject(UI->getEditedObject());
            break;
        case MGUI_EVENT_MOUSE_BUTTON_DOWN:
            if(UI->m_tabBehavior->isPressed() && window->isPressed() && (UI->getEditedObject()))
            {
                unsigned int iSize = window->getImagesNumber();
                if(iSize < 2)
                    break;

                unsigned int i;
                for(i=0; i<(iSize-1); i++)
                {
                    MGuiImage * image = window->getImage(i);
                    if(image->isMouseInside())
                    {
                        UI->setEditedBehavior(i+1);
                        UI->setNeedToUpdateEdit(true);
                        break;
                    }
                }
            }
            break;

        default:
            break;
	}
}

void MaratisUI::textWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

void MaratisUI::timeLeftWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

void MaratisUI::timeRightWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

void MaratisUI::timelineWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

void MaratisUI::keysWinEvents(MGuiWindow * window, MGuiEvent * guiEvents)
{
}

// edit menu buttons
void MaratisUI::activeEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                maratis->activeSelection();
            break;

        default:
            break;
	}
}

void MaratisUI::undoEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                maratis->undo();
            break;

        default:
            break;
	}
}

void MaratisUI::redoEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                maratis->redo();
            break;

        default:
            break;
	}
}

void MaratisUI::selectAllEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
    MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                maratis->selectAll();
                UI->editObject(UI->getEditedObject());
            }
            break;

        default:
            break;
	}
}

void MaratisUI::selectSameMeshEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
    MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                maratis->selectSameMesh();
                UI->editObject(UI->getEditedObject());
            }
            break;

        default:
            break;
	}
}

void MaratisUI::duplicateEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                maratis->duplicateSelectedObjects();
            break;

        default:
            break;
	}
}

void MaratisUI::linkEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                maratis->linkSelectedObjects();
            break;

        default:
            break;
	}
}

void MaratisUI::unlinkEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
                maratis->unlinkSelectedObjects();
            break;

        default:
            break;
	}
}


void MaratisUI::deleteEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	Maratis * maratis = Maratis::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(guiEvents->data[0] == MMOUSE_BUTTON_LEFT)
            {
                if(maratis->getSelectedObjectsNumber() > 0)
                    UI->setPopupButton(" Ok To Delete ", okDeleteObjectsEvents);
            }
            break;

        default:
            break;
	}
}

// physics
void MaratisUI::collisionShapeEvents(MGuiMenu * menu, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_ON_CHANGE:
        {
            MOEntity * entity = (MOEntity *)UI->getEditedObject();
            entity->getPhysicsProperties()->setCollisionShape((M_COLLISION_SHAPE_TYPE)menu->getMenuId());
        }
            break;

        default:
            break;
	}
}

// script buttons
void MaratisUI::okLoadScript(const char * filename)
{
	if(! filename)
		return;

	MaratisUI * UI = MaratisUI::getInstance();
	MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

	scene->setScriptFilename(filename);
	UI->editScene();
}

void MaratisUI::loadScriptEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MWindow * window = MWindow::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
            {
                char startPath[256];
                getGlobalFilename(startPath, window->getWorkingDirectory(), "scripts");
				UI->openFileBrowser(startPath, "", "import script", okLoadScript);
            }
            break;

        default:
            break;
	}
}

void MaratisUI::deleteScriptEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MaratisUI * UI = MaratisUI::getInstance();
	MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
            {
                scene->setScriptFilename(NULL);
                UI->editScene();
            }
            break;

        default:
            break;
	}
}

void MaratisUI::editScriptEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MEngine * engine = MEngine::getInstance();
	MWindow * window = MWindow::getInstance();
	MScene * scene = engine->getLevel()->getCurrentScene();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
            {
                const char * script = scene->getScriptFilename();
                if(script)
                {
#ifdef _WIN32
					char args[256];
					sprintf(args, "\"%s\"", script);
					window->execute(SCRIPT_READER, args);
#else
					window->execute(SCRIPT_READER, script);
#endif
                }
            }
            break;

        default:
            break;
	}
}

// font button
void MaratisUI::okLoadFont(const char * filename)
{
	if(! filename)
		return;

	MaratisUI * UI = MaratisUI::getInstance();
	MLevel * level = MEngine::getInstance()->getLevel();
	MOText * text = (MOText *)UI->getEditedObject();

	MFontRef * fontRef = level->loadFont(filename);
	if(fontRef){
		text->setFontRef(fontRef);
		UI->setEditedObject(text);
		UI->setNeedToUpdateEdit(true);
	}
}

void MaratisUI::loadFontEvents(MGuiButton * button, MGuiEvent * guiEvents)
{
	MWindow * window = MWindow::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
            if(button->isPressed())
            {
                char startPath[256];
                getGlobalFilename(startPath, window->getWorkingDirectory(), "fonts");
                UI->openFileBrowser(startPath, "", "import font", okLoadFont);
            }
            break;

        default:
            break;
	}
}

// renderer menu
void MaratisUI::rendererMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents)
{
	MEngine * engine = MEngine::getInstance();
	Maratis * maratis = Maratis::getInstance();
	MRendererManager * rendererManager = engine->getRendererManager();
	MRenderer * renderer = engine->getRenderer();

	switch(guiEvents->type)
	{
        case MGUI_EVENT_DRAW:
            if(! menu->isPressed())
            {
                const char * currentRendererName = NULL;
                if(renderer)
                    currentRendererName = renderer->getName();

                static unsigned int frame = 0;
                unsigned int menuId = 0;
                unsigned int rSize = rendererManager->getRenderersNumber();

                if(frame%10) // update menu
                {
                    menu->clearWindowMenu();
                    for(unsigned i=0; i<rSize; i++)
                    {
                        MRendererCreator * renderer = rendererManager->getRendererByIndex(i);
                        const char * name = renderer->getName();
                        if(name)
                        {
                            if(currentRendererName){
                                if(strcmp(currentRendererName, name) == 0)
                                    menuId = menu->getWindowMenu()->getButtonsNumber();
                            }

                            menu->addSimpleButton(name, NULL);
                        }
                    }
                    menu->setMenuId(menuId);
                }

                frame++;
            }
            break;
        case MGUI_EVENT_SEND_VARIABLE:
		{
			const char * name = menu->getText();
			if(name)
				maratis->changeRenderer(name);
		}
            break;

        default:
            break;
	}
}

// load texture events
static MTextureRef** loadTextureRefPtr;

void MaratisUI::okLoadTexture(const char * filename)
{
	if(! filename)
		return;

	MaratisUI * UI = MaratisUI::getInstance();
	MLevel * level = MEngine::getInstance()->getLevel();
	(*loadTextureRefPtr) = level->loadTexture(filename);
	UI->m_needToUpdateEdit = true;
}

void MaratisUI::loadTextureEvents(MGuiButton * button, MGuiEvent * guiEvent)
{
	MWindow * window = MWindow::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	switch(guiEvent->type)
	{
		case MGUI_EVENT_MOUSE_BUTTON_UP:
		{
			MTextureRef** textureRef = (MTextureRef**)button->getVariablePointer();
			if(textureRef)
			{
				loadTextureRefPtr = textureRef;

				char startPath[256];
                getGlobalFilename(startPath, window->getWorkingDirectory(), "maps");
				UI->openFileBrowser(startPath, "", "load texture", okLoadTexture);
			}
			break;
		}
	}
}

void MaratisUI::loadSettings()
{
    MKeyboard * keyboard = MKeyboard::getInstance();
    keyboard->onKeyDown(MKEY_DUMMY);

    //assign default values
    m_keys["A"] = 'A';
    m_keys["B"] = 'B';
    m_keys["C"] = 'C';
    m_keys["D"] = 'D';
    m_keys["E"] = 'E';
    m_keys["F"] = 'F';
    m_keys["G"] = 'G';
    m_keys["H"] = 'H';
    m_keys["I"] = 'I';
    m_keys["J"] = 'J';
    m_keys["K"] = 'K';
    m_keys["L"] = 'L';
    m_keys["M"] = 'M';
    m_keys["N"] = 'N';
    m_keys["O"] = 'O';
    m_keys["P"] = 'P';
    m_keys["Q"] = 'Q';
    m_keys["R"] = 'R';
    m_keys["S"] = 'S';
    m_keys["T"] = 'T';
    m_keys["U"] = 'U';
    m_keys["V"] = 'V';
    m_keys["W"] = 'W';
    m_keys["X"] = 'X';
    m_keys["Y"] = 'Y';
    m_keys["Z"] = 'Z';
    m_keys["1"] = '1';
    m_keys["2"] = '2';
    m_keys["3"] = '3';
    m_keys["4"] = '4';
    m_keys["5"] = '5';
    m_keys["6"] = '6';
    m_keys["7"] = '7';
    m_keys["8"] = '8';
    m_keys["9"] = '9';
    m_keys["0"] = '0';
    m_keys["-"] = '-';
    m_keys["="] = '=';
    m_keys["/"] = '/';
    m_keys["."] = '.';
    m_keys[","] = ',';
    m_keys["\\"] = '\\';
    m_keys["["] = '[';
    m_keys["]"] = ']';
    m_keys[";"] = ';';
    m_keys["'"] = '\'';
    m_keys["`"] = '`';
    m_keys["Escape"] = MKEY_ESCAPE;
    m_keys["Enter"] = MKEY_RETURN;
    m_keys["Num Enter"] = MKEY_KP_ENTER;
    m_keys["Tab"] = MKEY_TAB;
    m_keys["Backspace"] = MKEY_BACKSPACE;
    m_keys["Space"] = MKEY_SPACE;
    m_keys["Right Control"] = MKEY_RCONTROL;
    m_keys["Left Control"] = MKEY_LCONTROL;
    m_keys["Right Super"] = MKEY_RSUPER;
    m_keys["Left Super"] = MKEY_LSUPER;
    m_keys["Right Alt"] = MKEY_RALT;
    m_keys["Left Alt"] = MKEY_LALT;
    m_keys["Right Shift"] = MKEY_RSHIFT;
    m_keys["Left Shift"] = MKEY_LSHIFT;
    m_keys["Right Super"] = MKEY_RSUPER;
    m_keys["Left Super"] = MKEY_LSUPER;
    m_keys["F1"] = MKEY_F1;
    m_keys["F2"] = MKEY_F2;
    m_keys["F3"] = MKEY_F3;
    m_keys["F4"] = MKEY_F4;
    m_keys["F5"] = MKEY_F5;
    m_keys["F6"] = MKEY_F6;
    m_keys["F7"] = MKEY_F7;
    m_keys["F8"] = MKEY_F8;
    m_keys["F9"] = MKEY_F9;
    m_keys["F10"] = MKEY_F10;
    m_keys["F11"] = MKEY_F11;
    m_keys["F12"] = MKEY_F12;
    m_keys["Print Screen"] = MKEY_PRINT;
    m_keys["Pause"] = MKEY_PAUSE;
    m_keys["Help"] = MKEY_HELP;
    m_keys["Insert"] = MKEY_INSERT;
    m_keys["Home"] = MKEY_HOME;
    m_keys["Page Up"] = MKEY_PAGEUP;
    m_keys["Page Down"] = MKEY_PAGEDOWN;
    m_keys["End"] = MKEY_END;
    m_keys["Delete"] = MKEY_DELETE;
    m_keys["Num Lock"] = MKEY_NUMLOCK;
    m_keys["Num 1"] = MKEY_KP1;
    m_keys["Num 2"] = MKEY_KP2;
    m_keys["Num 3"] = MKEY_KP3;
    m_keys["Num 4"] = MKEY_KP4;
    m_keys["Num 5"] = MKEY_KP5;
    m_keys["Num 6"] = MKEY_KP6;
    m_keys["Num 7"] = MKEY_KP7;
    m_keys["Num 8"] = MKEY_KP8;
    m_keys["Num 9"] = MKEY_KP9;
    m_keys["Num 0"] = MKEY_KP0;
    m_keys["Left"] = MKEY_LEFT;
    m_keys["Right"] = MKEY_RIGHT;
    m_keys["Down"] = MKEY_DOWN;
    m_keys["Up"] = MKEY_UP;
    m_keys["None"] = MKEY_DUMMY;


    m_scSave.key1 = m_keys["S"];
    m_scSave.key2 = m_keys["Left Control"];
    m_scSave.key3 = m_keys["None"];
    m_shortcuts["Save"] = &m_scSave;

    m_scUndo.key1 = m_keys["Z"];
    m_scUndo.key2 = m_keys["Left Control"];
    m_scUndo.key3 = m_keys["None"];
    m_shortcuts["Undo"] = &m_scUndo;

    m_scRedo.key1 = m_keys["Y"];
    m_scRedo.key2 = m_keys["Left Control"];
    m_scRedo.key3 = m_keys["None"];
    m_shortcuts["Redo"] = &m_scRedo;

    m_scLoadLevel.key1 = m_keys["O"];
    m_scLoadLevel.key2 = m_keys["Left Control"];
    m_scLoadLevel.key3 = m_keys["None"];
    m_shortcuts["Load Level"] = &m_scLoadLevel;

    m_scQuit.key1 = m_keys["Q"];
    m_scQuit.key2 = m_keys["Left Control"];
    m_scQuit.key3 = m_keys["None"];
    m_shortcuts["Quit"] = &m_scQuit;

    m_scDuplicate.key1 = m_keys["D"];
    m_scDuplicate.key2 = m_keys["Left Control"];
    m_scDuplicate.key3 = m_keys["None"];
    m_shortcuts["Duplicate"] = &m_scDuplicate;

    m_scSelectAll.key1 = m_keys["A"];
    m_scSelectAll.key2 = m_keys["Left Control"];
    m_scSelectAll.key3 = m_keys["None"];
    m_shortcuts["Select All"] = &m_scSelectAll;

    m_scOrthogonalView.key1 = m_keys["Num 5"];
    m_scOrthogonalView.key2 = m_keys["None"];
    m_scOrthogonalView.key3 = m_keys["None"];
    m_shortcuts["Orthogonal View"] = &m_scOrthogonalView;

    m_scFaceView.key1 = m_keys["Num 1"];
    m_scFaceView.key2 = m_keys["None"];
    m_scFaceView.key3 = m_keys["None"];
    m_shortcuts["Face View"] = &m_scFaceView;

    m_scRightView.key1 = m_keys["Num 3"];
    m_scRightView.key2 = m_keys["None"];
    m_scRightView.key3 = m_keys["None"];
    m_shortcuts["Right View"] = &m_scRightView;

    m_scTopView.key1 = m_keys["Num 7"];
    m_scTopView.key2 = m_keys["None"];
    m_scTopView.key3 = m_keys["None"];
    m_shortcuts["Top View"] = &m_scTopView;

    m_scBottomView.key1 = m_keys["Num 9"];
    m_scBottomView.key2 = m_keys["None"];
    m_scBottomView.key3 = m_keys["None"];
    m_shortcuts["Bottom View"] = &m_scBottomView;

    m_scTransformMouse.key1 = m_keys["M"];
    m_scTransformMouse.key2 = m_keys["None"];
    m_scTransformMouse.key3 = m_keys["None"];
    m_shortcuts["Transform Mouse"] = &m_scTransformMouse;

    m_scTransformPosition.key1 = m_keys["T"];
    m_scTransformPosition.key2 = m_keys["None"];
    m_scTransformPosition.key3 = m_keys["None"];
    m_shortcuts["Transform Position"] = &m_scTransformPosition;

    m_scTransformRotation.key1 = m_keys["R"];
    m_scTransformRotation.key2 = m_keys["None"];
    m_scTransformRotation.key3 = m_keys["None"];
    m_shortcuts["Transform Rotation"] = &m_scTransformRotation;

    m_scTransformScale.key1 = m_keys["S"];
    m_scTransformScale.key2 = m_keys["None"];
    m_scTransformScale.key3 = m_keys["None"];
    m_shortcuts["Transform Scale"] = &m_scTransformScale;

    m_scMultipleSelection.key1 = m_keys["Left Control"];
    m_scMultipleSelection.key2 = m_keys["None"];
    m_scMultipleSelection.key3 = m_keys["None"];
    m_shortcuts["Multiple Selection"] = &m_scMultipleSelection;

    m_scFocusSelection.key1 = m_keys["F"];
    m_scFocusSelection.key2 = m_keys["None"];
    m_scFocusSelection.key3 = m_keys["None"];
    m_shortcuts["Focus Selection"] = &m_scFocusSelection;

    m_scDelete.key1 = m_keys["Backspace"];
    m_scDelete.key2 = m_keys["None"];
    m_scDelete.key3 = m_keys["None"];
    m_shortcuts["Delete"] = &m_scDelete;

    m_scDelete2.key1 = m_keys["Delete"];
    m_scDelete2.key2 = m_keys["None"];
    m_scDelete2.key3 = m_keys["None"];
    m_shortcuts["Delete2"] = &m_scDelete2;

    m_scPanView.key1 = m_keys["Left Control"];
    m_scPanView.key2 = m_keys["None"];
    m_scPanView.key3 = m_keys["None"];
    m_shortcuts["Pan View"] = &m_scPanView;

    // read preferences file
    const char * filename = "preferences.xml";
	TiXmlDocument doc(filename);
	if(! doc.LoadFile())
        return;
    TiXmlHandle hDoc(&doc);
	TiXmlElement * rootNode;
	TiXmlHandle hRoot(0);

	// maratis
	rootNode = hDoc.FirstChildElement().Element();
	if(! rootNode)
		return;

	if(strcmp(rootNode->Value(), "Maratis") != 0)
		return;

	hRoot = TiXmlHandle(rootNode);

	// preferences
	TiXmlElement * preferencesNode = rootNode->FirstChildElement("Preferences");
	if(! preferencesNode)
		return;

	// check keyboard node
	TiXmlElement * keyboardNode = preferencesNode->FirstChildElement("Keyboard");
	if(! keyboardNode)
		return;

	// check first shortcut
	TiXmlElement * shortcutNode = keyboardNode->FirstChildElement("Shortcut");
	if(! shortcutNode)
        return;

    do {
        string name = shortcutNode->Attribute("name");
        string key1 = shortcutNode->Attribute("key1");
        string key2 = shortcutNode->Attribute("key2");
        string key3 = shortcutNode->Attribute("key3");

        m_shortcuts[name]->key1 = m_keys[key1];
        m_shortcuts[name]->key2 = m_keys[key2];
        m_shortcuts[name]->key3 = m_keys[key3];

    } while ((shortcutNode = shortcutNode->NextSiblingElement()));
}

bool MaratisUI::isShortcutEngaged(struct Shortcut * sc, bool checkLastInput)
{
	MKeyboard * keyboard = MKeyboard::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

    unsigned int key1 = sc->key1;
    unsigned int key2 = sc->key2;
    unsigned int key3 = sc->key3;

    if(checkLastInput)
    {
        if(UI->m_lastInput != key1)
            return false;
    }
	else
	{
		if(! keyboard->isKeyPressed(key1))
			return false;
	}
	
    if(keyboard->isKeyPressed(key2) && keyboard->isKeyPressed(key3))
        return true;
    else
        return false;
}
