import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0
import Dragly 1.0

Viewport {
    property alias billboard: mainBillboard
    property alias positionReader: mainPositionReader
    property alias multiplier: mainDensityPlotter.multiplier
    property alias useSquareRootDensity: mainDensityPlotter.useSquareRootDensity
//    property real multiplier
//    property bool useSquareRootDensity
    property Sphere pickedSphere: null
    fillColor: "black"
    picking: false

    light: Light {
        ambientColor: Qt.rgba(1,1,1,1)
        position.x: myCamera.eye.x / 2
        position.y: myCamera.eye.y / 2
        position.z: myCamera.eye.z / 2
        quadraticAttenuation: 0.0025
    }

    camera: Camera {
        id: myCamera
        eye: Qt.vector3d(30,25,19)
        nearPlane: 5
        farPlane: 100
    }

    PositionReader {
        id: mainPositionReader
        fileName: "/home/svenni/Dropbox/projects/programming/honkytonk/anti/positions.bin"
    }


    MultiBillboard {
        id: mainBillboard
//                    sortPoints: Item3D.BackToFront
        useAlphaTest: true
        effect: Effect {
            blending: true
            texture: "particle.png"
        }
        positionReader: mainPositionReader
    }

    Item3D {
        id: volumeItem
        property double maxMinDifference: positionReader.voxelEdgeMax - positionReader.voxelEdgeMin
        cullFaces: Item3D.CullBackFaces
        scale: maxMinDifference
        mesh: Mesh {
            source: "cube.obj"

        }
        effect: VolumeShaderProgram {
            id: mainDensityPlotter
            property real multiplier: 100
            property bool useSquareRootDensity: false
            blending: true
            vertexShaderSource: "scalarvolume.vert"
            fragmentShaderSource: "scalarvolume.frag"
            positionReader: mainPositionReader
        }
    }

    focus: true

    Keys.onPressed: {
        if(event.key === Qt.Key_W) {
            var direction = myCamera.center.minus(myCamera.eye)
            direction = direction.normalized()
            myCamera.eye = myCamera.eye.plus(direction)
            myCamera.center = myCamera.center.plus(direction)
            event.accepted = true
        }
        if(event.key === Qt.Key_S) {
            var direction = myCamera.center.minus(myCamera.eye)
            direction = direction.normalized()
            myCamera.eye = myCamera.eye.minus(direction)
            myCamera.center = myCamera.center.minus(direction)
            event.accepted = true
        }
    }
}
