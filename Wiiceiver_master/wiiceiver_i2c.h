
#ifndef WIICEIVER_I2C_H
#define WIICEIVER_I2C_H

#include <Arduino.h>
#include <Wire.h>

/*
 *  This is a super-simple i2c transfer of a bytefield.  i2c messages
 *  are limited to 32 bytes by the Wire library.  We use arbitrary-length
 *  messages of PACKET_SIZE payload.  An additional leading byte is the
 *  starting address (offset) of the payload.
 *
 *  An address of 255 + no payload == "stop" (no more payloads in this 
 *  series).
 *
 *  The sender and receiver "share" a common bytefield / packet format.
 *  Sender starts blasting down changes, and finishes with the "stop".
 *  Sender could arbitrarily send random sections of the field, but 
 *  generally wouldn't.
 *
 *  Receiver just starts writing payload at the prvoided offset.  When
 *  the "stop" is received, it writes a flag called "lock" (not really
 *  a lock).  The slave should watch this flag and copy the buffer to 
 *  a new location, lest it get asynchronously get changed while 
 *  processing.
 *
 *  Timing: ~20-25ms to send the whole packet (about 170 bytes)
 *          ~5-7ms to send all but the "text" fields (40-50 bytes)
 *          ~40-60ms to update a slick display
 *
 *  In practice, wiiceiver will send most data all the time, occasionally
 *  send the text data.  The slave will spend most of its time updating
 *  the display.
 *
 *  I elected for the union because it's low-cost and easier to grok
 */

  #define WIICEIVER_I2C           53   // i2c slave address

  typedef struct StatusMessage_t {
    float chuckY;            // -1 .. 1
    bool chuckC, chuckZ;     // up/down state
    byte chuckCcounter, chuckZcounter;   // 0..250, #cycles since last change
    float peakDischarge;     // Amps
    float peakRegen;         // Amps
    float totalDischarge;    // mAh
    float totalRegen;        // mAh
    float current;           // Amps
    float throttle;          // -1 .. 1
    int uptime;              // seconds
    int lastWritten;         // seconds
    float history[3];        // previous net discharge, mAh
    char text[4][18];        // raw text fields
  }; 
  
  typedef union StatusPacket_t {
    StatusMessage_t message;
    byte bytes[sizeof(StatusMessage_t)];
  };
  
  StatusPacket_t statusPacket;
  uint8_t lock;


  #define PACKET_SIZE 30
  void xmit(const byte* buffer, byte length) {
    byte b = 0;
    
    while (b < length) {
      Wire.beginTransmission(WIICEIVER_I2C); 
      Wire.write(b);
      
      for (byte x = 0; x < PACKET_SIZE && b+x < length; x++) {
        byte data = (byte)(b+x);
        Wire.write(buffer[b+x]);
      } 
      b += PACKET_SIZE;
      
      Wire.endTransmission();
    } 
    
    Wire.beginTransmission(WIICEIVER_I2C); 
    Wire.write((byte)255);
    Wire.endTransmission();
  } // xmit(buffer, length)
  
  
  /*
   * asynch :/
   *
   * encode bytes starting at the address in the first byte
   * stop (set the lock) if we receive only a 255
   */
  void receiveEvent(int howMany) {
    unsigned long start = millis();
    byte b = Wire.read();
    while(Wire.available()) {
      statusPacket.bytes[b++] = Wire.read(); 
    }
    
    lock = (b == 255 && howMany == 1);
    return;
  } // receiveEvent(howMany)
  
#endif
