import QtQuick 2.15
import Niri 1.0

Item {
    width: 400
    height: 300

    Component.onCompleted: {
        console.log("Testing niri connection...")
        client.connect()
    }

    NiriClient {
        id: client

        onConnected: {
            console.log("✓ Connected to niri!")
        }

        onDisconnected: {
            console.log("✗ Disconnected from niri")
        }

        onErrorOccurred: function(error) {
            console.log("✗ Error:", error)
        }

        onEventReceived: function(event) {
            console.log("✓ Event received:", JSON.stringify(event, null, 2))
        }
    }
}
