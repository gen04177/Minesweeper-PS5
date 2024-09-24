# Minesweeper (PS5)

This is an SDL2/C game for jailbroken PS5 (1.xx-4xx).

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

## Credits
J. Tornblom - SDK and required payloads.
