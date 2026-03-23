{ pkgs ? import <nixpkgs> {} }:

let
  eclipselock = pkgs.stdenv.mkDerivation {
    name = "eclipselock";
    src = ./.;
    buildInputs = with pkgs; [
      makeWrapper
      openssl
      gtk3
      gtkmm30
      glib
      glibmm
      pkgconfig
      cmake
    ];

    nativeBuildInputs = with pkgs; [
      cmake
      pkgconfig
    ];

    buildPhase = ''
      mkdir -p build
      cd build
      cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$out \
        -DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=TRUE \
        -DCMAKE_PREFIX_PATH="${pkgs.gtkmm30}:${pkgs.gtk3}:${pkgs.glibmm}:${pkgs.glib}:${pkgs.openssl}"
      
      make -j$(nproc)
    '';

    installPhase = ''
      cd build
      make install
      
      # Wrap the program to ensure proper environment
      wrapProgram $out/bin/eclipselock \
        --prefix "PATH" : "${pkgs.makeWrapper}/bin:$PATH" \
        --prefix "LD_LIBRARY_PATH" : "${pkgs.gtk3.out}/lib:${pkgs.gtkmm30.out}/lib:${pkgs.glib.out}/lib:${pkgs.openssl.out}/lib"
    '';
    
    # Ensure proper runtime dependencies
    runtimeDependencies = with pkgs; [
      gtk3
      glib
      openssl
    ];
  };
in
  eclipselock