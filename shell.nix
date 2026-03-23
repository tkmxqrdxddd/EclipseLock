{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    # Development tools
    cmake
    pkg-config
    gcc
    gnumake

    # GTK development libraries
    gtk3
    glib
    glibmm
    gtkmm3

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
  PKG_CONFIG_PATH = "${pkgs.gtk3.dev}/lib/pkgconfig:${pkgs.glib.dev}/lib/pkgconfig:${pkgs.openssl.dev}/lib/pkgconfig";

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