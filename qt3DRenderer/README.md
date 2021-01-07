# qt3DRenderer

A complete 3D software renderer written in C++:

<img src="screenshot.gif" alt="Screenshot">

 It currently supports the following operations:

- Several vectors and matrices operations in 2D and 3D;
- Transforms for Model Space, World Space, Camera Space, Perspective Projection, Image Space and Screen Space;
- Back-face Culling;
- Frustum Clipping;
- Flat Shading;

Other supported features include:
- Loading vertices, faces and texture coordinates from Wavefront files;
- Loading external JPG/PNG texture images;

Its dependency on Qt is just to be able to load PNG/JPG textures and create the window that displays the pixels. 
A few minor changes are required in order to port the renderer to other GUI frameworks (SDL, GTK+, EFL, ...).

References:
- [Pikuma: 3D Graphics Programming](https://courses.pikuma.com/courses/learn-computer-graphics-programming)
