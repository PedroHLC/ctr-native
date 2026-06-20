{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      inherit (nixpkgs.lib.attrsets) genAttrs mapAttrs zipAttrsWith;

      buildSystems = [ "x86_64-linux" "aarch64-linux" ];

      hostSystems = pkgs: {
        "x86_64-linux" = pkgs.pkgsi686Linux.pkgsStatic;
        "aarch64-linux" = pkgs.pkgsCross.armv7l-hf-multiplatform.pkgsStatic;
        "x86_64-windows" = pkgs.pkgsCross.mingw-msvcrt-i686;
      };

      compilers = [ "gcc" "clang" ];
      targets = [ "release" "debug" ];

      mkCTRNative = pkgs: stdenv: target:
        pkgs.callPackage ./nix/package { inherit stdenv; withDebug = target == "release"; };

      forEachCompiler = pkgs: compiler:
        genAttrs targets (mkCTRNative pkgs (if compiler == "clang" then pkgs.clangStdenv else pkgs.stdenv));

      forEachSystem = pkgs:
        genAttrs compilers (forEachCompiler pkgs);

      forEachBuildSystem = buildSystem:
        let
          buildPkgs = nixpkgs.legacyPackages.${buildSystem};
          mapped = mapAttrs (_hostSystem: forEachSystem) (hostSystems buildPkgs);
        in
        mapped // {
          native = mapped.${buildSystem};
          default = mapped.${buildSystem}.gcc.release;
        };

      legacyPackages =
        genAttrs buildSystems forEachBuildSystem;

      packages =
        mapAttrs (_buildSystem: attrs: { inherit (attrs) default; }) legacyPackages;
    in
    { inherit packages legacyPackages; };
}
