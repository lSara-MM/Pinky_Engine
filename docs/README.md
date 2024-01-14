# Pinky Engine
C++ Game Engine

[![PinkyEngine - Download](https://img.shields.io/static/v1?label=PinkyEngine&message=Download&color=blue&logo=github)](https://github.com/lSara-MM/Pinky_Engine/releases/download/v.0.2/Pinky_Engine_v.0.2.zip "Download Engine") <t> [![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/lSara-MM/Pinky_Engine)

## About Pinky Engine

Pinky Engine is a Game Engine made by Andreu Nosàs Soler and Sara Qiao Martínez Mauri, as part of a college assignment from CITM (UPC) University, Terrassa, for the 3D engines subject during 3rd year of Game Design and Game Development degree.

![Pinky](https://github.com/lSara-MM/Pinky_Engine/assets/93879867/ed391b11-5df6-4e11-956c-bab93c571d51)

![image](https://github.com/lSara-MM/Pinky_Engine/assets/93879867/3dbef10b-5e6b-4caf-b42c-89cf877f54ad)

## Team Members

| Sara Qiao Martínez Mauri | Andreu Nosàs Soler |
| ----------- | ----------- |
| ![Fallout series](https://external-preview.redd.it/f8P_i_fMd-mPtdVyRiFMKOy88ciPVew4CGU9YmJ_W7s.jpg?width=640&crop=smart&auto=webp&s=51194a7f6424204858a10b688b1b05565caa826f) | ![Mass Effect 2007](http://cdn3.whatculture.com/wp-content/uploads/2015/11/3ehM1gNO.gif)|
| [![lSara-MM - ✧](https://img.shields.io/static/v1?label=lSara-MM&message=✧&color=blue&logo=github)](https://github.com/lSara-MM "Go to GitHub repo") <br> - GameObjects <br>  - Custom File Format <br> - Resource Manager <br> - Scene Serialization <br>  - Engine Windows <br>  - Component Mesh <br>  - Component Material <br>  - UI Components | [![AndyCubico - ✻](https://img.shields.io/static/v1?label=AndyCubico&message=✻&color=blue&logo=github)](https://github.com/AndyCubico "Go to GitHub repo") <br> - Frustum & Frustum Culling <br> - Mouse Picking <br> - Camera <br> - Component Transform <br>  - UI Components <br>  - Viewports <br> - Gizmos <br> - Play/Stop mode


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

## 5. References
- [defining-dialogue-systems](https://www.gamedeveloper.com/design/defining-dialogue-systems)

- [successful-in-game-dialogue-should-carry-players-forward](https://www.polygon.com/2014/3/17/5519270/successful-in-game-dialogue-should-carry-players-forward#:~:text=%22Conversations%20are%20a%20hallmark%20of,to%20new%20characters%20and%20locations)

- [video-game-dialogue-writing](https://gamedesignlounge.com/video-game-dialogue-writing/)

- [Realistic Dialogue Engine for Video Games](https://ir.lib.uwo.ca/cgi/viewcontent.cgi?referer=https://www.google.co.uk/&httpsredir=1&article=4141&context=etd)

- [A Summary of Research on Intelligent Dialogue Systems](https://iopscience.iop.org/article/10.1088/1742-6596/1651/1/012020/pdf#:~:text=Classification%20of%20dialogue%20systems&text=According%20to%20the%20purpose%20of,and%20small%2Dtalk%20dialogue%20system)
