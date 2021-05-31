# SN76489 VGM Player

> Warning: This project requires **Node 8.0**

Cross-platform node.js VGM player host & arduino firmware for hardware player which uses [SN76489](https://en.wikipedia.org/wiki/Texas_Instruments_SN76489).
This project omits **A LOT** of features of VGM file format - it simply proxies data & wait commands to the PSG.

> Disclaimer: All VGM files in this repository belong to their respective authors and provided only for the ease of testing.

## Usage

First, list the available ports:

```
node index.js -l                                                                          
comName: /dev/tty.Bluetooth-Incoming-Port
...
comName: /dev/tty.usbserial-1420 <--- Arduino connected over USB
```

Then, using this port play the VGM file:

```
node index.js -p /dev/tty.usbserial-1420 -i vgm/Sonic\ 2\ -\ Mystic\ Cave\ Zone.vgm
```

Compare to the original (that also utilizes YM3438): https://www.youtube.com/watch?v=HdgRJDu4hcU
