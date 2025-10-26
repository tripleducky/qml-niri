{ lib
, stdenv
, cmake
, pkg-config
, qt6
}:

stdenv.mkDerivation {
  pname = "qml-niri";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [
    cmake
    pkg-config
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtdeclarative
    qt6.qtwayland
  ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
  ];

  installPhase = ''
    runHook preInstall
    
    mkdir -p $out/lib/qt-6/qml
    cp -r Niri $out/lib/qt-6/qml/
    
    runHook postInstall
  '';

  meta = with lib; {
    description = "QML plugin for interacting with niri Wayland compositor";
    homepage = "https://github.com/imiric/qml-niri";
    license = licenses.mit;
    platforms = platforms.linux;
    maintainers = [ ];
  };
}
