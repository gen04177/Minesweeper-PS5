# Minesweeper (PS5)

This is an SDL2/C game for jailbroken PS5 (1.xx-5xx).

## Prerequisites
- elfldr.elf
- shsrv.elf
- ftpsrv.elf

## How to Use
1. Send the elfldr.elf payload to port 9020.
2. Send the shsrv.elf payload to port 9021.
3. Send the ftpsrv.elf payload to port 9021.
4. Copy minesweeper.elf to your PS5.
5. Execute `telnet [PS5-IP] 2323`.
6. Run the command `hbldr minesweeper.elf`.

## Controls:
Arrows: navigation

Cross: reveal a tile

Circle: flag a tile

L1: pause/resume music

Touchpad: menu

## Credits
- J. Tornblom - SDK and required payloads.
