# Pongo Doom

![PongoDoom](imgs/PongoDoomTitle.png)

This is a Proof of Concept port of Doom to pongoOS.

Currently this doesn't support any input, sound, or save files.  
This port embeds the original shareware version of Doom into the binary, when run will show the title screen and subsequently run some demo gameplay.

![PongoDoomFootage](imgs/footage.gif)

# Building/Running

- I've only tested/run this on an iPhone SE, in theory it should work on other checkM8/checkra1n'ble devices.

## MacOS
- Run the `make.sh` script which should produce a `pongo_doom` mach-o kext.
- Build PongoOS  and run via checkra1n
    - pongo doom was run/tested on a build from this commit of pongoOS: https://github.com/checkra1n/pongoOS/commit/a6e41dbd779b133d54f92c475ee4abbd415a3736). Future pongoOS builds will probably break the current implementation.
    - checkra1n can be downloaded [here](https://checkra.in/).
    - Run pongo via checkra1n on a compatible iOS device: `/Applications/checkra1n.app/Contents/MacOS/checkra1n -k build/Pongo.bin`
- Load the `pongo_doom` module, the pongoOS project has a `module_load.py` script to do this. `python3 module_load.py pongo_doom`
- Launch via `pongoterm`, in the same script folder `pongoterm` can be built, running it will drop you into pongoterm. Then enter the command `doom` which will run `pongo_doom`

# Thanks
- The checkra1n team for checkra1n and pongoOS
- stoffera, for the fbdoom port I forked which allowed me to port Doom to pongoOS a bit easier.
- The Doom team, for well.. Doom :D
