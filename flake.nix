{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };
  outputs = inputs@{ self, nixpkgs, ... }:
  let
    lib = nixpkgs.lib;
    pkgs = import nixpkgs { system = "x86_64-linux"; };
    nativeBuildInputs = [];
    buildInputs = with pkgs; [
      docker
      gnugrep
      gnumake
      wget
    ];
  in {
    devShell.x86_64-linux = pkgs.mkShell {
      nativeBuildInputs = nativeBuildInputs;
      buildInputs = buildInputs;
      LD_LIBRARY_PATH = lib.makeLibraryPath buildInputs;
      RUST_SRC_PATH = "${pkgs.rust.packages.stable.rustPlatform.rustLibSrc}";
    };
  };
}
