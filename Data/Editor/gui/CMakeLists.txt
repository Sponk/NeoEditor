INCLUDE(CopyFiles)

copy_files("*.*" "${MARATIS_BINARY_OUTPUT}/gui" "GuiFiles")
copy_files("meshs/*.*" "${MARATIS_BINARY_OUTPUT}/gui" "MeshFiles")
copy_files("default/*.*" "${MARATIS_BINARY_OUTPUT}/gui" "DefaultTheme")
