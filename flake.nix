{
  description = "Tool to quickly edit config files symlinked to the nix store, and restore the prior states afterwards";

  outputs = { self, nixpkgs }:
    let
      eachSystem = nixpkgs.lib.genAttrs (builtins.attrNames nixpkgs.legacyPackages);
      pkgsFor = eachSystem (system: import nixpkgs { inherit system; });
    in {
      packages = eachSystem (system: {
        default = self.packages.${system}.nixln-edit;
        nixln-edit = pkgsFor.${system}.callPackage ./. {};
      });

      devShells = eachSystem (system: {
        default = import ./shell.nix { pkgs = pkgsFor.${system}; };
      });
    };

  inputs = {
    nixpkgs.url = "nixpkgs";
  };
}
