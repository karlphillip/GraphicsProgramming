# qt3DRenderer

A C++ implementation of a complete 3D renderer that offers a graphics pipeline with the following operations:

- Transforms for Model Space, World Space, Camera Space, Perspective Projection, Image Space and Screen Space;
- Back-face Culling;
- Frustum Clipping;

Other supported features include:
- Loading vertices, faces and texture coordinates from simple Wavefront files;
- Loading external JPG/PNG texture images;

<img src="screenshot.gif" alt="Screenshot" width="400" height="200">

Its dependency on Qt is just to be able to load PNG/JPG textures and create the window that displays the pixels from the Color Buffer. Minor changes are required to port the renderer to other GUI frameworks (SDL, GTK+, EFL, ...).