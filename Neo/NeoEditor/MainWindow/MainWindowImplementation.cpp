//========================================================================
//	Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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

#include "MainWindow.h"

#include "Translator.h"
#include "Callbacks.h"
#include "Utils.h"
#include <string>
#include <FL/Fl_Color_Chooser.H>
#include <MeshSave.h>

using namespace Neo;

int EditorWindow::getSystemColor(int idx)
{
	// TODO: Boundary checking!
	return systemColors[idx];
}
extern EditorWindow window;

void SceneSetupDlg::ok_button_callback(Fl_Button* button, SceneSetupDlg* dlg)
{
	NeoEngine* engine = NeoEngine::getInstance();
	MVector3 ambientLight(dlg->color_r->value(), dlg->color_g->value(),
			dlg->color_b->value());

	engine->getLevel()->getCurrentScene()->setAmbientLight(ambientLight);

	button->parent()->label("Success");
	button->parent()->hide();
}

void SceneSetupDlg::cancel_button_callback(Fl_Button* button, void*)
{
	button->parent()->hide();
}

void SceneSetupDlg::find_file_callback(Fl_Button* button, void*)
{
	const char* filename = fl_native_file_chooser("Choose file", "*.lua",
			(current_project.path + "scripts").c_str(),
			Fl_Native_File_Chooser::BROWSE_FILE);

	int children = button->parent()->children();
	Fl_Input* input;

	for (int i = 0; i < children; i++)
	{
		if (!strcmp(button->parent()->child(i)->label(), "LUA-Script:"))
		{
			input = (Fl_Input*) button->parent()->child(i);
			input->value(filename);
			return;
		}
	}
}

void SceneSetupDlg::choose_light_color(Fl_Button* button, SceneSetupDlg* dlg)
{
	double r = dlg->color_r->value();
	double g = dlg->color_g->value();
	double b = dlg->color_b->value();

	fl_color_chooser("Choose a color", r, g, b);

	dlg->color_r->value(r);
	dlg->color_g->value(g);
	dlg->color_b->value(b);
}

void PlayerConsole::window_close_callback(Fl_Window* window, PlayerConsole* dlg)
{
	dlg->closed = true;
	window->hide();
	Fl::delete_widget(window);
}

PlayerConsole::PlayerConsole()
{
	closed = true;
}

void MaterialEditDlg::close_callback(Fl_Button* button, MaterialEditDlg* dlg)
{
	button->parent()->hide();
	Fl::delete_widget(button->parent());
	delete dlg;
}

void MaterialEditDlg::material_changed(Fl_Choice* choice, MaterialEditDlg* dlg)
{
	OEntity* entity =
			(OEntity*) NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(
					dlg->object_name);
	Mesh* mesh = entity->getMesh();

	if (!mesh)
	{
		MLOG_ERROR("mesh == NULL! This should not happen!");
		return;
	}

	if (choice->value() >= mesh->getMaterialsNumber())
		return;

	Material* material = mesh->getMaterial(choice->value());

	if (!material)
		return;

	MVector3 color = material->getEmit();

	dlg->color_r->value(color.x);
	dlg->color_g->value(color.y);
	dlg->color_b->value(color.z);

	color = material->getDiffuse();

	dlg->diffuse_r->value(color.x);
	dlg->diffuse_g->value(color.y);
	dlg->diffuse_b->value(color.z);

	color = material->getSpecular();

	dlg->specular_r->value(color.x);
	dlg->specular_g->value(color.y);
	dlg->specular_b->value(color.z);

	dlg->shininess_edit->value(material->getShininess());
	dlg->opacity_edit->value(material->getOpacity());
}

void MaterialEditDlg::close_window_callback(Fl_Window* window,
		MaterialEditDlg* dlg)
{
	Fl::delete_widget(window);
	delete dlg;
}

