{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    # Development tools
    cmake
    pkgconfig
    gcc
    gnumake
    
    # GTK development libraries
    gtk3
    glib
    glibmm
    
    # Crypto libraries
    openssl
    openssl.dev
    
    # Development utilities
    gdb
    valgrind
    clang
  ];

  # Set up environment variables
  CXX = "${pkgs.gcc}/bin/g++";
  CC = "${pkgs.gcc}/bin/gcc";
  
  # Add pkg-config paths
  PKG_CONFIG_PATH = lib.makeLibraryPath [
    pkgs.gtk3
    pkgs.glib
    pkgs.openssl
  ];

  shellHook = ''
    echo "Development environment ready!"
    echo "Available tools:"
    echo "  - cmake"
    echo "  - g++/gcc"
    echo "  - make"
    echo "  - gdb"
    echo "  - pkg-config (configured for GTK3, GLib, OpenSSL)"
    echo ""
    echo "To build the project:"
    echo "  1. mkdir -p build && cd build"
    echo "  2. cmake .."
    echo "  3. make"
  '';
}