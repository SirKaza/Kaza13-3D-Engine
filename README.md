# **Kaza13**: A 3D Game Engine

## **Overview**

Kaza13 is a custom engine developed as a part of the master in advanced programming in AAA video games at UPC. This engine facilitates the creation of 3D video games with key features like and editor with texture management, model rendering, user input handling, window management within others. It is built using C++, with SDL and OpenGL as main libraries.

## **Author**

- Marc Casanova Torrequebrada.

## **GitHub Repository**

[GitHub Repository](https://github.com/SirKaza/Kaza13-3D-Engine)

## **License**

This project is licensed under the [MIT License](LICENSE).

## **How to Use the Engine**

In case you want to open the project and run in an IDE:
Prerequisites: Visual Studio Community 2019.
1. Clone the repository:
   - Download or clone the repository from GitHub.
2. Open the `MyZork.sln` solution file in Visual Studio.
3. Build the project and run the game.

## **Controls and Actions**

### **Camera Controls**
| **Action**         | **Key / Control**             | **Description**                         |
|--------------------|-------------------------------|-----------------------------------------|
| **Move camera**    | (W, A, S, D) + right click    | Moves the camera in the scene           |
|                    | Middle mouse button + move    |                                         |
| **Rotate camera**  | Right mouse button + move     | Rotates the camera's view               |
| **Zoom**           | Mouse wheel                   | Zooms in/out                            |
|                    | Left Alt + right click + move |                                         |

### **Editor**

Log window, Console window, Configuration window, About window and Exit app.

Configuration window:
- Application: see app info and limit fps with fps and ms histogram. Limit fps depends on Vsync.
- Window: switch brightness, fov, width and height of main window. Also, see refresh rate of monitor. Finally, enable/disable fullscreen, borderless, resizable, full desktop and vsync.
- Texture: see texture specs and switch wrap mode, mag and min filter and mipmaps.
- Hardware: see hardware specs.

## **Versions Used**

- IDE: 
  - Visual Studio 2019 Community: 16.11.39
- Language:
  - C++: standard ISO C++17
- Libraries:
  - SDL: 2
  - OpenGL: 4.6.0
  - Glew: 2.1.0
  - Dear ImGui: 1.91.5
  - DirectXTex: 