void MaterialEditDlg::apply_callback(Fl_Button*, MaterialEditDlg* dlg)
{
	OEntity* entity =
			(OEntity*) NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(
					dlg->object_name);
	Mesh* mesh = entity->getMesh();

	if (!mesh)
	{
		MLOG_ERROR("mesh == NULL! This should not happen!");
		return;
	}

	if (dlg->materials_chooser->value() >= mesh->getMaterialsNumber())
		return;

	Material* material = mesh->getMaterial(dlg->materials_chooser->value());

	if (!material)
		return;

	MVector3 color;

	color.x = dlg->color_r->value();
	color.y = dlg->color_g->value();
	color.z = dlg->color_b->value();

	material->setEmit(color);

	color.x = dlg->diffuse_r->value();
	color.y = dlg->diffuse_g->value();
	color.z = dlg->diffuse_b->value();

	material->setDiffuse(color);

	color.x = dlg->specular_r->value();
	color.y = dlg->specular_g->value();
	color.z = dlg->specular_b->value();

	material->setSpecular(color);

	material->setShininess(dlg->shininess_edit->value());
	material->setOpacity(dlg->opacity_edit->value());

	::window.glbox->redraw();
}

void MaterialEditDlg::save_callback(Fl_Button*, MaterialEditDlg* dlg)
{
	OEntity* entity =
			(OEntity*) NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(
					dlg->object_name);
	Mesh* mesh = entity->getMesh();

	if (!mesh)
	{
		MLOG_ERROR("mesh == NULL! This should not happen!");
		return;
	}

	xmlMeshSave(entity->getMeshRef()->getFilename(), mesh);
}

void MaterialEditDlg::choose_diffuse_color(Fl_Button* button,
		MaterialEditDlg* dlg)
{
	double r = dlg->color_r->value();
	double g = dlg->diffuse_g->value();
	double b = dlg->diffuse_b->value();

	fl_color_chooser("Choose a color", r, g, b);

	dlg->diffuse_r->value(r);
	dlg->diffuse_g->value(g);
	dlg->diffuse_b->value(b);
}

void MaterialEditDlg::choose_specular_color(Fl_Button* button,
		MaterialEditDlg* dlg)
{
	double r = dlg->specular_r->value();
	double g = dlg->specular_g->value();
	double b = dlg->specular_b->value();

	fl_color_chooser("Choose a color", r, g, b);

	dlg->specular_r->value(r);
	dlg->specular_g->value(g);
	dlg->specular_b->value(b);
}

void MaterialEditDlg::choose_emit_color(Fl_Button* button, MaterialEditDlg* dlg)
{
	double r = dlg->color_r->value();
	double g = dlg->color_g->value();
	double b = dlg->color_b->value();

	fl_color_chooser("Choose a color", r, g, b);

	dlg->color_r->value(r);
	dlg->color_g->value(g);
	dlg->color_b->value(b);
}

void PublishDlg::publish_click(Fl_Button*, PublishDlg* dlg)
{
	Maratis* maratis = Maratis::getInstance();

	setPubDir(dlg->output_edit->value());
	std::string oldLevel = maratis->getCurrentLevel();

	// TODO: Make more efficient!
	maratis->save();
	maratis->loadLevel(dlg->level_edit->value());
	maratis->save();

	maratis->publish();

	// TODO: Error checking!
	fl_message("Project was successfully published!");

	maratis->loadLevel(oldLevel.c_str());
	dlg->window->hide();
	delete dlg;
}

void PublishDlg::cancel_click(Fl_Button*, PublishDlg* dlg)
{
	dlg->window->hide();
	delete dlg;
}

void PublishDlg::find_output_dir(Fl_Button*, PublishDlg* dlg)
{
	const char* filename = fl_native_file_chooser("Choose output", NULL,
			dlg->output_edit->value(),
			Fl_Native_File_Chooser::BROWSE_DIRECTORY);

	if (filename == NULL)
		return;

	dlg->output_edit->value(filename);
}

void PublishDlg::find_main_level(Fl_Button*, PublishDlg* dlg)
{
	const char* filename = fl_native_file_chooser("Choose output", NULL,
			dlg->output_edit->value(), Fl_Native_File_Chooser::BROWSE_FILE);

	if (filename == NULL)
		return;

	dlg->level_edit->value(filename);
}

void new_scene_ok_callback(Fl_Button* button, void*)
{
	::window.scene_name = ::window.scene_name_input->value();

	button->parent()->hide();
}

void new_scene_cancel_callback(Fl_Button* button, void*)
{
	Fl_Window* window = (Fl_Window*) button->parent();

	::window.scene_name = "";
	window->hide();
}

