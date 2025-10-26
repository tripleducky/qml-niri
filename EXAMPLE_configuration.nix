# Example /etc/nixos/configuration.nix with qml-niri installed

{ config, pkgs, ... }:

let
  # Import qml-niri from your local checkout
  qml-niri = pkgs.callPackage /home/andre/Downloads/qml-niri/default.nix { };
in
{
  imports =
    [ # Include the results of the hardware scan.
      ./hardware-configuration.nix
    ];

  # Bootloader
  boot.loader.systemd-boot.enable = true;
  boot.loader.efi.canTouchEfiVariables = true;

  networking.hostName = "nixos"; # Define your hostname

  # ... other configuration ...

  # System packages
  environment.systemPackages = with pkgs; [
    # Your existing packages
    vim
    git
    firefox
    
    # Add qml-niri
    qml-niri
    
    # You might also want these for using qml-niri:
    qt6.qtbase
    qt6.qtdeclarative
  ];

  # Set QML import path for the plugin
  environment.sessionVariables = {
    QML_IMPORT_PATH = [ "${qml-niri}/lib/qt-6/qml" ];
  };

  # ... rest of your configuration ...

  system.stateVersion = "24.05"; # Don't change this
}
