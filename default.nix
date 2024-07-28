{ stdenv
, cmake
}:

let
  pname = "nixln-edit";
  version = "0.1.0";
in stdenv.mkDerivation {
  inherit pname version;
  src = ./.;
  buildInputs = [ cmake ];
  buildPhase = ''
    cd ..
    echo \#define NAME \"${pname}\" > ./src/VERSION.h
    echo \#define VERSION \"${version}\" >> ./src/VERSION.h
    cd build
    cmake ..
    make
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp src/nixln-edit $out/bin/nixln-edit
  '';
}

