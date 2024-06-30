# Minesweeper (PS5)

This is a SDL2/C game for jailbroken PS5 (3.xx-4xx).

## Prerequisites
- elfldr.elf
- shsrv.elf
- ftpsrv.elf

## How to Use
1. Send the elfldr.elf payload to port 9020.
2. Send the shsrv.elf payload to port 9021.
3. Send the ftpsrv.elf payload to port 9021.
4. Copy the minesweeper.elf payload and all other files to `/data/minesweeper` directory on your PS5.
5. Execute `telnet [PS5-IP] 2323`.
6. cd `/data/minesweeper`
6. Run the command `hbldr minesweeper.elf`.

## Credits
Credits to J. Tornblom for the SDK, SDL2 port for PS5, and required payloads.