# Pinky Engine   ![Pinky](https://github.com/lSara-MM/Pinky_Engine/assets/93879867/ed391b11-5df6-4e11-956c-bab93c571d51)

[![PinkyEngine - Download](https://img.shields.io/static/v1?label=PinkyEngine&message=Download&color=blue&logo=github)](https://github.com/lSara-MM/Pinky_Engine/releases/download/v.1.0/Pinky_Engine_v.1.0.zip "Download Engine") 
[![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/lSara-MM/Pinky_Engine)

## About Pinky Engine

Pinky Engine is a Game Engine made by Andreu Nosàs Soler and Sara Qiao Martínez Mauri, as part of a college assignment from CITM (UPC) University, Terrassa, for the 3D engines subject during 3rd year of Game Design and Game Development degree.

![image](https://github.com/lSara-MM/Pinky_Engine/assets/93879867/3dbef10b-5e6b-4caf-b42c-89cf877f54ad)

## Team Members

| Sara Qiao Martínez Mauri | Andreu Nosàs Soler |
| ----------- | ----------- |
| [![lSara-MM - ✧](https://img.shields.io/static/v1?label=lSara-MM&message=✧&color=blue&logo=github)](https://github.com/lSara-MM "Go to GitHub repo") <br> - GameObjects <br>  - Custom File Format <br> - Resource Manager <br> - Scene Serialization <br>  - Engine Windows <br>  - Component Mesh <br>  - Component Material <br> - UI component pattern <br>  - UI Components | [![AndyCubico - ✻](https://img.shields.io/static/v1?label=AndyCubico&message=✻&color=blue&logo=github)](https://github.com/AndyCubico "Go to GitHub repo") <br> - Frustum & Frustum Culling <br> - Mouse Picking <br> - Component Camera <br> - Component Transform <br>  - Guizmos <br>  - Viewports <br> - UI Components <br> - UI logic <br> - Play/Stop mode


## Core Sub-Systems

### ↦ GameObject & Component System
Every entity is a _GameObject _ which can contain different type of components (Transform, Mesh, Material, UI, Camera).

When a model is imported to the scene, _GameObjects_ are created recursively for each mesh with _Transform_, _Mesh_ and _Material_ in case they have embedded textures.
An empy _GameObject_ only has _Transform_ but other components can be added afterwards.

### ↦ Resource Manager
Once a file is imported to the scene for the first time, save and load it from memory if needed again. Delete the resource once it's not used anymore. This way it's not used more memory than needed and the usage of the game is drastically reduced.

### ↦ Custom File Format
Save and load in an encrypted files to protect and optimize the process of importing and loading files such as textures and 3D models.

### ↦ Scene Serialization & Time management
Scene's _GameObject_'s structure is saved and loaded as .json using parson library.
This method is used in the time management as well. On _Play_, it saves a temporary file of the current scene and when _Stop_ it loads it back so the changes made during _Play_ time are not saved like in Unity.

### ↦ Camera
Scene rendering time was reduced with camera culling, camera frustum, and AABB boxes to discard all the meshes outside the camera view.


# High-level System: User Interface

System to render a variety of widgets on screen. It follows a component patern like the normal game objects, with each component inthereting from the base class C_UI. 

## UI Game Objects/Widgets: 

### ↦ Canvas 
Surface that represents the game window. What is inside of it will be rendered to game. It is automatically created when any UI Game Object is created and there is no canvas on scene.

### ↦ Image 
Displays an image. Options to change its colour and the texture attached to it
### ↦ Text 
Widget that renders the desired text ("Hello World" by default). Option to change the font colour and size.

### ↦ Button
It is formed by two Games Objects:
  * Text: if the user desires to input a text
  * Button: holds the logic of the button with the following states: Normal, Focusd, Pressed, Selected and Release
    * Option to change the colours of each state 
    * Option to change the colour of the texture
    * Option to change the texture
    * It can call custom functions (like the fade in the demo)
      
![engineGif2](https://github.com/lSara-MM/Pinky_Engine/assets/99950309/5e01bd9e-487e-4c09-b3fb-2761cf250ee2)

### ↦ Input Box
Allows player to input any text when selected :
  * Input box Game Object holds the logic and has the same states as button
  * Text Game Object to store the input text (and be able to move it around)
  * It has the option to put a character limit
    
### ↦ Checkbox 
Composed of multiple Game Objects:
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
   
 ![engineGif1](https://github.com/lSara-MM/Pinky_Engine/assets/99950309/aeeeec42-293a-43a0-9d52-6e0c3bf3bb00)


<iframe width="560" height="315" src="https://www.youtube.com/embed/dJmOgvL1sJY?si=99HFYbOXOEplq2HC" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>



## 5. References
- [defining-dialogue-systems](https://www.gamedeveloper.com/design/defining-dialogue-systems)

- [successful-in-game-dialogue-should-carry-players-forward](https://www.polygon.com/2014/3/17/5519270/successful-in-game-dialogue-should-carry-players-forward#:~:text=%22Conversations%20are%20a%20hallmark%20of,to%20new%20characters%20and%20locations)

- [video-game-dialogue-writing](https://gamedesignlounge.com/video-game-dialogue-writing/)

- [Realistic Dialogue Engine for Video Games](https://ir.lib.uwo.ca/cgi/viewcontent.cgi?referer=https://www.google.co.uk/&httpsredir=1&article=4141&context=etd)

- [A Summary of Research on Intelligent Dialogue Systems](https://iopscience.iop.org/article/10.1088/1742-6596/1651/1/012020/pdf#:~:text=Classification%20of%20dialogue%20systems&text=According%20to%20the%20purpose%20of,and%20small%2Dtalk%20dialogue%20system)
