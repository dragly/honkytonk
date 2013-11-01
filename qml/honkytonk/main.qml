import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.0

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
            timeSlider.value += mainViewport.billboard.sampleStep
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 5
        ColumnLayout {
            MainViewport {
                id: mainViewport
                billboard {
                    currentSampleStep: timeSlider.value
                    nVisibleSampleSteps: nVisibleSampleStepsSlider.trueValue
                    size: particleSizeSlider.value
                    sampleStep: sampleStepSlider.trueValue
                }
                positionReader {
                    fileName: windowRoot.fileName
                }
                multiplier: densityMultiplierSlider.value
                useSquareRootDensity: useSquareRootDensityCheckBox.checked

                Layout.fillWidth: true
                Layout.fillHeight: true
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
                    value: 10
                    stepSize: mainViewport.billboard.sampleStep
                    minimumValue: 0
                    maximumValue: mainViewport.positionReader.nSampleSteps
                }
            }
        }
        ColumnLayout {
            Label {
                text: qsTr("Armadillo cube,\nsteps x particles x dimensions")
            }
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
                text: qsTr("Sample step: ") + sampleStepSlider.trueValue
            }
            Slider {
                id: sampleStepSlider
                property int trueValue: 1 + Math.exp(value) / Math.exp(10) * 10000
                Layout.minimumWidth: 200
                minimumValue: 0
                maximumValue: 10
                value: 1
            }
            Label {
                text: qsTr("Visible samples: ") + nVisibleSampleStepsSlider.trueValue
            }
            Slider {
                id: nVisibleSampleStepsSlider
                property int trueValue: 1 + Math.exp(value) / Math.exp(10) * 10000
                Layout.minimumWidth: 200
                value: 0.1
                minimumValue: 0
                maximumValue: 10
            }
            Label {
                text: qsTr("Particle size:")
            }
            Slider {
                id: particleSizeSlider
                Layout.minimumWidth: 200
                minimumValue: 0.01
                maximumValue: 2
                value: 0.2
            }
            Label {
                text: qsTr("Density multiplier:")
            }
            Slider {
                id: densityMultiplierSlider
                Layout.minimumWidth: 200
                minimumValue: 0.01
                maximumValue: 500
                value: 30
            }
            CheckBox {
                id: useSquareRootDensityCheckBox
                checked: false
                text: "Use sqrt(densityValue)"
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
