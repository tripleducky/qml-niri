# qml-niri

A QML plugin for interacting with the [niri](https://github.com/YaLTeR/niri) Wayland compositor via its IPC protocol.

<details>
  <summary>Why?</summary>

I really like the niri compositor/WM, but there are no good integrations for it for building UI widgets, status bars, etc. There are several options mentioned in the [awesome-niri](https://github.com/Vortriz/awesome-niri) list, but none of them are great IMO.

[QuickShell](https://git.outfoxxed.me/outfoxxed/quickshell) and Qt Quick stand out above the rest, but it currently only supports Hyprland. There is interest in adding [support for niri](https://github.com/quickshell-mirror/quickshell/issues/47), but the feature is blocked by the author's desire for compositors to implement a set of [generic Wayland protocols](https://github.com/quickshell-mirror/shell-protocols), which is in progress for niri. This is understandable, as it would avoid projects like QuickShell having to add support for custom IPC protocols of each compositor, but in the meantime, niri users are left without a good solution. If, and when, QuickShell officially supports niri via these generic protocols, there will likely be little need for qml-niri to exist.

The [DankMaterialShell](https://github.com/AvengeMedia/DankMaterialShell) project uses QuickShell and integrates with niri, but [this is done by shelling out to `niri` commands](https://github.com/AvengeMedia/DankMaterialShell/blob/a17343f40e2c2788d775aef08e55f73ce6e20ae2/Services/NiriService.qml), which is not ideal. It arguably simplifies the implementation by using QML, but connecting to the niri IPC socket directly would have less overhead, and this can only be done from C++. DMS is also much too complex and fancy for my personal needs, which is why I decided to not use it.
</details>


## Features

- Real-time window and workspace monitoring and switching
- Tracking of focus, urgency, layout changes, etc.
- Event-driven updates for all compositor changes
- Native QML integration with Qt 6


## Requirements

- Qt 6 (Core and QML modules)
- CMake 3.16 or newer
- A running niri compositor with a set `NIRI_SOCKET` environment variable
- C++17 compatible compiler


## Disclaimer

The author is an experienced programmer, but not with C++ or Qt. Most of this project was written with the assistance of LLM tools such as Claude Sonnet 4.5. That said, **nothing was "vibe-coded", and all code was carefully reviewed and tested**.

If you do run into any issues, or have improvement suggestions, creating a [GitHub issue](https://github.com/imiric/qml-niri/issues) would be appreciated.


## Installation

### Building from source

Install [just](https://github.com/casey/just) and run:
```bash
git clone https://github.com/imiric/qml-niri.git
cd qml-niri
just build
```

The `just build` command will create a `build` directory and compile the plugin. The built plugin will be located in `build/Niri/`.

### Installing system-wide

After building, copy the plugin to your QML import path:

```bash
# Find your QML import path
qtpaths6 --qt-query QT_INSTALL_QML

# Copy the plugin (adjust path as needed)
sudo cp -r build/Niri /usr/lib64/qt6/qml/
```

Alternatively, you can set the `QML_IMPORT_PATH` environment variable to include the build directory when running your QML applications.


## Usage

### Basic setup

Import the plugin and create a Niri instance:

```qml
import QtQuick
import Niri 0.1

Item {
    Niri {
        id: niri
        Component.onCompleted: connect()
        
        onConnected: console.log("Connected to niri")
        onErrorOccurred: function(error) {
            console.error("Error:", error)
        }
    }
}
```

### Working with workspaces

Access workspace information via the `workspaces` model:

```qml
ListView {
    model: niri.workspaces
    delegate: Rectangle {
        Text {
            text: "Workspace " + model.index + 
                  (model.isFocused ? " (focused)" : "")
        }
        MouseArea {
            anchors.fill: parent
            onClicked: niri.focusWorkspaceById(model.id)
        }
    }
}
```

Available workspace properties:
- `id`: Unique workspace identifier
- `index`: Workspace position on its output
- `name`: Optional workspace name
- `output`: Output device name
- `isActive`: Currently active on its output
- `isFocused`: Currently focused workspace
- `isUrgent`: Has windows requesting attention
- `activeWindowId`: ID of the active window

### Working with windows

Access window information via the `windows` model:

```qml
ListView {
    model: niri.windows
    delegate: Rectangle {
        color: model.isFocused ? "lightblue" : "white"
        
        Text {
            text: model.title + " (" + model.appId + ")"
        }
        
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: function(mouseEvent) {
                if (mouseEvent.button === Qt.LeftButton) {
                    niri.focusWindow(model.id)
                } else {
                    niri.closeWindow(model.id)
                }
            }
        }
    }
}
```

Available window properties:
- `id`: Unique window identifier
- `title`: Window title
- `appId`: Application identifier
- `pid`: Process ID (-1 if unavailable)
- `workspaceId`: Current workspace ID
- `isFocused`: Currently focused window
- `isFloating`: Floating window state
- `isUrgent`: Window urgency flag

### Convenience properties

Access the currently focused window directly:

```qml
Text {
    text: niri.focusedWindowTitle || "No focused window"
}

Text {
    text: "App: " + (niri.focusedWindowAppId || "none")
}
```

Count of total windows and workspaces:
```qml
Text {
    text: "Total windows: " + niri.windows.count
}

Text {
    text: "Total workspaces: " + niri.workspaces.count
}
```

### Available methods

Workspace control:
```qml
niri.focusWorkspace(0)              // By index
niri.focusWorkspaceById(12345)      // By ID
niri.focusWorkspaceByName("code")   // By name
```

Window control:
```qml
niri.focusWindow(windowId)
niri.closeWindow(windowId)
niri.closeWindowOrFocused()         // Close focused window
```


## Testing

The plugin was mostly tested manually, using a few [integration tests](/test). You can run them with:

```bash
# Test event stream
just test events

# Test workspace model
just test workspaces

# Test window model
just test windows
```

Pull requests to improve the testing situation, add unit tests, etc., are very welcome!


## API Reference

### Niri Object

*Properties:*
- `workspaces`: WorkspaceModel - List of all workspaces
- `windows`: WindowModel - List of all windows
- `focusedWindowTitle`: string - Title of focused window
- `focusedWindowAppId`: string - App ID of focused window

*Methods:*
- `connect()`: bool - Connect to niri IPC socket
- `isConnected()`: bool - Check connection status
- `focusWorkspace(index)` - Focus workspace by index
- `focusWorkspaceById(id)` - Focus workspace by ID
- `focusWorkspaceByName(name)` - Focus workspace by name
- `focusWindow(id)` - Focus specific window
- `closeWindow(id)` - Close specific window
- `closeWindowOrFocused()` - Close focused window

*Signals:*
- `connected()` - Emitted on successful connection
- `disconnected()` - Emitted on disconnection
- `errorOccurred(error)` - Emitted on error
- `rawEventReceived(event)` - Emitted for all IPC events
- `focusedWindowTitleChanged()` - Emitted when focused window title changes
- `focusedWindowAppIdChanged()` - Emitted when focused window app ID changes


## Troubleshooting

- `module "Niri" is not installed`:
  Ensure `QML_IMPORT_PATH` includes the directory containing the `Niri` directory (not the `Niri` directory itself), or that you copied to plugin to an existing QML import path (e.g. `/usr/lib64/qt6/qml/`).
  
  Also, confirm that you're using Qt 6, and not older versions. You can do this with `qml --version`. If the Qt 6 binary is not on your `$PATH` (e.g. on Void Linux it is at `/usr/lib/qt6/bin/qml`), you can symlink it as `qml6` somewhere on your `$PATH`.

- *Connection failed*:
  Verify that the `NIRI_SOCKET` environment variable is set and points to a valid socket.
  It should be something like `/run/user/<name>/niri.wayland-1.1856.sock`. Note that
  this is affected by the value of `XDG_RUNTIME_DIR`.


## License

[MIT](/LICENSE)
