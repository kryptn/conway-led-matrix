# Conway's Game of Life
#### On a 16x16 LED Matrix

## materials

required per display piece: 

- [esp8266][1]
- [16x16 RGB LED matrix][2]

additional materials necessary:

- [wires][3]
- [knurled nuts][4]
- [m3 heat set tip][5]
- [set with m3 screws][6]
- [aio robotics white pla][7]

tools needed:

- 3d printer
- solder iron
  - with [m3 insert tip][5]

| item                 |  startup cost | cost per piece |
|----------------------|--------------:|---------------:|
| [board][1]           |           $17 |          $5.67 |
| [LED Matrix][2]      |           $35 |            $35 |
| [wires][3]           |            $7 |                |
| [heatset inserts][4] |           $20 |                |
| [m3 tip][5]          |           $19 |                |
| [m3 screws][6]       |           $20 |                |
| [white pla][7]       |           $14 |                |
| 3d printer           |         $250+ |                |
| **total**            |         $132+ |            $41 |

## Instructions

1.  print both pieces. my settings:
    - 0.15mm layer height
    - first layer 210c, other layers 205c
    - bed 60c
    - 20% gyroid infill
2. flash code onto esp8266
    - may have to change some settings
    - arduino ide
    - board manager, [details on board store page][1]
    - code link here
3. prepare board
    - cut off JST with DOUT
    - cut off middle 5v/gnd
4. insert heat insets on markings
5. fasten flashed esp8266
    - with pins facing out
    - with usb on bottom
6. assemble
    - place led matrix in main body
    - slide back plate down, careful with the bottom JST connector
    - separate three of the male -> female jumper wires
        - on the female side, split the three enough to plug in across the board
    - connect 5v to Vin, Gnd to Gnd, DIN to GPIO5
7. Plug in USB

[1]: https://smile.amazon.com/gp/product/B07LCMNXTN
[2]: https://smile.amazon.com/gp/product/B01HGJRBWW
[3]: https://smile.amazon.com/gp/product/B01EV70C78
[4]: https://smile.amazon.com/gp/product/B07MWBJB67
[5]: https://smile.amazon.com/gp/product/B078K72615
[6]: https://smile.amazon.com/gp/product/B01MT836J4
[7]: https://smile.amazon.com/gp/product/B01HYYPMAM