{
  description = "Tool to quickly edit config files symlinked to the nix store, and restore the prior states afterwards";

  outputs = { self, nixpkgs }:
    let
      eachSystem = nixpkgs.lib.genAttrs [ "aarch64-darwin" "aarch64-linux" "x86_64-darwin" "x86_64-linux" ];
      pkgsFor = eachSystem (system: import nixpkgs { inherit system; });
    in {
      packages = eachSystem (system: {
        default = self.packages.${system}.nixln-edit;
        nixln-edit = pkgsFor.${system}.callPackage ./. {};
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
