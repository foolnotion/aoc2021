{
  description = "Aoc 2021";

  inputs.flake-utils.url = "github:numtide/flake-utils";
  inputs.nur.url = "github:nix-community/NUR";
  inputs.nixpkgs.url = "github:nixos/nixpkgs/master";

  outputs = { self, flake-utils, nixpkgs, nur }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ nur.overlay ];
          };
          repo = pkgs.nur.repos.foolnotion;
        in
        {
          devShell = pkgs.gcc11Stdenv.mkDerivation {
            name = "aoc-2021-env";
            hardeningDisable = [ "all" ];
            impureUseNativeOptimizations = true;
            nativeBuildInputs = with pkgs; [ bear cmake clang_13 clang-tools cppcheck ];
            buildInputs = with pkgs; [
                # python environment for bindings and scripting
                diff-so-fancy
                doctest
                eigen
                fmt
                gdb
                hyperfine
                hotspot
                jemalloc
                linuxPackages.perf
                microsoft_gsl
                openlibm
                pkg-config
                valgrind
                xxHash
                # Some dependencies are provided by a NUR repo
                repo.aria-csv
                repo.autodiff
                repo.cpp-sort
                repo.fast_float
                repo.pratt-parser
                repo.robin-hood-hashing
                repo.scnlib
                repo.span-lite
                repo.taskflow
                repo.vectorclass
                repo.vstat
              ];

            shellHook = ''
              LD_LIBRARY_PATH=${pkgs.lib.makeLibraryPath [ pkgs.gcc11Stdenv.cc.cc.lib ]};
              '';
          };
        }
      );
}
