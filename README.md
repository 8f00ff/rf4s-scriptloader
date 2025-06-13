[![RF4S-ScriptLoader](https://img.shields.io/badge/RF4S-ScriptLoader-8f00ff)](https://github.com/8f00ff/rf4s-scriptloader)

[<img src="https://raw.githubusercontent.com/ZoeBijl/QueerCats/refs/heads/main/MorningCoffee/SVG/QueerCatMorningCoffee_Progress.svg" width="48" height="48" alt="Morning Pride Cat"/>](https://github.com/ZoeBijl/QueerCats)

## Overview

**rf4s-scriptloader** is a runtime modding framework for *Rune Factory 4 Special* (PC).  
It sneaks in through `SDL2.dll`, hooks into the game at launch, and opens the door for mods to do far more than asset swaps and text edits — without touching the original binaries.

## Experimental Branch

> **Note:** This branch is an experimental reimplementation of the ["Everyone is Bi"](https://www.nexusmods.com/runefactory4special/mods/98) mod for Rune Factory 4 Special.
It is a standalone mod and does not include any modloader or script loader features.
For the original xdelta patch version and future updates, see the [Nexus Mods page](https://www.nexusmods.com/runefactory4special/mods/98).

The goal? A stable, flexible foundation for real, systemic modding. Including (but absolutely not limited to):

- Extending the dialogue system with proper branching, conditionals, and custom placeholders (like pronouns, titles, or whatever else the script needs)
- Separating character visuals (Frey or Lest) from gender identity and presentation, so you can finally mix and match without breaking anything
- Supporting same-gender relationships the game didn’t account for, not by replacing content, but by building on top of it
- Runtime toggles for visual mods (like alternate hairstyles or custom looks), accessible in-game, not buried in file swaps
- A modular architecture designed for other mods to plug in cleanly and extend the same systems

This is about giving RF4S a future-proof modding backbone — one that’s inclusive, extensible, and a little more in tune with, well, the actual 21st century.

## Usage

**This branch contains a standalone mod. It does not require or provide any modloader or script loader functionality.**

The changes will be automatically applied to the game if the mod is installed correctly.

## Installation

1. **Locate your game folder**  
   To find it, you can right click the game in the game list of Steam's library and click on `Manage -> Browse local files`.

2. **Rename the original SDL2**  
   Inside that folder, rename the existing SDL2 DLL to `SDL2_real.dll`:  
   **Please note** that the new name for the original DLL is important.

3. **Drop in the mod loader**
   Copy the provided SDL2.dll from this project in place of the original.

## Building

Building is fully containerized — no setup, no mess, no Windows required.  
Just make sure you’re on Linux, because that’s all I know how to support.

### Prerequisites

- Docker
- GNU Make
- Wget
- A working shell (preferably not cursed)

If you're using Nix, a dev shell is already set up via `flake.nix`.

### Steps

1. Clone the repo:

   ```
   git clone https://github.com/8f00ff/rf4s-scriptloader
   cd rf4s-scriptloader
   ```

2. Run make (inside the Docker container):

   ```
   ./Makefile
   ```

3. The resulting `SDL2.dll` will be in the `out/` directory.

4. To install it directly into your game folder:
   ```
   make install
   ```
   This will rename the original SDL2.dll to SDL2_real.dll (if the hash matches), then copy in the modded one. This also assumes you have the game installed through Steam and it's in the default location.

## Changelog

[![GitHub Tag](https://img.shields.io/github/v/tag/8f00ff/rf4s-scriptloader)](https://github.com/8f00ff/rf4s-scriptloader/tags)

See the [CHANGELOG.md](CHANGELOG.md) file for a detailed list of changes.

## Contributing

See the [Contributing Guidelines](CONTRIBUTING.md) for more information.

## License

[![GitHub License](https://img.shields.io/github/license/8f00ff/rf4s-scriptloader)](LICENSE.md)

This project is licensed under [GNU General Public License (GPL-3.0)](https://www.gnu.org/licenses/gpl-3.0.en.html) - see the [LICENSE.md](LICENSE.md) file for details.

This project also has specific attribution requirements - see the [Attribution Requirements](CONTRIBUTING.md#attribution-requirements) section of the [Contributing Guidelines](CONTRIBUTING.md).

## Attributions

For attribution information including third-party assets and tools used in this project, see [ATTRIBUTIONS.md](ATTRIBUTIONS.md).
