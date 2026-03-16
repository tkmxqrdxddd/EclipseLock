{ pkgs ? import <nixpkgs> {} }:

let
  eclipselock = pkgs.stdenv.mkDerivation {
    name = "eclipselock";
    src = ./.;
    
    buildInputs = with pkgs; [
      cmake
      makeWrapper
      openssl
      wxGTK32
      gtk3
      libX11
      libXrandr
      libXinerama
    ];

    cmakeFlags = [
      "-DCMAKE_BUILD_TYPE=Release"
    ];

    postInstall = ''
      wrapProgram $out/bin/eclipselock \
        --prefix "PATH" : "${pkgs.lib.makeBinPath [ pkgs.openssl ]}" \
        --set "LD_LIBRARY_PATH" "${pkgs.lib.makeLibraryPath [ pkgs.wxGTK32 pkgs.gtk3 pkgs.libX11 ]}"
    '';
  };
in
  eclipselock
