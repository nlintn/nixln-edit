{ cmake
, lib
, stdenv
}:

let
  pname = "nixln-edit";
  version = "0.2.0";
in stdenv.mkDerivation {
  inherit pname version;
  src = ./.;
  buildInputs = [ cmake ];
  buildPhase = ''
    cd ..
    echo \#define NAME \"${pname}\" > ./src/VERSION.h
    echo \#define VERSION \"${version}\" >> ./src/VERSION.h
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
  '';
  installPhase = ''
    mkdir -p $out/bin
    cp src/nixln-edit $out/bin/nixln-edit
  '';
  meta = with lib; {
    description = "Tool to quickly edit config files symlinked to the nix store, and restore the prior states afterwards";
    homepage = "https://github.com/nlintn/nixln-edit";
    license = licenses.mit;
    mainProgram = "nixln-edit";
  };
}

