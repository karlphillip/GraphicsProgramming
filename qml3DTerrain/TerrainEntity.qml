import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Logic 2.12
import Qt3D.Extras 2.12

Entity {
    id: root
    property var cols : 2
    property var rows : 2
    property real xoff
    property real yoff

    /* validateRange: doesn't let the total number of indices in a mesh be larger
     * than quint16 (65536).
     *
     * const int faces = 2 * (resolution.width() - 1) * (resolution.height() - 1);
     * const int indices = 3 * faces;
     * Q_ASSERT(indices < std::numeric_limits<quint16>::max());
     */
    function validateRange(c, r, returnCols) {
        let faces = 2 * (c - 1) * (r - 1);
        let indices = 3 * faces;
        if (indices >= 65536) {
            return 105;
        }

        if (returnCols)
            return c;

        return r;
    }

    function evalMeshSize() {
        //console.log("evalMeshSize: c=", root.cols, "r=", root.rows);
        let cols = validateRange(root.cols, root.rows, true);
        let rows = validateRange(root.cols, root.rows, false);
        //console.log("evalMeshSize: cols=", cols, "rows=", rows);
        return Qt.size(cols, rows);
    }

    PlaneMesh {
        id: customMesh
        width: 10.0
        height: 20.0
        meshResolution: evalMeshSize()
    }

    Transform {
        id: transform
        translation: Qt.vector3d(0.0, 0.0, 0.0)
        rotationX: 10 // 90
    }

    PhongAlphaMaterial {
        id: material
        ambient: "blue"
        diffuse: "blue"
        alpha: 0.8
    }

    WireframeMaterial {
        id: wireframeMaterial
        yoff: root.yoff

        ambient: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
        diffuse: Qt.rgba( 0.8, 0.8, 0.8, 1.0 )
        effect: WireframeEffect { }
    }

    //components: [ customMesh, transform, material ]
    components: [ customMesh, transform, wireframeMaterial ]
}
