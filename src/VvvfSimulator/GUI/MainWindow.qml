// TO DO: This is an automatically generated draft of the GUI. It is not complete and will be updated in the future.

/*
   Copyright © 2025 VvvfGeeks, VVVF Systems
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

// MainWindow.qml
// Version 1.9.1.1

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 960
    height: 500
    title: qsTr("MainWindow")
    visible: true

    Component.onCompleted: {
        // Center the window on the screen
        mainWindow.x = (Screen.width - mainWindow.width) / 2
        mainWindow.y = (Screen.height - mainWindow.height) / 2
    }

    GridLayout {
        anchors.fill: parent
        rows: 2
        rowSpacing: 0

        Rectangle {
            Layout.fillWidth: true
            height: 25
            color: "transparent"
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            columnSpacing: 0

            GridLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                columns: 1
                rowSpacing: 5
                padding: 10

                TabView {
                    id: settingTabs
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Tab {
                        title: qsTr("Settings")
                        GridLayout {
                            rows: 4
                            rowSpacing: 5

                            Button {
                                text: qsTr("PWM Level")
                                onClicked: settingButtonClick()
                            }
                            Button {
                                text: qsTr("Minimum Frequency")
                                onClicked: settingButtonClick()
                                Layout.row: 1
                            }
                            Button {
                                text: qsTr("Jerk Setting")
                                onClicked: settingButtonClick()
                                Layout.row: 2
                            }
                        }
                    }

                    Tab {
                        title: qsTr("Accelerate")
                        GridLayout {
                            rows: 2
                            rowSpacing: 5

                            ListView {
                                id: accelerateSettings
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.row: 0
                                model: accelerateModel
                                delegate: Item {
                                    width: parent.width
                                    height: 50
                                    Column {
                                        Text {
                                            text: model.pulseMode
                                        }
                                        Text {
                                            text: model.controlFrequencyFrom
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 50
                                color: "lightgray"
                                radius: 25
                                Layout.row: 1

                                Button {
                                    text: "+"
                                    onClicked: settingEditClick()
                                }
                            }
                        }
                    }

                    Tab {
                        title: qsTr("Brake")
                        GridLayout {
                            rows: 2
                            rowSpacing: 5

                            ListView {
                                id: brakeSettings
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.row: 0
                                model: brakeModel
                                delegate: Item {
                                    width: parent.width
                                    height: 50
                                    Column {
                                        Text {
                                            text: model.pulseMode
                                        }
                                        Text {
                                            text: model.controlFrequencyFrom
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 50
                                color: "lightgray"
                                radius: 25
                                Layout.row: 1

                                Button {
                                    text: "+"
                                    onClicked: settingEditClick()
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                // Add content for the right side of the window here
            }
        }
    }

    // Define the models for the ListView
    ListModel {
        id: accelerateModel
        ListElement { pulseMode: "Mode1"; controlFrequencyFrom: "50Hz" }
        ListElement { pulseMode: "Mode2"; controlFrequencyFrom: "60Hz" }
    }

    ListModel {
        id: brakeModel
        ListElement { pulseMode: "Mode1"; controlFrequencyFrom: "50Hz" }
        ListElement { pulseMode: "Mode2"; controlFrequencyFrom: "60Hz" }
    }

    // Define the functions for button clicks
    function settingButtonClick() {
        console.log("Setting button clicked")
    }

    function settingEditClick() {
        console.log("Setting edit button clicked")
    }
}