import QtQuick 2.15
import Niri 0.1

Item {
    width: 400
    height: 300

    Component.onCompleted: {
        console.log("=== Testing Niri Event Stream ===")
        niri.connect()
    }

    Niri {
        id: niri

        onConnected: {
            console.log("✓ Connected to niri socket")
            console.log("  Waiting for events...")
        }

        onDisconnected: {
            console.log("✗ Disconnected from niri")
        }

        onErrorOccurred: function(error) {
            console.log("✗ Error:", error)
        }

        onRawEventReceived: function(event) {
            console.log("✓ Event received:", JSON.stringify(event, null, 2))
        }
    }

    Timer {
        interval: 100
        running: niri.isConnected()
        repeat: true
        onTriggered: {
            // Keep the event loop running to receive events
        }
    }

    Text {
        anchors.centerIn: parent
        text: "Check console output for events\n" +
              "Interact with niri (switch workspaces, etc.)\n" +
              "Press Ctrl+C to exit"
        horizontalAlignment: Text.AlignHCenter
    }
}
