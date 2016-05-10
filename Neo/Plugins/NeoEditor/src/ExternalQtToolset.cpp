#include "ExternalQtToolset.h"
#include "Tool.h"

#define SEXPRESSO_OPT_OUT_PIKESTYLE
#include <sexpresso.hpp>
#include <sstream>

std::string ExternalQtToolset::fileDialog(const char* message,
										  const char* start,
										  const char* filetypes,
										  bool save,
										  bool directories)
{
	std::stringstream ss;
	ss << "(message \"" << message << "\")(startdir \"" << start << "\")(filetypes \"" << filetypes << "\")";
	ss << "(save " << save << ")(directories " << directories << ")";

	auto result = sexpresso::parse(Tool::executeToolNonBlocking("filedialog", ss.str().c_str()));
	return result.getChildByPath("filename")->arguments().begin()->value.str;
}