void ConstraintPropertiesDlg::enable_constraint_callback(
		Fl_Check_Button* button, ConstraintPropertiesDlg* dlg)
{
	OEntity* entity =
			NeoEngine::getInstance()->getLevel()->getCurrentScene()->getEntityByName(
					window.name_edit->value());

	if (!entity)
		return;

	PhysicsProperties* phys = entity->getPhysicsProperties();

	if (!phys)
		return;

	PhysicsConstraint* constraint = phys->getConstraint();

	if (!constraint && button->value() == 1)
	{
		phys->createConstraint();

		dlg->xpivot_edit->activate();
		dlg->ypivot_edit->activate();
		dlg->zpivot_edit->activate();

		dlg->xlinear_lower->activate();
		dlg->ylinear_lower->activate();
		dlg->zlinear_lower->activate();

		dlg->xlinear_upper->activate();
		dlg->ylinear_upper->activate();
		dlg->zlinear_upper->activate();

		dlg->xangular_upper->activate();
		dlg->yangular_upper->activate();
		dlg->zangular_upper->activate();

		dlg->xangular_lower->activate();
		dlg->yangular_lower->activate();
		dlg->zangular_lower->activate();

		dlg->parent_collision->activate();
		dlg->parent_input->activate();

	}
	else if (constraint && button->value() == 0)
	{
		phys->deleteConstraint();

		dlg->xpivot_edit->deactivate();
		dlg->ypivot_edit->deactivate();
		dlg->zpivot_edit->deactivate();

		dlg->xlinear_lower->deactivate();
		dlg->ylinear_lower->deactivate();
		dlg->zlinear_lower->deactivate();

		dlg->xlinear_upper->deactivate();
		dlg->ylinear_upper->deactivate();
		dlg->zlinear_upper->deactivate();

		dlg->xangular_upper->deactivate();
		dlg->yangular_upper->deactivate();
		dlg->zangular_upper->deactivate();

		dlg->xangular_lower->deactivate();
		dlg->yangular_lower->deactivate();
		dlg->zangular_lower->deactivate();

		dlg->parent_collision->deactivate();
		dlg->parent_input->deactivate();

	}
}

void ConstraintPropertiesDlg::close_callback(Fl_Button*,
		ConstraintPropertiesDlg* dlg)
{
	OEntity* entity =
			NeoEngine::getInstance()->getLevel()->getCurrentScene()->getEntityByName(
					window.name_edit->value());

	if (!entity)
		return;

	PhysicsProperties* phys = entity->getPhysicsProperties();

	if (!phys)
	{
		fl_message(
				"Selected object has no physics properties. Can not edit constraints!");
		return;
	}

	PhysicsConstraint* constraint = phys->getConstraint();

	if (constraint)
	{
		MVector3 vector;

		vector.x = dlg->xpivot_edit->value();
		vector.y = dlg->ypivot_edit->value();
		vector.z = dlg->zpivot_edit->value();
		constraint->pivot = vector;

		vector.x = dlg->xlinear_lower->value();
		vector.y = dlg->ylinear_lower->value();
		vector.z = dlg->zlinear_lower->value();
		constraint->lowerLinearLimit = vector;

		vector.x = dlg->xlinear_upper->value();
		vector.y = dlg->ylinear_upper->value();
		vector.z = dlg->zlinear_upper->value();
		constraint->upperLinearLimit = vector;

		vector.x = dlg->xangular_lower->value();
		vector.y = dlg->yangular_lower->value();
		vector.z = dlg->zangular_lower->value();
		constraint->lowerAngularLimit = vector;

		vector.x = dlg->xangular_upper->value();
		vector.y = dlg->yangular_upper->value();
		vector.z = dlg->zangular_upper->value();
		constraint->upperAngularLimit = vector;

		constraint->disableParentCollision = !dlg->parent_collision->value();
		constraint->parentName.set(dlg->parent_input->value());
	}

	dlg->win->hide();
	Fl::delete_widget(dlg->win);
	delete dlg;
}

void ConstraintPropertiesDlg::cancel_callback(Fl_Button*,
		ConstraintPropertiesDlg* dlg)
{
	dlg->win->hide();
	Fl::delete_widget(dlg->win);
	delete dlg;
}

