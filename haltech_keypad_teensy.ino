#include <ezButton.h>
#include <FlexCAN_T4.h>   //Teensy CAN library
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1; // initiate CAN1 on pins 22 & 23
CAN_message_t ka1;        // keep alive 1 CAN msg
CAN_message_t ka2;        // keep alive 2 CAN msg
CAN_message_t incoming;   // incoming CAN data from Haltech
CAN_message_t outgoing;   // outgoing CAN data with our buttons presses

const byte NUM_EZBUTTONS = 16;  // size of the ezButton array
ezButton ezButtonArray[] = {    //assign input pins to ezButton array for button debouncing
  ezButton(0),                  
  ezButton(1),
  ezButton(2),
  ezButton(3),
  ezButton(4),
  ezButton(5),                  // Buttons must be switched to GND
  ezButton(6),                  // pinMode INPUT_PULLUP is default for ezButton
  ezButton(7),
  ezButton(8),
  ezButton(9),
  ezButton(10),
  ezButton(11),
  ezButton(12),
  ezButton(16), // this is a pin substitution due to onboard LED on pin13 - Change this value if using pin16 for something else
  ezButton(14),
  ezButton(15)
};

bool inputPinArray[16]; // array of the input pin states. 15 pins + 1 for pin13(LED) which we disregard
int  byteOne;         // this is first byte of CAN keypad buttons frame
int  byteTwo;         // this is second byte of CAN keypad button frame

#define btn1 0x1      // * The CAN msg for keypad is three bytes
#define btn2 0x2      // * Buttons 1-8 are in sent in the first byte and buttons 9-15 are sent in the second byte, the third byte is empty 
#define btn3 0x4      // * As an example, btn 1 is 00000001 and btn2 is 00000010
#define btn4 0x8      // * Both buttons pressed would be 00000011 or 0x3 in HEX
#define btn5 0x10     // * Button 3 is 00000100 (0x4) button5 is 00010000 (0x10) button 7 is 010000000 (0x40) etc
#define btn6 0x20     // * Example: buttons 1-8 all pressed would be 11111111 (0xFF)
#define btn7 0x40
#define btn8 0x80
#define btn9 0x1      // * Button 9 is 00000001 of the second byte, and so on
#define btn10 0x2     // * Example: buttons 9-15 all pressed would be 01111111 (0x7F)
#define btn11 0x4
#define btn12 0x8
#define btn13 0x10
#define btn14 0x20    // * Example: btns 1, 2, 5, 9, 15 all pressed would be sent as 00010011 01000001 00000000
#define btn15 0x40

void setup() {
  for (int i = 0; i < NUM_EZBUTTONS; i++) {
    ezButtonArray[i].setDebounceTime(50); // set button debounce time to 50 milliseconds
  }
  can1.begin();                 // start FlexCAN
  can1.setBaudRate(1000000);    //Haltech baud rate is 1Mb/s

  //You can test inputs here to see the keypad buttons active in NSP -- Comment out readInputPins() in the main loop
  //inputPinArray[0] = true;    // You may need to reboot ECU to see changes made here
  //inputPinArray[1] = true;
  //inputPinArray[2] = true;
  //inputPinArray[3] = true;
  //inputPinArray[4] = true;
  //inputPinArray[5] = true;
  //inputPinArray[6] = true;
  //inputPinArray[7] = true;
  //inputPinArray[8] = true;
  //inputPinArray[9] = true;
  //inputPinArray[10] = true;
  //inputPinArray[11] = true;
  //inputPinArray[12] = true;
  //inputPinArray[14] = true;
  //inputPinArray[15] = true;
}

void loop() {
  readInputPins();
  convertInputsToButtons();
  if (can1.read(incoming)) {
    CANKeepAliveFrames();
    processFrame(incoming);
    sendCANButtons();
  }
}

// Read the Teensy input pins and fill the input array with button states
void readInputPins() {
  for (int i = 0; i < NUM_EZBUTTONS; i++) {
    ezButtonArray[i].loop(); // MUST call this loop() function first (for ezButton debouncing)
  }
  for (int i = 0; i < NUM_EZBUTTONS; i++) {       // Read the state of each button
    if ( ezButtonArray[i].getState() == LOW ) {   // LOW state means that button is pressed
      inputPinArray[i] = true;
    } else {
      inputPinArray[i] = false;
    }
  }
}

