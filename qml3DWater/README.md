# qml3DWater

Qt3D/QML implementation of water reflection/refraction using clipping planes. 

The water is just a simple a `PlaneMesh` entity and most of the work is done by `water.frag`:

- DuDv map is used to create distortions on the water surface;
- The Fresnel effect allows the water to be more transparent when looking from above;
- Specular highlights are based on a Normal map;
- The mesh of the water is not changed at all by the vertex shader;

<img src="screenshot.gif" alt="Screenshot" width="400" height="400">

**References**:

- [OpenGL Water Tutorials](https://www.youtube.com/playlist?list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh)