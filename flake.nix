{
  description = "QML plugin for niri Wayland compositor";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "qml-niri";
          version = "0.1.0";

          src = ./.;

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

          meta = with pkgs.lib; {
            description = "QML plugin for interacting with niri Wayland compositor";
            homepage = "https://github.com/imiric/qml-niri";
            license = licenses.mit;
            platforms = platforms.linux;
          };
        };

        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            cmake
            pkg-config
            qt6.wrapQtAppsHook
            just
          ];

          buildInputs = with pkgs; [
            qt6.qtbase
            qt6.qtdeclarative
            qt6.qtwayland
          ];

          shellHook = ''
            export QT_PLUGIN_PATH="${pkgs.qt6.qtbase}/${pkgs.qt6.qtbase.qtPluginPrefix}"
            export QML_IMPORT_PATH="${pkgs.qt6.qtdeclarative}/${pkgs.qt6.qtbase.qtQmlPrefix}"
            echo "qml-niri development environment loaded"
            echo "Run 'just build' to build the project"
          '';
        };
      }
    );
}
