# haltech_keypad_teensy

This program is designed to emulate a Haltech 15 button CAN Keypad "B". It runs on the Teensy4.0 Development Board. 

Credit to analoghan for the original code, I have just expanded on his sketch to include all 15 buttons and made it suitable for more generic use. 
His sketch is here [haltech_keypad_emulator](https://github.com/analoghan/haltech_keypad_emulator) and [YouTube video here](https://www.youtube.com/watch?v=lhPgehjmZn4)

This program simply takes button inputs from 15 input pins and converts them as CAN keypad buttons to be broadcast over the Haltech CAN network.

I'm not a programmer, I've tried to keep the code beginner friendly.

You will need a Teensy4.0 board, a 12v to 5v stepdown regulator, and a CANbus tranceiver. 

A standard Arduino is not fast enough for the CANbus to work.

ezButton is utilised for button debouncing.



### Hardware I used
* [Teensy4.0](https://www.pjrc.com/store/teensy40.html)
* [Pololu 5V, 500mA Step-Down Voltage Regulator D24V5F5](https://www.pololu.com/product/2843)
* [Waveshare SN65HVD230 CAN Board/Transceiver](https://www.waveshare.com/sn65hvd230-can-board.htm)
* Male Deutsch DTM-4 Connector

### Dependancies
[FlexCAN_T4 Library](https://github.com/tonton81/FlexCAN_T4) - https://github.com/tonton81/FlexCAN_T4

[ezButton Library](https://github.com/ArduinoGetStarted/button) - https://github.com/ArduinoGetStarted/button
