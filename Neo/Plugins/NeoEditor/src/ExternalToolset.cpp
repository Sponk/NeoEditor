#include "ExternalToolset.h"
#include "Tool.h"
#include <Translator.h>

#define SEXPRESSO_OPT_OUT_PIKESTYLE
#include <sexpresso.hpp>

std::string ExternalToolset::fileDialog(const char* message,
										  const char* start,
										  const char* filetypes,
										  bool save,
										  bool directories)
{
	std::stringstream ss;
	ss << "(message \"" << message << "\")(startdir \"" << start << "\")(filetypes \"" << filetypes << "\")";
	ss << "(save " << save << ")(directories " << directories << ")";

	auto result = sexpresso::parse(Tool::executeToolNonBlocking("filedialog", ss.str().c_str()));
	auto filename = result.getChildByPath("filename");

	if(filename)
		return filename->arguments().begin()->value.str;

	return "";
}


void ExternalToolset::messagebox(const char* title, const char* message)
{
	std::stringstream ss;
	ss << "(title \"" << title << "\")(text \"" << message << "\")";
	Tool::executeToolNonBlocking("messagebox", ss.str().c_str());
}

void ExternalToolset::aboutDialog()
{
	messagebox(tr("About"), tr("This is the Neo Scene Editor experience v0.5.1a. This program is distributed under the terms and conditions of the GNU LGPL."));
}

Neo::Vector4 ExternalToolset::colorDialog(const Neo::Vector4& start)
{
	std::stringstream ss;
	ss << "(color " << start.x << " " << start.y << " " << start.z << " " << start.w << ")";

	auto result = sexpresso::parse(Tool::executeToolNonBlocking("colorpicker", ss.str().c_str()));
	auto comp = result.getChildByPath("color")->arguments().begin();

	Neo::Vector4 retval;
	retval.x = stof(comp->value.str);
	retval.y = stof((++comp)->value.str);
	retval.z = stof((++comp)->value.str);
	retval.w = stof((++comp)->value.str);

	return retval;
}
