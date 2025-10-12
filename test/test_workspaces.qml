import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Niri 0.1

ApplicationWindow {
    visible: true
    width: 600
    height: 400
    title: "Niri Workspaces Test"

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
                text: "Total workspaces: " + niri.workspaces.count
                font.pixelSize: 12
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#CCC"
        }

        Text {
            text: "Click on a workspace to switch to it"
            font.pixelSize: 10
            color: "#666"
            font.italic: true
        }

        // All workspaces list
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: niri.workspaces
            spacing: 5
            clip: true

            delegate: Rectangle {
                width: ListView.view.width
                height: 80
                color: model.isFocused ? "#4CAF50" :
                       model.isActive ? "#8BC34A" : "#E0E0E0"
                border.color: model.isUrgent ? "red" : "#999"
                border.width: model.isUrgent ? 3 : 1
                radius: 5

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true

                    onEntered: {
                        parent.opacity = 0.8
                    }

                    onExited: {
                        parent.opacity = 1.0
                    }

                    onClicked: {
                        console.log("Switching to workspace", model.index, "(ID:", model.id + ")")
                        niri.focusWorkspaceById(model.id)
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    RowLayout {
                        Text {
                            text: "Workspace " + model.index
                            font.bold: true
                            font.pixelSize: 14
                        }
                        Text {
                            text: model.name || "(unnamed)"
                            font.italic: !model.name
                            color: "#666"
                        }
                    }

                    RowLayout {
                        Text {
                            text: "ID: " + model.id
                            font.pixelSize: 10
                            color: "#888"
                        }
                        Text {
                            text: "Output: " + (model.output || "none")
                            font.pixelSize: 10
                        }
                    }

                    RowLayout {
                        spacing: 10
                        Text {
                            text: model.isFocused ? "● FOCUSED" :
                                  model.isActive ? "○ ACTIVE" : "  inactive"
                            font.bold: model.isFocused || model.isActive
                            color: model.isFocused ? "white" :
                                   model.isActive ? "#333" : "#999"
                            font.pixelSize: 12
                        }
                        Text {
                            text: "⚠ URGENT"
                            color: "darkred"
                            font.bold: true
                            visible: model.isUrgent
                        }
                    }

                    Text {
                        text: "Active Window: " +
                              (model.activeWindowId ? model.activeWindowId : "none")
                        font.pixelSize: 10
                        color: "#888"
                    }
                }
            }
        }
    }
}
