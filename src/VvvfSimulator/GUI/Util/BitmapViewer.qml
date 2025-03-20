import QtQuick 2.15
import QtQuick.Controls 2.15
import VvvfSimulator.GUI.Util 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: bitmapViewer.title

    signal windowResized(int width, int height) // Custom signal

    onWidthChanged: windowResized(width, height) // Emit signal on width change
    onHeightChanged: windowResized(width, height) // Emit signal on height change

    BitmapViewer {
        id: bitmapViewer
        onPixmapChanged: console.log("Pixmap updated")
        onTitleChanged: console.log("Title updated")
    }

    // Listen for the requestResize signal
    Connections {
        target: bitmapViewer
        onRequestResize: {
            width = width_;
            height = height_;
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        Image {
            source: bitmapViewer.pixmap
            fillMode: Image.PreserveAspectFit
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            text: "Simulate Update"
            onClicked: {
                // Simulate setting a new pixmap and title programmatically
                bitmapViewer.pixmap = Qt.resolvedUrl("path/to/new/image.png")
                bitmapViewer.title = "Updated Image"
            }
        }
    }

    // Handle the custom signal
    onWindowResized: console.log("Window resized to:", width, "x", height)
}