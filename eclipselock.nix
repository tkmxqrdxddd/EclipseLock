{ pkgs ? import <nixpkgs> {} }:

let
  eclipselock = pkgs.stdenv.mkDerivation {
    name = "eclipselock";
    src = ./.;
    buildInputs = with pkgs; [
      makeWrapper
      openssl
    ];

    buildPhase = ''
      mkdir -p build
      g++ -std=c++17 -o build/eclipselock src/main.cpp -lssl -lcrypto
    '';

    installPhase = ''
      mkdir -p $out/bin
      cp build/eclipselock $out/bin/eclipselock
      wrapProgram $out/bin/eclipselock --prefix "PATH" : "${pkgs.makeWrapper}/bin:$PATH"
    '';
  };
in
  eclipselock