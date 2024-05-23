{
  description = "Tool to quickly edit config files symlinked to the nix store, and restore the prior states afterwards";

  outputs = { self, nixpkgs }:
    let
      eachSystem = nixpkgs.lib.genAttrs [ "aarch64-darwin" "aarch64-linux" "x86_64-darwin" "x86_64-linux" ];
      pkgsFor = eachSystem (system: import nixpkgs { inherit system; overlays = [ self.overlays.default ]; });
      pname = "nixln-edit";
      version = "0.1.0";
    in {
      overlays.default = final: prev: {
        nixln-edit = with final; stdenv.mkDerivation {
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
        };
      };

      packages = eachSystem (system: {
        default = self.packages.${system}.nixln-edit;
        inherit (pkgsFor.${system}) nixln-edit;
      });

      devShells = eachSystem (system: {
        default =
          pkgsFor.${system}.mkShell {
            nativeBuildInputs = with pkgsFor.${system}; [ cmake clang-tools_17 ];
          };
      });
    };

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };
}
