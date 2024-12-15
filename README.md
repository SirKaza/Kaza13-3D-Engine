# **Kaza13**: 3D Game Engine

## **Overview**

Kaza13 is a custom engine developed as a part of the master in advanced programming in AAA video games at UPC. This engine facilitates the creation of 3D video games with key features like an editor with texture management, geometry loader rendering, user input handling, window management within others. It is built using C++, with SDL and OpenGL as main libraries.

## **Author**

- Marc Casanova Torrequebrada.

## **GitHub Repository**

[GitHub Repository](https://github.com/SirKaza/Kaza13-3D-Engine)

## **License**

This project is licensed under the [MIT License](LICENSE).

## **How to Use the Engine**

In case you want to open the project and run in an IDE (Prerequisites: Visual Studio Community 2019):
1. Clone the repository:
   - Download or clone the repository from GitHub.
2. Open the `MyZork.sln` solution file in Visual Studio.
3. Build the solution and run the game.

## **Controls and Actions**

### **Camera Controls**
| **Action**         | **Key / Control**             | **Description**                         |
|--------------------|-------------------------------|-----------------------------------------|
| **Move camera**    | (W, A, S, D) + right click    | Moves the camera in the scene           |
|                    | Middle mouse button + move    |                                         |
| **Rotate camera**  | Right mouse button + move     | Rotates the camera's view (free camera) |
| **Zoom**           | Mouse wheel                   | Zooms in/out                            |
|                    | Left Alt + right click + move |                                         |
| **Focus**          | F                             | Focus camera around the geometry        |
| **Orbital**        | Left Alt + left click + move  | Orbitates camera around the geometry    |

### **Editor**

Editor sections are the following:
  - Console: Display all the output from the application, the creation of the game engine, and different states or changes that can be applied, such as new models/textures. This console has commands (clear, history, help) and buttons (clear, copy) that clears the console or copies all outputs to clipboard. Additionally, has some filters for the output.
  - Configuration:
    - Application: See app info and limit FPS with FPS and MS histogram. Limit FPS depends on Vsync.
    - Window: Switch brightness, FOV, width, and height of the main window. Also, see the refresh rate of the monitor. Finally, enable/disable fullscreen, borderless, resizable, full desktop, and vsync.
    - Texture: For each texture, switch wrap mode, mag and min filter, and mipmaps.
    - Hardware: See hardware specs.
  - Properties: Display of details about Geometries and Textures that are being used.
  - About: Information about the engine, such as licenses, github, libraries and build/config.
  - Exit: Option to finish the game engine loop and quit the app.

## **Comments**

Available models with their corresponding textures:
  - Triangle
  - TriangleWithoutIndices
  - Box
  - BoxInterleaved
  - BoxTextured
  - BakerHouse
  - Duck
  - CesiumMilkTruck

Also, Baboon.png texture is included because is nice for seeing texture filters in different models.

This game engine loads and renders colors and diffuse textures in ModuleTextures.

Camera controls are limited between each order for a fluid experience and movement.

Rotation angle limit is 90 degrees for Z axis (45 degrees up and down) so user always does not desorientate.

Memory leaks were tested with a library called Visual Leak Detector and with Visual Studio itself.

Strings are used only used when necessary.

## **Versions Used**

- IDE: 
  - Visual Studio 2019 Community: 16.11.39
- Language:
  - C++: standard ISO C++17
- Libraries:
  - SDL: 2.0.16
  - OpenGL: 4.6.0
  - Glew: 2.1.0
  - Dear ImGui: 1.91.5
  - DirectXTex: 2.0.6
  - TinyGLTF: 2.9.3