# Building on NixOS

This document provides instructions for building qml-niri on NixOS.

## The `syncqt` Problem

On NixOS, you might encounter an error like:
```
file syncqt does not exist
```

This happens because NixOS has a unique file system structure where Qt tools are not in standard locations. The `syncqt` tool is used by Qt to generate forwarding headers, but CMake can't find it in the expected paths.

## Solutions

### Option 1: Using Nix Flakes (Recommended)

If you have flakes enabled, this is the easiest method:

```bash
# Enter the development shell
nix develop

# Build the project
just build

# Or build using nix directly
nix build
```

The built plugin will be in `build/Niri/` or `result/lib/qt-6/qml/Niri/`.

### Option 2: Using Traditional Nix Shell

If you're not using flakes:

```bash
# Enter the development shell
nix-shell

# Build the project
just build
```

### Option 3: Install to your NixOS system

You can add this to your NixOS configuration:

```nix
{ config, pkgs, ... }:

let
  qml-niri = pkgs.stdenv.mkDerivation {
    pname = "qml-niri";
    version = "0.1.0";

    src = pkgs.fetchFromGitHub {
      owner = "imiric";
      repo = "qml-niri";
      rev = "main";  # or a specific commit/tag
      sha256 = "0000000000000000000000000000000000000000000000000000";  # Update this
    };

    nativeBuildInputs = with pkgs; [
      cmake
      pkg-config
      qt6.wrapQtAppsHook
    ];

    buildInputs = with pkgs; [
      qt6.qtbase
      qt6.qtdeclarative
      qt6.qtwayland
    ];

    cmakeFlags = [
      "-DCMAKE_BUILD_TYPE=Release"
    ];

    installPhase = ''
      mkdir -p $out/lib/qt-6/qml
      cp -r Niri $out/lib/qt-6/qml/
    '';
  };
in
{
  environment.systemPackages = [ qml-niri ];
  
  # Make sure QML can find the plugin
  environment.sessionVariables = {
    QML_IMPORT_PATH = "${qml-niri}/lib/qt-6/qml";
  };
}
```

## Enabling Flakes (if not already enabled)

Add to your `/etc/nixos/configuration.nix`:

```nix
{
  nix.settings.experimental-features = [ "nix-command" "flakes" ];
}
```

Then rebuild your system:
```bash
sudo nixos-rebuild switch
```

## Troubleshooting

### CMake can't find Qt6

Make sure you're in the nix-shell environment. The Qt6 packages are provided by the shell, not globally.

### QML can't find the Niri module

Set the `QML_IMPORT_PATH` environment variable:

```bash
export QML_IMPORT_PATH=$PWD/build
qml6 test/test_workspaces.qml
```

Or if installed via nix:
```bash
export QML_IMPORT_PATH=/nix/store/.../lib/qt-6/qml
```

### Different Qt versions

Make sure you're using Qt6 from nixpkgs. Check with:
```bash
which qml6
qml6 --version
```

## Using with Quickshell

If you want to use this with Quickshell on NixOS, you'll need to ensure Quickshell can find the plugin:

```bash
# Add to your shell environment or Quickshell wrapper
export QML_IMPORT_PATH=/path/to/qml-niri/build:$QML_IMPORT_PATH
quickshell --path /path/to/your/config.qml
```

Or create a wrapper script:
```bash
#!/usr/bin/env bash
export QML_IMPORT_PATH=/nix/store/.../lib/qt-6/qml
exec quickshell "$@"
```
