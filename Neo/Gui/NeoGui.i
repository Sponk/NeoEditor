%module NeoGui
%{
	#include <NeoCore.h>
	#include <GuiSystem.h>
	#include <Widget.h>
	#include <Button.h>

	#include <ThemedButton.h>
	#include <Sprite.h>
	#include <Tile.h>
	#include <InputField.h>
	#include <Canvas.h>
	#include <Label.h>

	using namespace Neo;
	using namespace Gui;
	
	// #define Canvas Neo::Gui::Canvas
%}

#define NEO_CORE_EXPORT
%include <Vector2.h>
%include <Widget.h>
%include <Canvas.h>
%include <GuiSystem.h>
%include <Button.h>

%include <Sprite.h>
%include <Tile.h>
%include <InputField.h>
%include <ThemedButton.h>
%include <Label.h>
