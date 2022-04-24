import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import "perlin.js" as Perlin

Entity {
    id: root

    property Layer layer
    property vector3d pos:  Qt.vector3d(0, 0, 0)
    property color color: "green"

    property int width: 100
    property int height: 100
    property size meshResolution: Qt.size(80, 80)

    property var vertices: generateTerrain()                // fp array with (x,y,z) coordinates for all vertices
    property int vertexCount: 0
    //property var normals: generateNormals(root.vertices) // fp array with (x,y,z) coordinates of all normals
    property var normals: generateNormals_v2(root.vertices) // fp array with (x,y,z) coordinates of all normals

    property int maxHeight: 50     // maximum height of a vertex
    property int seed: 8           // seed for perlin noise
    property int stretchFactor: 50 // control curvature of the surface

    //components: [ customMesh, material, transform, root.layer ]
    components: [ customMesh, materialGLSL, transform, root.layer ]

    PlaneMesh {
        id: mesh
        width: root.width
        height: root.height
        meshResolution: root.meshResolution
    }

    // GeometryRenderer has a "geometry" property
    // Geometry has multiple Attribute(s) property
    // Attribute has a "buffer" property which contains the actual data
    // It's better to feed the buffer from C++
    // Ex: ex-pyramid-qml
    //
    // Alternative: indexed rendering (set attributeType to Attribute.IndexAttribute)
    // - Build one buffer with the unique vertex positions
    // - Build another buffer with indexes from the 1st buffer to define the triangles (counterclockwise)
    // Ex: ex-surface-function (look at normal computation)

    GeometryRenderer {
        id: customMesh
        primitiveType: GeometryRenderer.Triangles
        //primitiveType: GeometryRenderer.TriangleFan

        geometry: Geometry {

            //
            // Attribute: defines data for the vertices
            // (list of vertex positions, list of vertex colors, list of normals, list of texture coords)
            //

            Attribute {
                name: defaultPositionAttributeName // in vec3 vertexPosition
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float

                vertexSize: 3 // (x, y, z) components
                count: root.vertexCount // number of unique vertices

                byteOffset: 0 // first of the unit
                byteStride: vertexSize * Float32Array.BYTES_PER_ELEMENT   // (x, y, z) * size(float)

                buffer: Buffer {
                    type: Buffer.VertexBuffer
                    data: root.vertices
                }
            }

            // if you dont specify normals, not even PhongMaterial will work
            Attribute {
                name: defaultNormalAttributeName
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float

                vertexSize: 3
                count: root.normals.length // one normal per vertex
                byteOffset: 0
                byteStride: vertexSize * Float32Array.BYTES_PER_ELEMENT // (x, y, z) * size(float)

                buffer: Buffer {
                    type: Buffer.VertexBuffer
                    data: root.normals
                }
            }

        } //Geometry

    } // GeometryRenderer

    PhongMaterial {
        id: material
        ambient: Qt.rgba(0.2, 0.2, 0.2) // emitted by an object without any other light source.
        diffuse: root.color // emitted for rought surface reflections with the lights
        specular: Qt.rgba(0.3, 0.3, 0.3) // emitted for shiny surface reflections with the lights
        shininess: 1.0
    }

    PhongShadingMaterial {
        id: materialGLSL
        ambient: Qt.rgba(0.02, 0.02, 0.02, 1) // emitted by an object without any other light source.
        diffuse: Qt.rgba(0.0, 1.0, 0.0) // emitted for rought surface reflections with the lights
        specular: Qt.rgba(0.4, 0.4, 0.4, 1) // emitted for shiny surface reflections with the lights
        shininess: 10.0        
    }

    Transform {
        id: transform
        rotationX: 0
        translation: Qt.vector3d(root.pos.x, root.pos.y, root.pos.z)
    }

    /*
     * The returned array is a new Float32Array[] that contains a sequence of (x,y,z) values:
     * - Every 3 elements of the array define a vertex;
     * - It takes 3 vertices to define one triangle;
     * - 2 triangles are required to render a QUAD;
     *
     * The number of vertices in the returned array can be calculated by dividing its length by 3.
     */
    function generateTerrain() {
        // debug: use this simple QUAD for testing purposes
        //
        //      TOP VIEW               -z
        //                              |
        //                      D                C
        // (-50.0, 0.0, -50.0)  o----------------o  ( 50.0, 0.0, -50.0)
        //                      |                |
        //                      |       0,0      |
        //                      |                |
        // (-50.0, 0.0, 50.0)   o----------------o  ( 50.0, 0.0, 50.0)
        //                      A                B
        //                              |
        //                             +z
        //
//        let halfW = root.width / 2;
//        let halfH = root.height / 2;
//        var vertices = new Float32Array([
//            // bottom triangle: A, B, C
//            -halfW, 0.0,  halfH,        // A
//             halfW, 0.0,  halfH,        // B
//             halfW, 0.0, -halfH,        // C
//            // top triangle: A, C, D
//            -halfW, 0.0,  halfH,        // A
//             halfW, 0.0, -halfH,        // C
//            -halfW, 0.0, -halfH         // D
//        ]);
//        return vertices;

        console.log("generateTerrain: creating 2D points");
        let pts = flatPlane();
        root.vertexCount = pts.length;

        console.log("generateTerrain: randomizing 2D points");
        let terrainPts = updateTerrain(pts); // update y-component of the points

        console.log("generateTerrain: generating 3D mesh");
        let terrainMesh = generateMesh(terrainPts); // generate triangles

        console.log("generateTerrain: done");
        return terrainMesh;
    }

    // gridPlane: generates a list of 3D points that describe a 2D grid
    // starting from the bottom of the grid.
    function flatPlane() {
        let pts = [];

        let xInc = root.width / (root.meshResolution.width - 1);
        let zInc = root.height / (root.meshResolution.height - 1);
//        console.log("heightMap: xInc=", xInc, "zInc=", zInc);

        let xPos = -(root.width/2);
        let zPos =  (root.height/2);
//        console.log("heightMap: xPos=", xPos, "zPos=", zPos);

        for (let z = 0; z < root.meshResolution.height; ++z)
        {
            for (let x = 0; x < root.meshResolution.width; ++x)
            {
                let p = Qt.vector3d(xPos, 0, zPos);
//                console.log("heightMap: p.x=", p.x, "p.y=", p.y, "p.z=", p.z);

                // NOTE: could update the terrain elevation right here
                // but for simplicity purposes, we don't.

                // add new point to the list
                pts.push(p);
                xPos = xPos + xInc;
            }

            zPos = zPos - zInc; // decrement z-value
            xPos = -(root.width/2); // reset xPos
        }

        return pts;
    }

    // updateTerrain: changes the y-component of the pts using some random noise
    function updateTerrain(pts)
    {
        // initialize the seed for perlin noise generation
        Perlin.module.seed(root.seed);

        // dont transform the edge vertices
        const borderSize = 1;

        for (let z = borderSize; z < root.meshResolution.height-borderSize; ++z)
        {
            // each iteration generates 2 triangles
            for (let x = borderSize; x < root.meshResolution.width-borderSize; ++x)
            {
                let idx = x + root.meshResolution.width * z;

                // all noise functions return values in the range of -1 to 1
                // use Perlin.noise.simplex2 and Perlin.noise.perlin2 for 2d noise
                var newHeight = Perlin.module.simplex2(x / root.stretchFactor, z / root.stretchFactor);
                pts[idx].y = Math.abs(newHeight) * root.maxHeight;
            }
        }

        return pts;
    }

    // generateTriangles: creates the mesh (triangles) from a list of 3D points (vertices).
    // Consider this simple list with 6 points:
    //
    //   p[0]    p[1]    p[2]
    //    o-------o-------o
    //    |     . |     . |
    //    |   .   |   .   |
    //    | .     | .     |
    //    o-------o-------o
    //   p[3]    p[4]    p[5]
    //
    //
    // To create a mesh, we need to generate two triangles for each quad
    // using counter-clockwise winding:
    //   Triangle 1: p[3], p[4], p[1]
    //   Triangle 2: p[3], p[1], p[0]
    //   Triangle 3: p[4], p[5], p[2]
    //   Triangle 4: p[4], p[2], p[1]
    //
    function generateMesh(pts)
    {
        if (pts.length < 4)
            return new Float32Array([]);

        if (root.meshResolution.width <= 1 || root.meshResolution.height <= 1)
            return new Float32Array([]);

        const m = root.meshResolution.width;
        const n = root.meshResolution.height;
        const numVertices = m * n;
        const numEdges = (m-1) * n + (n - 1) * m; // square grid graph
        const numFaces = numEdges - numVertices + 1;
        const numTriangles = numFaces * 2;

        console.log("generateMesh: numVertices=", numVertices, "numEdges=", numEdges, "numFaces=", numFaces, "numTriangles=", numTriangles);

        // 9 elements per triangle: vertex1(x,y,z) + vertex2(x,y,z) + vertex3(x,y,z)
        var vertices = new Float32Array(numTriangles * 9);
        let v = 0;

        // the first elements of the list define the bottom vertices of the mesh
        //
        //   idx3    idx4    idx5
        //    o-------o-------o
        //    |     . |     . |
        //    |   .   |   .   |
        //    | .     | .     |
        //    o-------o-------o
        //  idx0     idx1    idx2
        //
        for (let z = 0; z < root.meshResolution.height-1; ++z)
        {
            // each iteration generates 2 triangles:
            //
            //   idx3    idx4
            //    o-------o
            //    |     . |         Triangle #1: idx0, idx1, idx3
            //    |   .   |         Triangle #2: idx1, idx4, idx3
            //    | .     |
            //    o-------o
            //  idx0     idx1
            for (let x = 0; x < root.meshResolution.width-1; ++x)
            {
                let idx = x + root.meshResolution.width * z;
                let nextIdx = (x + 1) + root.meshResolution.width * z;
                let topIdx = x + root.meshResolution.width * (z + 1);
                let topNextIdx = (x + 1) + root.meshResolution.width * (z + 1);

                //
                // Triangle #1
                //

//                console.log("generateTerrain: Triangle 1:")
//                console.log("generateTerrain: x=", x, "z=", z, "       idx=", idx, " point=", pts[idx]);
//                console.log("generateTerrain: x=", x, "z=", z, "   nextIdx=", idx, " point=", pts[nextIdx]);
//                console.log("generateTerrain: x=", x, "z=", z, "topNextIdx=", topNextIdx, " point=", pts[topNextIdx]);

                // p[3]
                vertices[v++] = pts[idx].x;
                vertices[v++] = pts[idx].y;
                vertices[v++] = pts[idx].z;

                // p[4]
                vertices[v++] = pts[nextIdx].x;
                vertices[v++] = pts[nextIdx].y;
                vertices[v++] = pts[nextIdx].z;

                // p[1]
                vertices[v++] = pts[topNextIdx].x;
                vertices[v++] = pts[topNextIdx].y;
                vertices[v++] = pts[topNextIdx].z;

                //
                // Triangle #2
                //

//                console.log("generateTerrain: Triangle 2:")
//                console.log("generateTerrain: x=", x, "z=", z, "        idx=", idx, " point=", pts[idx]);
//                console.log("generateTerrain: x=", x, "z=", z, " topNextIdx=", topNextIdx, " point=", pts[topNextIdx]);
//                console.log("generateTerrain: x=", x, "z=", z, "     topIdx=", topIdx, " point=", pts[topIdx]);

                // p[3]
                vertices[v++] = pts[idx].x;
                vertices[v++] = pts[idx].y;
                vertices[v++] = pts[idx].z;

                // p[1]
                vertices[v++] = pts[topNextIdx].x;
                vertices[v++] = pts[topNextIdx].y;
                vertices[v++] = pts[topNextIdx].z;

                // p[0]
                vertices[v++] = pts[topIdx].x;
                vertices[v++] = pts[topIdx].y;
                vertices[v++] = pts[topIdx].z;
            }
        }

        return vertices;
    }  

    // generateNormals: since a normal is defined as being the vector perpendicular
    // to all vectors within a given plane (in N dimensions), we need a plane to calculate a normal.
    // This means that normals are per face which is calculated as the cross product of the faces edges.
    // https://gamedev.stackexchange.com/a/178608/3078
    // https://stackoverflow.com/a/6661242/176769

    // A triangle with points A, B, C, has position vectors
    //   ↑A, ↑B, ↑C
    // and the edges have vectors:
    //   ↑B - ↑A and ↑C - ↑A
    // the normal vector is given by:
    //   ↑Nf = (↑B - ↑A) × (↑C - ↑A)
    function normalFromVertex(vertexA, vertexB, vertexC) {
        let edgeBA = vertexB.minus(vertexA);
        let edgeCA = vertexC.minus(vertexA);
        let normal = edgeBA.crossProduct(edgeCA);
        let sinAlpha = normal.length() / (edgeBA.length() * edgeCA.length());
        return normal.normalized().times(Math.asin(sinAlpha));
    }

    // normals per vertex: super expensive
    // https://stackoverflow.com/a/18520394/176769
    function generateNormals(vertices) {
        console.log("generateNormals: START meshResolution=", root.meshResolution, "vertices.length=", vertices.length);
        const startTime = new Date().getTime();

        // create output array
        var normals = new Float32Array(vertices.length);

        // each vertex takes 3 (x,y,z) components and therefore
        // a triangle (3 vertex * 3 components) requires 9 elements from the list
        const vertexSize = 3;
        const vertexComponents = 3;
        const triangleSize = vertexSize * vertexComponents; // 9
        const faceSize = triangleSize * 2;

        const numTriangles = vertices.length / triangleSize;

        // each iteration processes a vertex and calculates its normal
        for(let v = 0; v < vertices.length; v += vertexSize) {
            let vertex = Qt.vector3d(vertices[v], vertices[v+1], vertices[v+2]);
            let vertexNormal = Qt.vector3d(0, 0, 0);

            // each iteration of the inner loop goes through all triangles of the mesh (super expensive)
            // that adjusts the vertex normal a little bit for a more realistic lighting effect
            for (let t = 0; t < numTriangles; ++t)
            {
                const baseIdx = t * triangleSize;
                let triangleVertexA = Qt.vector3d(vertices[baseIdx], vertices[baseIdx+1], vertices[baseIdx+2]);
                let triangleVertexB = Qt.vector3d(vertices[baseIdx+3], vertices[baseIdx+4], vertices[baseIdx+5]);
                let triangleVertexC = Qt.vector3d(vertices[baseIdx+6], vertices[baseIdx+7], vertices[baseIdx+8]);

                // does this triangle contains the current vertex?
                if (vertex.fuzzyEquals(triangleVertexA) || vertex.fuzzyEquals(triangleVertexB) || vertex.fuzzyEquals(triangleVertexC))
                    vertexNormal = vertexNormal.plus(normalFromVertex(triangleVertexA, triangleVertexB, triangleVertexC));
            }

            // update normal for each vertex
            vertexNormal = vertexNormal.normalized();
            normals[v] = vertexNormal.x;
            normals[v+1] = vertexNormal.y;
            normals[v+2] = vertexNormal.z;
        }

        const elapsedTime = new Date().getTime() - startTime;
        console.log("generateNormals: END elapsed time:", elapsedTime / 1000.0, "(s)");

        return normals;
    }

    // normals per triangle: if using indexed buffers, check this implementation:
    // https://gamedev.stackexchange.com/a/178608/3078
    function generateNormals_v2(vertices) {
        console.log("generateNormals_v2: START meshResolution=", root.meshResolution, "vertices.length=", vertices.length);
        const startTime = new Date().getTime();

        var normals = new Float32Array(vertices.length);

        // each vertex takes 3 (x,y,z) components and therefore
        // a triangle (3 vertex * 3 components) requires 9 elements from the list
        const vertexSize = 3;
        const vertexComponents = 3;
        const triangleSize = vertexSize * vertexComponents; // 9
        const faceSize = triangleSize * 2;
        const numTriangles = vertices.length / triangleSize;

        for (let t = 0; t < numTriangles; ++t)
        {
            const idx = t * triangleSize;

            //    C
            //    o
            //    |`.
            //    |  `.
            //    |    `.
            //    o------o
            //    A      B
            //
            let vertexA = Qt.vector3d(vertices[idx], vertices[idx+1], vertices[idx+2]);
            let vertexB = Qt.vector3d(vertices[idx+3], vertices[idx+4], vertices[idx+5]);
            let vertexC = Qt.vector3d(vertices[idx+6], vertices[idx+7], vertices[idx+8]);

            let edgeBA = vertexB.minus(vertexA);
            let edgeCA = vertexC.minus(vertexA);

            // causes weird serrilhated colors in the valley
            let areaWeightedNormal = edgeBA.crossProduct(edgeCA).normalized();

            // vertexA
            normals[idx] = areaWeightedNormal.x;
            normals[idx+1] = areaWeightedNormal.y;
            normals[idx+2] = areaWeightedNormal.z;

            // vertexB
            normals[idx+3] = areaWeightedNormal.x;
            normals[idx+4] = areaWeightedNormal.y;
            normals[idx+5] = areaWeightedNormal.z;

            // vertexC
            normals[idx+6] = areaWeightedNormal.x;
            normals[idx+7] = areaWeightedNormal.y;
            normals[idx+8] = areaWeightedNormal.z;
        }

        const elapsedTime = new Date().getTime() - startTime;
        console.log("generateNormals_v2: END elapsed time:", elapsedTime, "(ms)");

        return normals;
    }
}
