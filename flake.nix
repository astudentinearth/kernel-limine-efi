{
  description = "osdev env";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs, ... }@inputs:
    let
      systems = [
        "x86_64-linux"
        "aarch64-darwin"
        "aarch64-linux"
      ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages."${system}");
    in
    {
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            xorriso
            mtools
            pkgsCross.x86_64-embedded.stdenv.cc
            pkgsCross.x86_64-embedded.stdenv.cc.bintools
            qemu
            OVMFFull
            gnumake
            gdb
            limine-full
            bear
            nasm
          ];

          env = {
            OVMF_CODE = "${pkgs.OVMFFull.firmware}";
            LIMINE_CMD = "${nixpkgs.lib.getExe pkgs.limine-full}";
          };
        };
      });
    };

}
