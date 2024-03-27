 { pkgs ? import <nixpkgs> {} }:
 
 (pkgs.buildFHSUserEnv {
   name = "bazel-userenv";
   targetPkgs = pkgs: with pkgs; [
     bash
     bazel
     git
     python3
     libz
     glibc
     glibc.dev
     gcc
     stdenv.cc.cc
     stdenv.cc.libc
     #glibc
     #glibc.dev
     #gcc
     #
     #stdenv.cc.cc
     #stdenv.cc.libc
     #mktemp
     #which
   ];
   runScript = "bash";
 }).env

# nix-shell --pure ./tools/bazel.nix
