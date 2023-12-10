# Pinky Engine

3D Game engine done by Sara Qiao Martínez Mauri and Andreu Nosàs Soler for the Game Engines subject in the CITM bachelor's degree in Video Game Design and Development.

# Controls

* Right Click + W/A/S/D: FPS camera controls
* Right Click + Q/E: Go down and up
* Left Alt + Left Click: Orbital camera around selected Game Object
* F: Focus on selected Game Object
* Mouse Wheel: Zoom In/Out
* Left shift: Multiply movement and zoom speed
* Left control + Left click : Select multiple Game Objects in the Hierarchy
* Del/backspace key: delete selected Game Objects
* Guizmo modes:
	* Translation: W
	* Rotation: E
	* Scale: R

# Features

* Load and draw FBX files by dragging them to the engine from anywhere in the Hard Drive
* Load and apply DDS/PNG textures by dragging them to the engine from anywhere in the Hard Drive
* Game Object structure, one for each loaded mesh. Inspector shows the following:
	* Each GameObject can have 4 components: Transform, Mesh, Material and Camera
		* Transform component that allows to translate, rotate and scale Game Objects, both with the inspector and guizmos
		* Mesh component: 
			* Allows to show vertex normals, face normals of the geometry, OBB, and global AABB
			* Option to change the mesh for another that is in resources, if there is only one 
			instance of the previous one it will go away until it is imported again.
		* Material component allows to change the color (with a color picker) and a toggle to show the checkers
		* Camera component:
			* User can modify the near plane, far plane and FOV
			* Game camera checkbox to enable or disable if the component is the rendering game camera
			* Meshes can be discarded using Frustum Culling, checkbox to enable or disable
	* User can add mesh (choose a primitive shape), material and camera (can't add more than one of each for each Game Object, like in Unity Engine)
	* Mesh and material component can be deleted pressing the cross icon
	* Toggle to enable and disable Game Object, it will not render and will appear slightly turned off in the hierarchy
	* Clicking the name allows the user to change it (press enter to save it and esc to cancel)
	* GameObjects can be picked from the world using the mouse
---------------------
* Resource and CFF
---------------------
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
	* Search option to look for messages with the types introduced
* Play time: 
	* User can user can Start / Pause / Stop / Step the simulation and recover its original state
	* 2 timers show the real time since the engine started and the game time since the simulation started (play button)
* Cameras:
	* There are 2 windows: one with editor view and another withe the game view
	* Game view is captured with a Game Object and can be changed
	* Each one has its own framebuffer and the user can visualize both at the same time

# Menus

* File:
	* Options allow the user to change the theme of the UI: classic, light and dark
	* Save button to save current scene
	* Save As... button to save current scene with a name
	* Exit button to close the application
* Project:
	* Shows the folder containing all the resources used by the editor
	* By right clicking user can import the asset to Scene or delete it (select Game Object to import textures)
* Resource:
	* Displays UID, name and reference count of each resource
	* User can delete them
* Game Object:
	* Option to create an empty Game Object with onlty a trasnform component
	* Option to create a primitive shape (cube, sphere, cylinder and plance)
	* Option to create one of the custom meshes (a fox, King Shark and Kuro)
* Play Time:
	* Play to start the simulation and save current scene
	* Stop to recover original state and restart timer
	* Pause to pause simulation
	* Step to pause simulation and advance time equal to Play speed
	* Slider Play Speed to adjust how fast game time passes
* Window:
	* Show and hide the hierarchy window
	* Show and hide the inspector window
	* Show and hide the project window
	* Show and hide the console window
	* Show and hide the resources window
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
* Raycast toggle to debug the raycast used in mouse picking

# Libraries Used

* [ImGUI](https://github.com/ocornut/imgui)
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
* [PhysFS](https://icculus.org/physfs/)
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