# haltech_keypad_teensy

This code is designed to run on a Teensy4.0 and will emulate a Haltech 15 button CAN Keypad "B". 

It simply takes button inputs from 15 input pins and converts them as CAN keypad buttons broadcast over Haltech CAN network.
You will need a Teensy 4.0, a 12v to 5v stepdown regulator, and a CANbus tranceiver.

Credit to @analoghan for this code I have just modified his program for more generic use. [haltech_keypad_emulator](https://github.com/analoghan/haltech_keypad_emulator)

I'm not a programmer so don't expect this code to be perfect. I didn't bother implementing any button debouncing because I'm using heavy Knitter pushbutton switches.

### Hardware I used
* [Teensy4.0](https://www.pjrc.com/store/teensy40.html)
* [Pololu 5V, 500mA Step-Down Voltage Regulator D24V5F5](https://www.pololu.com/product/2843)
* [Waveshare SN65HVD230 CAN Board/Transceiver](https://www.waveshare.com/sn65hvd230-can-board.htm)
* Male Deutsch DTM-4 Connector

### Dependancies
[FlexCAN_T4 Library](https://github.com/tonton81/FlexCAN_T4) - https://github.com/tonton81/FlexCAN_T4
