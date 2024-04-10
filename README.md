# nixln-edit

Temporarily replaces a symlink with a writeable copy of the link target and opens it up with an editor of your choice to allow quick changes. Afterwards automatically restores the symlink.


## Why?
With config files managed by nix / home-manager it can be quite cumbersome to make many (small) changes in a short amount of time (e.g. to find just the right colour value),
because of the need to build a new NixOS / home-manager generation everytime, which will at least take a few seconds.

This tool allows quick & dirty editing of these files so you can find *just* the right config values and are able to transfer these to your NixOS / home-manager configuration afterwards.


## Usage

### With Nix
Get a temporary shell with `nixln-edit` available:
```sh
nix --extra-experimental-features "nix-command flakes" shell github:nlintn/nixln-edit
```
Now to use the tool, just do `nixln-edit LINK_NAME` (`nixln-edit LINK_NAME -e EDITOR` to use an editor other than `$EDITOR`)

### Without Nix

```sh
git clone https://github.com/nlintn/nixln-edit.git
cd nixln-edit
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```
The `nixln-edit` binary will now be available in `nixln-edit/build/src/nixln-edit`.
