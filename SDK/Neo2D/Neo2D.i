%module Neo2DLua
%{
#include <Neo2D.h>
#include <Object2D.h>
#include <Widget.h>
#include <Button.h>
#include <Canvas.h>

#include <CheckButton.h>
#include <Container.h>
#include <EditField.h>
#include <Event.h>
#include <HorizontalLayout.h>
#include <VerticalLayout.h>
#include <ImageButton.h>
#include <KeyboardShortcuts.h>
#include <Label.h>
#include <Menu.h>
#include <Neo2DLevel.h>
#include <RadioButton.h>
#include <Scrollbar.h>
#include <ScrollPanel.h>
#include <Sprite.h>
#include <Toolbar.h>
#include <Translator.h>
#include <TreeView.h>
#include <SwigCasts.h>

using namespace Neo2D;
using namespace Gui;

%}

%include <std_vector.i>
#define NEO2D_EXPORT
#define override // For older versions of SWIG

// Expects the argument to disown like "Type* name"
#define DISOWN(a) %apply SWIGTYPE* DISOWN {a};

%define MAKE_SHARED_WIDGET(type)
%extend type {
	%newobject makeShared;
	static shared_ptr<type> makeShared(int x, int y, unsigned int w, unsigned int h, const char* label,
											const shared_ptr<Object2D>& parent) {
		return make_shared<type>(x,y,w,h,label,parent);
	}
};

/*%typemap(in) shared_ptr<Neo2D::Object2D>* {

  if(SWIG_ConvertPtr(L, $input, (void **) &$1, $1_descriptor, 0) == -1)
  {
     type* temp;
     if (SWIG_ConvertPtr(L, $input, (void **) &temp, $descriptor(type*), 0) == -1)
     {
     	std::cerr << "Error: Could not cast " << $1_descriptor << " to shared_ptr<Object2D>!" << std::endl;
        return 0;
     }
     $1 = (type*) temp;
  }
}*/

%enddef

// Define skeleton for shared_ptr
template<class T>
class shared_ptr
{
 public:
 	shared_ptr();
  	DISOWN(T* o) shared_ptr(T* o);
    T* operator-> () const;
    void reset();
    T* get() const;
};

%template(SharedObject2D) shared_ptr<Neo2D::Object2D>;
%template(SharedWidget) shared_ptr<Neo2D::Gui::Widget>;
%template(SharedCheckButton) shared_ptr<Neo2D::Gui::CheckButton>;
%template(SharedEditField) shared_ptr<Neo2D::Gui::EditField>;
%template(SharedImageButton) shared_ptr<Neo2D::Gui::ImageButton>;
%template(SharedLabel) shared_ptr<Neo2D::Gui::Label>;
%template(SharedSprite) shared_ptr<Neo2D::Sprite>;
%template(SharedContainer) shared_ptr<Neo2D::Gui::Container>;

MAKE_SHARED_WIDGET(Neo2D::Gui::Widget)
MAKE_SHARED_WIDGET(Neo2D::Gui::Button)
MAKE_SHARED_WIDGET(Neo2D::Gui::CheckButton)
MAKE_SHARED_WIDGET(Neo2D::Gui::EditField)
MAKE_SHARED_WIDGET(Neo2D::Gui::ImageButton)
MAKE_SHARED_WIDGET(Neo2D::Gui::Label)
MAKE_SHARED_WIDGET(Neo2D::Sprite)

%include "SwigCasts.h"

%extend Neo2D::Object2D {
	%newobject makeShared;
	static shared_ptr<Neo2D::Object2D> makeShared(int x, int y, unsigned int w, unsigned int h,
											const shared_ptr<Object2D>& parent) {
		return make_shared<Neo2D::Object2D>(x,y,w,h,parent);
	}
}

%extend Neo2D::Gui::Container {
	%newobject makeShared;
	static shared_ptr<Neo2D::Gui::Container> makeShared(int x, int y, unsigned int w, unsigned int h,
											const shared_ptr<Object2D>& parent)
	{
		return make_shared<Neo2D::Gui::Container>(x,y,w,h,parent);
	}
};

%ignore Neo2D::Gui::Widget::setCallback(std::function<Neo2D::Gui::Widget&, void*>);

#ifdef SWIGLUA
%header %{
lua_State* g_GlobalLuaState = 0;
%}

%init %{
g_GlobalLuaState = L;
%}

%extend Neo2D::Gui::Widget {
	void Neo2D::Gui::Widget::setCallback(const char* luaFunction)
	{
		lua_State* L = g_GlobalLuaState;

		$self->setScriptCallback(luaFunction);
		$self->setCallback([$self] (Neo2D::Gui::Widget& w, void* state) {

			lua_State* L = (lua_State*) state;
			lua_getglobal(L, w.getScriptCallback());

            if(!lua_isfunction(L, -1))
            {
            	lua_pop(L, 1);
            	std::cerr << "Error: Could not execute callback " << w.getScriptCallback() << " because it does not exist!" << std::endl;
            	return;
            }

			if(lua_pcall(L, 0, 0, 0) != 0)
            {
            	std::cerr << lua_tostring(L, -1) << std::endl;
            	return;
            }
		}, L);
	}
};
#endif

%include "include/Neo2D.h"
%include "include/Object2D.h"
%include "include/Widget.h"
%include "include/Button.h"
%include "include/CheckButton.h"
%include "include/Container.h"
%include "include/EditField.h"
%include "include/Event.h"
%include "include/LayoutStrategy.h"
%include "include/FilterStrategy.h"
%include "include/HorizontalLayout.h"
%include "include/VerticalLayout.h"
%include "include/ImageButton.h"
%include "include/KeyboardShortcuts.h"
%include "include/Label.h"
%include "include/Menu.h"
%include "include/Neo2DLevel.h"
%include "include/RadioButton.h"
%include "include/Scrollbar.h"
%include "include/ScrollPanel.h"
%include "include/Sprite.h"
%include "include/Toolbar.h"
%include "include/Translator.h"
%include "include/TreeView.h"
%include "include/Container.h"
%include "include/Canvas.h"