void ConfigurationDlg::choose_background_color(Fl_Button*,
		ConfigurationDlg* dlg)
{
	double r = dlg->background_color_r->value();
	double g = dlg->background_color_g->value();
	double b = dlg->background_color_b->value();

	fl_color_chooser("Choose a color", r, g, b);

	dlg->background_color_r->value(r);
	dlg->background_color_g->value(g);
	dlg->background_color_b->value(b);
}

void ConfigurationDlg::choose_background2_color(Fl_Button*,
		ConfigurationDlg* dlg)
{
	double r = dlg->background2_color_r->value();
	double g = dlg->background2_color_g->value();
	double b = dlg->background2_color_b->value();

	fl_color_chooser("Choose a color", r, g, b);

	dlg->background2_color_r->value(r);
	dlg->background2_color_g->value(g);
	dlg->background2_color_b->value(b);
}

void ConfigurationDlg::choose_foreground_color(Fl_Button*,
		ConfigurationDlg* dlg)
{
	double r = dlg->foreground_color_r->value();
	double g = dlg->foreground_color_g->value();
	double b = dlg->foreground_color_b->value();

	fl_color_chooser("Choose a color", r, g, b);

	dlg->foreground_color_r->value(r);
	dlg->foreground_color_g->value(g);
	dlg->foreground_color_b->value(b);
}

void ConfigurationDlg::apply_settings_callback(Fl_Button*,
		ConfigurationDlg* dlg)
{
	Fl::foreground(255 * dlg->foreground_color_r->value(),
			255 * dlg->foreground_color_g->value(),
			255 * dlg->foreground_color_b->value());
	Fl::background(255 * dlg->background_color_r->value(),
			255 * dlg->background_color_g->value(),
			255 * dlg->background_color_b->value());
	Fl::background2(255 * dlg->background2_color_r->value(),
			255 * dlg->background2_color_g->value(),
			255 * dlg->background2_color_b->value());

	const char* themes[] =
	{ "gtk+", "gleam", "plastic", "none" };
	Fl::scheme(themes[dlg->theme_chooser->value()]);

	if (dlg->lang_chooser->changed()
			&& ask(tr("Your language selection requires the application to restart. Continue?")))
	{
		// Load all languages
		char dir[256];
		getGlobalFilename(dir, NeoWindow::getInstance()->getCurrentDirectory(),
				"translations");

		std::vector<std::string> files;
		readDirectory(dir, &files, false, false);

#ifndef WIN32
		Translator::getInstance()->loadTranslation(
				(string(dir) + "/" + files[dlg->lang_chooser->value()]).c_str());
#else
		Translator::getInstance()->loadTranslation((string(dir) + "\\" + files[dlg->lang_chooser->value()]).c_str());
#endif

		save_settings();

#ifndef WIN32
		getGlobalFilename(dir, NeoWindow::getInstance()->getCurrentDirectory(),
				"neo-editor");
#else
		getGlobalFilename(dir, NeoWindow::getInstance()->getCurrentDirectory(), "neo-editor.exe");
#endif
		NeoWindow::getInstance()->executeDetached(dir, NULL, true);
	}

	// FIXME: UGLY!!
	dlg->foreground_color_b->parent()->parent()->parent()->parent()->redraw();
	main_window->redraw();
}

void ConfigurationDlg::setInputMethod(Fl_Menu_*, long idx)
{
	if (idx == -1)
	{
		window.inputMethod = NULL;
		return;
	}

	extern std::vector<MPluginScript*> editorPlugins;
	window.inputMethod = editorPlugins[idx];
}

void ConfigurationDlg::plugin_changed_callback(Fl_Browser*,
		ConfigurationDlg* dlg)
{
	extern std::vector<MPluginScript*> editorPlugins;

	if (editorPlugins.size() < dlg->plugin_browser->value())
		return;
	int idx = dlg->plugin_browser->value() - 1;

	if (idx < 0)
		return;

	MPluginScript* plugin = editorPlugins[idx];
	dlg->author_edit->value(plugin->getAuthor().c_str());
	dlg->license_edit->value(plugin->getLicense().c_str());

	dlg->text_buffer.text(plugin->getDescription().c_str());
}

