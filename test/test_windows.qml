import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Niri 0.1

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Niri Windows Test"

    Niri {
        id: niri
        Component.onCompleted: connect()

        onConnected: {
            console.log("✓ Connected to niri")
            statusText.text = "Connected"
            statusText.color = "green"
        }

        onDisconnected: {
            console.log("✗ Disconnected from niri")
            statusText.text = "Disconnected"
            statusText.color = "red"
        }

        onErrorOccurred: function(error) {
            console.log("✗ Error:", error)
            statusText.text = "Error: " + error
            statusText.color = "red"
        }

        onFocusedWindowTitleChanged: {
            console.log("Focused window title changed:", niri.focusedWindowTitle)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        // Status header
        RowLayout {
            Layout.fillWidth: true

            Text {
                id: statusText
                text: "Connecting..."
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "Total windows: " + niri.windows.count
                font.pixelSize: 12
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#CCC"
        }

        // Focused window info
        Rectangle {
            Layout.fillWidth: true
            height: 100
            color: "#E8F5E9"
            border.color: "#4CAF50"
            border.width: 2
            radius: 5

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10

                Text {
                    text: "Currently Focused Window"
                    font.bold: true
                    font.pixelSize: 14
                    color: "#2E7D32"
                }

                Text {
                    text: "Title: " + (niri.focusedWindowTitle || "(none)")
                    font.pixelSize: 12
                    font.italic: !niri.focusedWindowTitle
                }

                Text {
                    text: "App ID: " + (niri.focusedWindowAppId || "(none)")
                    font.pixelSize: 12
                    font.italic: !niri.focusedWindowAppId
                }
            }
        }

        Text {
            text: "All Windows (click to focus, right-click to close)"
            font.pixelSize: 10
            color: "#666"
            font.italic: true
        }

        // All windows list
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: niri.windows
            spacing: 5
            clip: true

            delegate: Rectangle {
                width: ListView.view.width
                height: 70
                color: model.isFocused ? "#4CAF50" : "#F5F5F5"
                border.color: model.isUrgent ? "red" : "#CCC"
                border.width: model.isUrgent ? 3 : 1
                radius: 5

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true

                    onEntered: {
                        parent.opacity = 0.8
                    }

                    onExited: {
                        parent.opacity = 1.0
                    }

                    onClicked: function(mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            console.log("Focusing window", model.id, "-", model.title)
                            niri.focusWindow(model.id)
                        } else if (mouse.button === Qt.RightButton) {
                            console.log("Closing window", model.id, "-", model.title)
                            niri.closeWindow(model.id)
                        }
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 2

                    RowLayout {
                        Text {
                            text: model.title || "(no title)"
                            font.bold: model.isFocused
                            font.pixelSize: 14
                            font.italic: !model.title
                            color: model.isFocused ? "white" : "black"
                        }

                        Text {
                            text: "● FOCUSED"
                            font.bold: true
                            color: "white"
                            visible: model.isFocused
                        }

                        Text {
                            text: "⚠ URGENT"
                            color: "darkred"
                            font.bold: true
                            visible: model.isUrgent
                        }

                        Text {
                            text: "[Floating]"
                            color: model.isFocused ? "white" : "#666"
                            font.italic: true
                            visible: model.isFloating
                        }
                    }

                    RowLayout {
                        Text {
                            text: "App: " + (model.appId || "unknown")
                            font.pixelSize: 10
                            color: model.isFocused ? "white" : "#666"
                        }
                        Text {
                            text: "PID: " + (model.pid >= 0 ? model.pid : "unknown")
                            font.pixelSize: 10
                            color: model.isFocused ? "white" : "#666"
                        }
                    }

                    Text {
                        text: "ID: " + model.id + " | Workspace: " +
                              (model.workspaceId || "none")
                        font.pixelSize: 10
                        color: model.isFocused ? "white" : "#888"
                    }
                }
            }
        }
    }
}
