# Pinky Engine

3D Game engine done by Sara Qiao Martínez Mauri and Andreu Nosàs Soler for the Game Engines subject in the CITM bachelor's degree in Video Game Design and Development.

# Controls

* Right Click + W/A/S/D: FPS camera controls
* Right Click + Q/E: Go down and up
* Left Alt + Left Click: Orbital camera around (0,0,0)
* F: Focus on coordinates (0,0,0)
* Mouse Wheel: Zoom In/Out
* Left shift: Multiply movement and zoom speed
* Left control + Left click : Select multiple Game Objects in the Hierarchy
* Del/backspace key: delete selected Game Objects

# Features

* Load and draw FBX files by dragging them to the engine from anywhere in the Hard Drive
* Load and apply DDS/PNG textures by dragging them to the engine from anywhere in the Hard Drive
* Game Object structure, one for each loaded mesh. Inspector shows the following:
	* Each GameObject can have 3 components: Transform, Mesh and Material
		* Mesh component allows to show vertex normals and face normals of the geometry
		* Material component allows to change the color (with a color picker) and a toggle to show the checkers
	* User can add mesh (choose a primitive shape) and material components (can't add more than one of each, like in Unity Engine)
	* Mesh and material component can be deleted pressing the cross icon
	* Toggle to enable and disable Game Object, it will not render and will appear slightly turned off in the hierarchy
	* Clicking the name allows the user to change it (press enter to save it and esc to cancel)
* Hierarchy:
	* User can fold and unfold the children of a parent
	* Multiple selection of Game Objects
	* Delete Game objects from the engine deletes them from the hierarchy
	* Move Game Object to change its parent
	* Right click Game Object allows to create a child with a primitive shape or empty

* Console Window:
	* Show the geometry and texture loading process
	* Show the libraries initialization process
	* Special messages for errors and warnings (highlighted in red and yellow)
	* Button to clear the console.
## Menus

* File:
	* Options allow the user to change the theme of the UI: classic, light and dark
	* Exit button to close the application
* Game Object:
	* Option to create an empty Game Object with onlty a trasnform component
	* Option to create a primitive shape (cube, sphere, cylinder and plance)
	* Option to create one of the custom meshes (a fox, King Shark and Kuro)
* Window:
	* Show and hide the hierarchy window
	* Show and hide the inspector window
* Help:
	* Module settings
		* Configuration window
			* Change window size
			* Change screen brightness
			* Show FPS information
				*Enable and disable FPS cap, and change the value
				*Enable and disable VSync
			* Show reported memory
		* OpenGl options
		* Change UI settings
	* Information output: show Hardware information
	* About window: License information and three additional windows:
		* Pinky Engine: team members and s short description, with link to the Github repository
		* Libraries used, with link to their pages
		* External sources: wip, credit creators of ImGUI layouts that we are planning to use 
* Wireframe toggle to enable and disable showing the wireframe of all meshes

# Libraries Used

* [ImGUI](https://github.com/ocornut/imgui)
* [OpenGL](https://glew.sourceforge.net/)
* [MathGeoLib](https://github.com/juj/MathGeoLib)
* [Parson](https://github.com/kgabis/parson)
* [Assimp](https://www.assimp.org/)
* [DevIL](https://openil.sourceforge.net/)
* [SDL](https://www.libsdl.org/index.php)
* [mmgr](https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml)
* [Deviceld](https://github.com/GameTechDev/gpudetect)

# Github links

* [Repository](https://github.com/lSara-MM/Pinky_Engine)
* [Releases](https://github.com/lSara-MM/Pinky_Engine/releases)
* [Sara Qiao Martínez Mauri](https://github.com/lSara-MM)
* [Andreu Nosàs Soler](https://github.com/AndyCubico)