void ConfigurationDlg::reset_settings_callback(Fl_Button*,
		ConfigurationDlg* dlg)
{
	// Update colors
	Fl_Color c = window.getSystemColor(0);

	char unsigned bytes[4];
	bytes[0] = (c >> 24) & 0xFF;
	bytes[1] = (c >> 16) & 0xFF;
	bytes[2] = (c >> 8) & 0xFF;
	bytes[3] = c & 0xFF;

	dlg->background_color_r->value(static_cast<float>(bytes[0]) / 255.0f);
	dlg->background_color_g->value(static_cast<float>(bytes[1]) / 255.0f);
	dlg->background_color_b->value(static_cast<float>(bytes[2]) / 255.0f);

	c = window.getSystemColor(1);
	bytes[0] = (c >> 24) & 0xFF;
	bytes[1] = (c >> 16) & 0xFF;
	bytes[2] = (c >> 8) & 0xFF;
	bytes[3] = c & 0xFF;

	dlg->background2_color_r->value(static_cast<float>(bytes[0]) / 255.0f);
	dlg->background2_color_g->value(static_cast<float>(bytes[1]) / 255.0f);
	dlg->background2_color_b->value(static_cast<float>(bytes[2]) / 255.0f);

	c = window.getSystemColor(2);
	bytes[0] = (c >> 24) & 0xFF;
	bytes[1] = (c >> 16) & 0xFF;
	bytes[2] = (c >> 8) & 0xFF;
	bytes[3] = c & 0xFF;

	dlg->foreground_color_r->value(static_cast<float>(bytes[0]) / 255.0f);
	dlg->foreground_color_g->value(static_cast<float>(bytes[1]) / 255.0f);
	dlg->foreground_color_b->value(static_cast<float>(bytes[2]) / 255.0f);

	// FIXME: UGLY!!
	dlg->foreground_color_b->parent()->parent()->parent()->parent()->redraw();
	main_window->redraw();
}

void PostEffectsDlg::uniform_select_callback(Fl_Browser* widget,
		PostEffectsDlg* dlg)
{
	PostProcessor* pp = ::window.glbox->getPostProcessor();
	int idx = widget->value() - 1;

	if (idx < 0)
		return;

	switch (pp->getUniformType(idx))
	{
	case M_VARIABLE_FLOAT:
		dlg->uniform_value->value(pp->getFloatUniformValue(idx));
		break;
	}
}

void PostEffectsDlg::find_vert_file_callback(Fl_Button* widget,
		PostEffectsDlg* dlg)
{
	const char* filename = fl_native_file_chooser("Choose output", NULL,
			(current_project.path + "shaders").c_str(),
			Fl_Native_File_Chooser::BROWSE_FILE);

	if (filename == NULL)
		return;

	dlg->vert_shad_edit->value(filename);
}

void PostEffectsDlg::find_frag_file_callback(Fl_Button* widget,
		PostEffectsDlg* dlg)
{
	const char* filename = fl_native_file_chooser("Choose output", NULL,
			(current_project.path + "shaders").c_str(),
			Fl_Native_File_Chooser::BROWSE_FILE);

	if (filename == NULL)
		return;

	dlg->frag_shad_edit->value(filename);
}

void PostEffectsDlg::preview_callback(Fl_Button* widget, PostEffectsDlg* dlg)
{
	NeoEngine* engine = NeoEngine::getInstance();

	if (!ask(tr("If you load a new shader all uniform settings you created will be lost. Continue?")))
		return;

	PostProcessor* postProcessor = ::window.glbox->getPostProcessor();

	postProcessor->setResolutionMultiplier(dlg->resolution_edit->value());
	postProcessor->loadShaderFile(dlg->vert_shad_edit->value(),
			dlg->frag_shad_edit->value());

	// For use when saving the project
	engine->getGame()->getPostProcessor()->setResolutionMultiplier(
			dlg->resolution_edit->value());

	::window.glbox->enablePostEffects();
}

void PostEffectsDlg::use_post_effects_callback(Fl_Check_Button* widget,
		PostEffectsDlg* dlg)
{
	if (widget->value() != 0)
	{
		dlg->uniform_value->activate();
		dlg->vert_shad_edit->activate();
		dlg->frag_shad_edit->activate();
		dlg->uniforms_browser->activate();
		dlg->vert_btn->activate();
		dlg->frag_btn->activate();
		dlg->preview_btn->activate();
		dlg->add_uniform->activate();
		dlg->save_button->activate();
		dlg->load_button->activate();
		dlg->resolution_edit->activate();

		::window.glbox->enablePostEffects();
	}
	else
	{
		dlg->uniform_value->deactivate();
		dlg->vert_shad_edit->deactivate();
		dlg->frag_shad_edit->deactivate();
		dlg->uniforms_browser->deactivate();
		dlg->vert_btn->deactivate();
		dlg->frag_btn->deactivate();
		dlg->preview_btn->deactivate();
		dlg->add_uniform->deactivate();
		dlg->save_button->deactivate();
		dlg->load_button->deactivate();
		dlg->resolution_edit->deactivate();

		::window.glbox->disablePostEffects();
	}
}

