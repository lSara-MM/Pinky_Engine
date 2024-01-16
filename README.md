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
* Resources and CFF
	* Models, meshes and textures are saved with our own format under Library folder
	* Scene can be serialized to a file that can be loaded
	* Upon start, all resources not managed inside Assets are generated in Library 
	* Library folder regenerates from Assets + meta content
	* All resources use reference counting
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
	* Button to clear the console
	* Search option to look for messages 
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
	* Option to create one of the UI elements
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

# High-level System
* System to render a variety of widgets on screen
* UI Game Objects/Widgets: 
	* Canvas: 
		* surface that represents the game window. What is inside of it will be rendered to game 
		* automatically created when any UI Game Object is created and there is no canvas on scene
	* Image: displays an image. Options to change its colour and the texture attached to it
	* Text: renders the desired text ("Hello World" by default). Option to change the font colour and size
	* Button: it is formed by two Games Objects:
		* Text: if the user desires to input a text
		* Button: holds the logic of the button with the following states: Normal, Focusd, Pressed, Selected and Release
			* Option to change the colours of each state 
			* Option to change the colour of the texture
			* Option to change the texture
			* It can call custom functions (like the fade in the demo) 
	* Input Box: allows player to input any text when selected :
		* Input box Game Object holds the logic and has the same states as button
		* Text Game Object to store the input text (and be able to move it around)
	* Checkbox: composed of multiple Game Objects:
		* Checkbox as the parent of the rest holds the logic to disable the checkmark and execute a desired function
		* Text if user wants to put a label to the widget
		* Background behind the checkmark
		* Checkmark, changes when the Checkbox parent is changed
		* As with button, option to change the colours of the states and of the textures
* All of the UI Game Objects can be navigated through with tab (change the selected one)
* Logic of all the UI works only in play mode
* Buttons allow input from the keyboard (spacebar)
* There is a demo that shows the functionality of all the Widgets:
	* Main menu with a fullscreen background, with an imput box and a button to start, when pressed fades away
	to the next scene
	* Next scene has the town of the second assignment and a crosshair in the middle of the screen
	* Pressing F1 in this scene opens up a menu, with a checkbox to drag the window and to toggle Vsync

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