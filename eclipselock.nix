{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "eclipselock";
  src = ./.;

  buildInputs = with pkgs; [
    openssl
    gtk3
    gtkmm3
    glib
    glibmm
    pkg-config
  ];

  nativeBuildInputs = with pkgs; [
    cmake
    pkg-config
    makeWrapper
  ];

  postInstall = ''
    wrapProgram $out/bin/eclipselock \
      --prefix LD_LIBRARY_PATH : "${pkgs.gtk3.out}/lib:${pkgs.gtkmm3.out}/lib:${pkgs.glib.out}/lib:${pkgs.openssl.out}/lib"
  '';

  meta = with pkgs.lib; {
    description = "File encryption tool with GTK interface";
    license = licenses.mit;
    maintainers = [ maintainers.tkmxqrdxddd ];
  };
}