# qml3DTerrain

Qt3D/QML implementation of dynamic terrain generation based on the Perlin noise:

<img src="screenshot.gif" alt="Screenshot" width="400" height="400">

Terrain generation and the wireframe looks are both transformations done by GLSL shaders on the meshes of a `PlaneMesh` entity.

**References**:

- [Qt 3D: Wireframe QML Example](https://doc.qt.io/qt-5/qt3d-wireframe-example.html).
- [The Coding Train - Coding Challenge #11: 3D Terrain Generation with Perlin Noise in Processing](https://www.youtube.com/watch?v=IKB1hWWedMk)
- [Generic Noise Algorithms in GLSL](https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83)
