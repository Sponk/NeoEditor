The Neo Editor
==============

Neo is a 3D scene editor for creating stunning video games and simulations for Linux and Windows. It is based on a modified version of the Maratis3D engine to deliver an easy to use and yet powerful interface to work with. It is compatible with the official Maratis3D editor in the sense that you can use and edit existing levels and projects without needing to change or convert them. The package also includes an modified player which makes it possible to benefit from all new features added to the engine during the development of Neo. The player is source compatible with all Lua scripts for Maratis3D except for the joystick support. Note that this player is not binary compatible with existing Maratis game plugins! That means that it is necessary to recompile the plugin using the packaged SDK which is no problem since the API did not change.

Compilation
===========

Neo uses a CMake based build system for creating IDE projects, Makefiles and packages for various Linux distributions and Windows. Simply use CMake to create the buildsystem and compile using the generated project/Makefile.
This version uses LuaJIT as the standard LUA solution.
You can compile the engine to use Lua instead of LuaJIT by adding "-DSTANDARD_LUA=true" to your CMake call.

Terms and conditions
====================

This project is free software. Free not as in "free beer" but as in "free speech". That means that the four fundamental freedoms are granted to you as the user.

See also: https://www.gnu.org/philosophy/free-sw.html
Neo is free software licensed under the terms and conditions of the GNU GPL license: https://www.gnu.org/copyleft/gpl.html
The player is also licensed under the terms and conditions of the GNU GPL license.
The Maratis3D core engine is licensed under the terms and conditions of the zlib/libpng license: http://opensource.org/licenses/zlib-license.php

Note that the GPL is a copyleft license which means that you can not use sourcecode from Neo or the player to create proprietary software! You are allowed to create proprietary C++ game plugins since it only interacts with the zlib licensed SDK. Lua scripts are also allowed to be proprietary.