import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: loadMidiWindow
    width: 400
    height: implicitHeight
    title: qsTr("Load MIDI")
    visible: true
    flags: Qt.Dialog
    modality: Qt.WindowModal

    // Resources and styles (equivalent to merged dictionaries in XAML)
    property alias voidBackgroundBrush: "lightgray" // Replace with actual color or resource
    property alias subTitleBarBrush: "darkgray"    // Replace with actual color or resource
    property alias slimButtonStyle: ButtonStyle {} // Replace with actual style
    property alias slimTextBoxStyle: TextFieldStyle {} // Replace with actual style

    background: Rectangle {
        color: loadMidiWindow.voidBackgroundBrush
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Title Bar
        RowLayout {
            height: 25
            spacing: 0
            Rectangle {
                width: 25
                height: parent.height
                color: "transparent"
                Image {
                    anchors.centerIn: parent
                    source: "icon.png" // Replace with actual icon
                }
            }
            Label {
                Layout.fillWidth: true
                text: qsTr("Load MIDI")
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            Button {
                width: 46
                text: "X"
                onClicked: loadMidiWindow.close()
            }
        }

        // Main Content
        GridLayout {
            columns: 2
            rowSpacing: 10
            columnSpacing: 10
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 10

            // File Selection
            Rectangle {
                Layout.row: 0
                Layout.column: 0
                Layout.columnSpan: 2
                height: 50
                color: loadMidiWindow.subTitleBarBrush
                Label {
                    anchors.centerIn: parent
                    text: qsTr("File")
                    font.pixelSize: 12
                }
            }
            Button {
                Layout.row: 0
                Layout.column: 1
                text: qsTr("Select File")
                style: loadMidiWindow.slimButtonStyle
                onClicked: {
                    // Implement file selection logic
                    console.log("Select File button clicked")
                }
            }

            // Track Input
            Rectangle {
                Layout.row: 1
                Layout.column: 0
                height: 50
                color: loadMidiWindow.subTitleBarBrush
                Label {
                    anchors.centerIn: parent
                    text: qsTr("Track")
                    font.pixelSize: 12
                }
            }
            TextField {
                Layout.row: 1
                Layout.column: 1
                placeholderText: qsTr("Enter track number")
                style: loadMidiWindow.slimTextBoxStyle
                onTextChanged: {
                    // Implement track text change logic
                    console.log("Track changed to:", text)
                }
            }

            // Priority Input
            Rectangle {
                Layout.row: 2
                Layout.column: 0
                height: 50
                color: loadMidiWindow.subTitleBarBrush
                Label {
                    anchors.centerIn: parent
                    text: qsTr("Priority")
                    font.pixelSize: 12
                }
            }
            TextField {
                Layout.row: 2
                Layout.column: 1
                placeholderText: qsTr("Enter priority")
                style: loadMidiWindow.slimTextBoxStyle
                onTextChanged: {
                    // Implement priority text change logic
                    console.log("Priority changed to:", text)
                }
            }
        }
    }
}