# CHIP-8

A CHIP-8 interpreter written in C with SDL2.

CHIP-8 is a simple virtual machine from the 1970s used to run small games on early microcomputers. The goal of this project is to run classic CHIP-8 games, starting with Pong and Space Invaders.

## Installation

Requires `gcc`, `make` and SDL2.

```sh
# Debian / Ubuntu
sudo apt install build-essential libsdl2-dev

git clone https://github.com/Abel-Lr/CHIP-8.git
cd CHIP-8
make
```

## Usage

Put your `.ch8` ROMs in the `roms/` directory, then run:

```sh
./chip8 roms/pong.ch8
```

or

```sh
make run ROM=roms/pong.ch8
```

## Controls

The CHIP-8 keypad is mapped to the left side of an AZERTY keyboard:

```
CHIP-8          Keyboard
1 2 3 C         1 2 3 4
4 5 6 D   ->    A Z E R
7 8 9 E         Q S D F
A 0 B F         W X C V
```

## Goals

- [ ] Pong
- [ ] Space Invaders
