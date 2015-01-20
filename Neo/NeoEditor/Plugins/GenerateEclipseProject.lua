setPluginName("Generate Eclipse project")
setPluginAuthor("Yannick Pflanzer")
setPluginDescription("A plugin that creates an Eclipse project for easy scripting.")

addEditorMenu("Generate Project", "generate_callback")

require("plugins.libs.misc")

default_project = 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
<projectDescription> \
	<name>$PROJECT_NAME</name> \
	<comment></comment> \
	<projects> \
	</projects> \
	<buildSpec> \
		<buildCommand> \
			<name>org.eclipse.dltk.core.scriptbuilder</name> \
			<arguments> \
			</arguments> \
		</buildCommand> \
	</buildSpec> \
	<natures> \
		<nature>org.eclipse.ldt.nature</nature> \
	</natures> \
</projectDescription>"

function generate_callback()

	local projectDir = getProjectDir()
	if projectDir:len() == 0 then
		messagebox("You have to load a project first!")
		return
	end
	
	-- TODO: Ask user for name!
	default_project = default_project:gsub("$PROJECT_NAME", "NeoScriptProject")
	
	local projectFile = io.open(projectDir .. ".project", "w")
	projectFile:write(default_project)
	projectFile:flush()
	projectFile:close()
end
