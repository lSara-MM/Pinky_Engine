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
      
![buttonShow](https://github.com/lSara-MM/Pinky_Engine/assets/99950309/fd209ec1-f3c2-4d4a-9bc1-8a0982685493)

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



## License
MIT License

Copyright (c) 2023 lSara-MM

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
