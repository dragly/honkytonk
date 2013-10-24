import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.0
import Qt3D 2.0
import Qt3D.Shapes 2.0
import Dragly 1.0

Item {
    id: windowRoot
    property string fileName
    property bool isPlaying: false
    width: 1000
    height: 800

    Timer {
        id: playbackTimer
        running: windowRoot.isPlaying
        interval: 16
        repeat: true
        onTriggered: {
            var dt = 0.001
            if(timeSlider.value + dt > 1) {
                timeSlider.value = 0
            } else {
                timeSlider.value += dt
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 5
        ColumnLayout {
            Viewport {
                Layout.fillWidth: true
                Layout.fillHeight: true
                fillColor: "black"

                light: Light {
                    ambientColor: Qt.rgba(1,1,1,1)
                    position.x: myCamera.eye.x / 2
                    position.y: myCamera.eye.y / 2
                    position.z: myCamera.eye.z / 2
                    quadraticAttenuation: 0.01
                }

                camera: Camera {
                    id: myCamera
                    eye: Qt.vector3d(21,7,19)
                    nearPlane: 5
                    farPlane: 50
                    eyeSeparation: 10
                }

                MultiBillboard {
//                    sortPoints: Item3D.BackToFront
                    nVisiblePoints: 1000 * nTimeStepsSlider.value
                    currentTimeStep: 10000 * timeSlider.value
                    fileName: windowRoot.fileName
                    useAlphaTest: true
                    effect: Effect {
                        blending: true
                        texture: "particle.png"
                    }
                }
            }
            RowLayout {
                Button {
                    id: playPauseButton
                    text: windowRoot.isPlaying ? qsTr("Pause") : qsTr("Play")
                    onClicked: windowRoot.isPlaying = !windowRoot.isPlaying
                }
                Slider {
                    id: timeSlider
                    Layout.fillWidth: true
                }
            }
        }
        ColumnLayout {
            Label {
                text: qsTr("File:")
            }
            TextField {
                Layout.minimumWidth: 200
                text: windowRoot.fileName
                readOnly: true
            }
            Button {
                text: qsTr("Browse")
                onClicked: fileDialog.visible = true
                Layout.alignment: Qt.AlignRight
            }
            Label {
                text: qsTr("Number of timesteps:")
            }
            Slider {
                id: nTimeStepsSlider
                Layout.minimumWidth: 200
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        selectMultiple: false
        folder: "/tmp"
        onAccepted: {
            windowRoot.fileName = fileDialog.fileUrl.toString().replace("file://", "")
        }
    }
}
