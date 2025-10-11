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

        Text {
            id: statusText
            text: "Connecting..."
            font.bold: true
        }

        Text {
            text: "Switch workspaces in niri to see updates in real-time"
            font.pixelSize: 10
            color: "#666"
            font.italic: true
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: niri.workspaces
            spacing: 5

            delegate: Rectangle {
                width: ListView.view.width
                height: 80
                color: model.isFocused ? "#4CAF50" :
                       model.isActive ? "#8BC34A" : "#E0E0E0"
                border.color: model.isUrgent ? "red" : "transparent"
                border.width: 3
                radius: 5

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

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#CCC"
        }

        RowLayout {
            Item { Layout.fillWidth: true }

            Text {
                text: "Total workspaces: " + niri.workspaces.count
                font.pixelSize: 12
                font.bold: true
            }
        }
    }
}
