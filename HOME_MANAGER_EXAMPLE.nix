# Add to your home.nix configuration

{ config, pkgs, ... }:

let
  qml-niri = pkgs.stdenv.mkDerivation {
    pname = "qml-niri";
    version = "0.1.0";

    # For local development, point to your local source
    src = /home/andre/Downloads/qml-niri;
    
    # Or fetch from GitHub
    # src = pkgs.fetchFromGitHub {
    #   owner = "imiric";
    #   repo = "qml-niri";
    #   rev = "main";
    #   sha256 = pkgs.lib.fakeSha256;  # Replace with actual hash after first build
    # };

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
      license = licenses.mit;
      platforms = platforms.linux;
    };
  };
in
{
  # Install the package
  home.packages = [ qml-niri ];

  # Set QML_IMPORT_PATH to include the plugin
  home.sessionVariables = {
    QML_IMPORT_PATH = "${qml-niri}/lib/qt-6/qml:$QML_IMPORT_PATH";
  };
}

# After editing, rebuild with:
# home-manager switch