void PostEffectsDlg::add_uniform_callback(Fl_Button*, PostEffectsDlg* dlg)
{
	Fl_Double_Window* win = dlg->new_uniform_window();
	win->show();

	while (win->shown())
		Fl::wait();

	Fl::delete_widget(win);
}

void PostEffectsDlg::new_uniform_ok(Fl_Button* btn, PostEffectsDlg* dlg)
{
	btn->parent()->hide();

	// TODO: More uniform types!
	::window.glbox->getPostProcessor()->addFloatUniform(
			dlg->uniform_name_edit->value());
	::window.glbox->getPostProcessor()->setFloatUniformValue(
			dlg->uniform_name_edit->value(), dlg->uniform_value_edit->value());

	dlg->update_uniform_list();
}

void PostEffectsDlg::new_uniform_cancel(Fl_Button* btn, PostEffectsDlg*)
{
	btn->parent()->hide();
}

void PostEffectsDlg::update_uniform_list()
{
	uniforms_browser->clear();
	PostProcessor* pp = ::window.glbox->getPostProcessor();

	uniforms_browser->begin();
	for (int i = 0; i < pp->getNumUniforms(); i++)
	{
		uniforms_browser->add(pp->getUniformName(i));
	}
	uniforms_browser->end();
}

void PostEffectsDlg::update_uniform_float(Fl_Button* btn, PostEffectsDlg* dlg)
{
	PostProcessor* pp = ::window.glbox->getPostProcessor();
	int idx = dlg->uniforms_browser->value() - 1;

	if (idx < 0)
		return;

	switch (pp->getUniformType(idx))
	{
	case M_VARIABLE_FLOAT:
		pp->setFloatUniformValue(pp->getUniformName(idx),
				dlg->uniform_value->value());
		break;
	}
}

void PostEffectsDlg::save_profile_callback(Fl_Button* widget,
		PostEffectsDlg* dlg)
{
	const char* filename = fl_native_file_chooser("Choose file", "*.ini",
			(current_project.path + "shaders").c_str(),
			Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

	if (filename == NULL)
		return;

	ofstream out;
	out.open(filename, ios::out);
	if (!out)
		return;

	out << "[Uniforms]" << endl;

	PostProcessor* pp = ::window.glbox->getPostProcessor();

	// TODO: Allow other variable types!
	for (int i = 0; i < pp->getNumUniforms(); i++)
	{
		out << pp->getUniformName(i) << "=" << pp->getFloatUniformValue(i)
				<< endl;
	}

	out.close();
}

void PostEffectsDlg::load_profile_callback(Fl_Button* widget,
		PostEffectsDlg* dlg)
{
	const char* filename = fl_native_file_chooser("Choose file", "*.ini",
			(current_project.path + "shaders").c_str(),
			Fl_Native_File_Chooser::BROWSE_FILE);

	if (filename == NULL)
		return;

	INI::Parser parser(filename);
	PostProcessor* pp = ::window.glbox->getPostProcessor();

	// TODO: More types!
	for (INI::Level::value_map_t::iterator iter =
			parser.top()("Uniforms").values.begin();
			iter != parser.top()("Uniforms").values.end(); iter++)
	{
		pp->addFloatUniform(iter->first.c_str());
		float value;

		sscanf(iter->second.c_str(), "%f", &value);
		pp->setFloatUniformValue(iter->first.c_str(), value);
	}

	dlg->update_uniform_list();
}

void PostEffectsDlg::update_resolution(Fl_Value_Input* value,
		PostEffectsDlg* dlg)
{
	PostProcessor* pp = ::window.glbox->getPostProcessor();
	pp->setResolutionMultiplier(value->value());

	NeoEngine::getInstance()->getGame()->getPostProcessor()->setResolutionMultiplier(
			value->value());
}