// Read through our pin input array, find active pins/buttons and add the keypad specific binary values together
void convertInputsToButtons() {
  byteOne = 0; //reset message to 0
  byteTwo = 0;
  for ( int i = 0; i < NUM_EZBUTTONS ; i++ ) {    // Iterate through each button in the array
    if ( inputPinArray[i] == true) {              // Only add values if the button is pressed
      if ( i == 0) {
        byteOne += btn1;
      } else if ( i == 1 ) {
        byteOne += btn2;
      } else if ( i == 2 ) {
        byteOne += btn3;
      } else if ( i == 3) {
        byteOne += btn4;
      } else if ( i == 4 ) {
        byteOne += btn5;
      } else if ( i == 5 ) {
        byteOne += btn6;
      } else if ( i == 6 ) {
        byteOne += btn7;
      } else if ( i == 7 ) {
        byteOne += btn8;
      } else if ( i == 8 ) {
        byteTwo += btn9;        // Button 9 = Start of byteTwo
      } else if ( i == 9 ) {
        byteTwo += btn10;
      } else if ( i == 10 ) {
        byteTwo += btn11;
      } else if ( i == 11 ) {
        byteTwo += btn12;
      } else if ( i == 12 ) {    // NOTE: skipped pin 13
        byteTwo += btn13;
      } else if ( i == 14 ) {
        byteTwo += btn14;
      } else if ( i == 15 ) {
        byteTwo += btn15;
      }
    }
  }
}

// Sends a basic CAN keypad "keep alive" msg for ECU
void CANKeepAliveFrames() {
  ka2.id = 1805;
  ka2.flags.extended = false;
  ka2.len = 1;
  ka2.buf[0] = 5;
  can1.write(ka2);
}

// Takes in ECU msgs and basically emulates more CAN keypad "keep alive" responses
void processFrame(CAN_message_t &frame) {
  int b0, b1, b2, b3, b4, b5, b6, b7;

  // Keypad Configuration Section
  if (frame.id == 1549) {
    if ((frame.buf[0]) == 34 ) {
      b0 = 96;
      b1 = (frame.buf[1]);
      b2 = (frame.buf[2]);
      b3 = (frame.buf[3]);
      b4 = 0;
      b5 = 0;
      b6 = 0;
      b7 = 0;
      CANSyncFrames(b0, b1, b2, b3, b4, b5, b6, b7);
    } else if ((frame.buf[0]) == 66 ) {
      b0 = 67;
      b1 = (frame.buf[1]);
      b2 = (frame.buf[2]);
      b3 = (frame.buf[3]);
      if ((b1 == 24) && (b2 == 16) && (b3 == 1)) {
        b4 = 7;
        b5 = 4;
        b6 = 0;
        b7 = 0;
      } else if ((b1 == 24) && (b2 == 16) && (b3 == 2)) {
        b4 = 75;
        b5 = 51;
        b6 = 0;
        b7 = 0;
      } else if ((b1 == 24) && (b2 == 16) && (b3 == 3)) {
        b4 = 1;
        b5 = 0;
        b6 = 0;
        b7 = 0;
      } else if ((b1 == 24) && (b2 == 16) && (b3 == 4)) {
        b4 = 166;
        b5 = 184;
        b6 = 25;
        b7 = 12;
      } else if ((b1 == 0) && (b2 == 24) && (b3 == 1)) {
        b4 = 141;
        b5 = 1;
        b6 = 0;
        b7 = 64;
      } else {
        b4 = 0;
        b5 = 0;
        b6 = 0;
        b7 = 0;
      }
      CANSyncFrames(b0, b1, b2, b3, b4, b5, b6, b7);
    } else if (((frame.buf[0]) == 0) && ((frame.buf[7]) == 200)) {
      b0 = 128;
      b1 = 0;
      b2 = 0;
      b3 = 0;
      b4 = 1;
      b5 = 0;
      b6 = 4;
      b7 = 5;
      CANSyncFrames(b0, b1, b2, b3, b4, b5, b6, b7);
    }
  }
}

// Sends the above emulated CAN keypad "keep alive" response
void CANSyncFrames(int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7) {
  ka1.id = 1421;
  ka1.flags.extended = false;
  ka1.len = 8;
  ka1.buf[0] = b0;
  ka1.buf[1] = b1;
  ka1.buf[2] = b2;
  ka1.buf[3] = b3;
  ka1.buf[4] = b4;
  ka1.buf[5] = b5;
  ka1.buf[6] = b6;
  ka1.buf[7] = b7;
  can1.write(ka1);
}

// Send our CAN frames emulating Haltech CAN Keypad buttons.
void sendCANButtons() {
  outgoing.id = 397;
  outgoing.flags.extended = false;
  outgoing.len = 3;
  outgoing.buf[0] = byteOne;
  outgoing.buf[1] = byteTwo;
  outgoing.buf[2] = 0;
  can1.write(outgoing);
}
