INCLUDE(CopyFiles)

copy_files("*.*" "${NEO_BINARY_OUTPUT}/gui" "GuiFiles")
copy_files("meshs/*.*" "${NEO_BINARY_OUTPUT}/gui" "MeshFiles")
copy_files("default/*.*" "${NEO_BINARY_OUTPUT}/gui" "DefaultTheme")
copy_files("icons/*.*" "${NEO_BINARY_OUTPUT}/gui/icons" "Icons")
