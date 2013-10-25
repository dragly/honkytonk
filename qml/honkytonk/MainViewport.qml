import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0
import Dragly 1.0

Viewport {
    property alias billboard: mainBillboard
    property Sphere pickedSphere: null
    fillColor: "black"
    picking: true

    light: Light {
        ambientColor: Qt.rgba(1,1,1,1)
        position.x: myCamera.eye.x / 2
        position.y: myCamera.eye.y / 2
        position.z: myCamera.eye.z / 2
        quadraticAttenuation: 0.0025
    }

    camera: Camera {
        id: myCamera
        eye: Qt.vector3d(21,7,19)
        nearPlane: 5
        farPlane: 100
        Behavior on center.x {
            NumberAnimation {
                duration: 500
            }
        }
        Behavior on center.y {
            NumberAnimation {
                duration: 500
            }
        }
        Behavior on center.z {
            NumberAnimation {
                duration: 500
            }
        }
    }

    MultiBillboard {
        id: mainBillboard
//                    sortPoints: Item3D.BackToFront
        useAlphaTest: true
        effect: Effect {
            blending: true
            texture: "particle.png"
        }
    }

    Sphere {
        id: sphere1
        radius: 0.2
        x: 0.7
        effect: Effect {
            color: pickedSphere === sphere1 ? "red" : "yellow"
            Behavior on color {
                ColorAnimation { duration: 600 }
            }
        }

        onPressed: {
            myCamera.center = position
            pickedSphere = sphere1
        }
    }

    Sphere {
        id: sphere2
        radius: 0.2
        x: -0.7
        onPressed: {
            myCamera.center = position
            pickedSphere = sphere2
        }
        effect: Effect {
            color: pickedSphere === sphere2 ? "red" : "yellow"
            Behavior on color {
                ColorAnimation { duration: 600 }
            }
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
//        if(event.key === Qt.Key_D) {
//            var direction = myCamera.center.minus(myCamera.eye)
//            direction = direction.normalized()
//            var rotation90 = Qt.matrix4x4(0,0,1,0,
//                                        0,1,0,0,
//                                        -1,0,0,0,
//                                        0,0,0,1)
//            direction = rotation90.times(direction)
//            myCamera.eye = myCamera.eye.plus(direction)
//            myCamera.center = myCamera.center.plus(0.1*direction)
//            event.accepted = true
//        }
//        if(event.key === Qt.Key_A) {
//            var direction = myCamera.center.minus(myCamera.eye)
//            direction = direction.normalized()
//            var rotation90 = Qt.matrix4x4(0,0,1,0,
//                                        0,1,0,0,
//                                        -1,0,0,0,
//                                        0,0,0,1)
//            direction = rotation90.times(direction)
//            myCamera.eye = myCamera.eye.minus(direction)
//            myCamera.center = myCamera.center.minus(0.1*direction)
//            event.accepted = true
//        }
    }
}
