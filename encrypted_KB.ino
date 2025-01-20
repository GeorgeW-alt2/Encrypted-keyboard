#include "hidboot.h"
#include "usbhub.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif
// XOR Encryption/Decryption Function for Arduino/C++

// Simple XOR encryption key (can be changed)
byte ENCRYPTION_KEY = 0x00;  // Example key, can be any byte value

// Function to XOR encrypt/decrypt a single character
char xorCrypt(char input) {
  return input ^ ENCRYPTION_KEY;
}

class KbdRptParser : public KeyboardReportParser
{
    void PrintKey(uint8_t mod, uint8_t key);

  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);

    void OnKeyDown	(uint8_t mod, uint8_t key);
    void OnKeyUp	(uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  MODIFIERKEYS mod;
  *((uint8_t*)&mod) = m;
  /*
  Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
  Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
  Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");
  Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");

  Serial.print(" >");
  PrintHex<uint8_t>(key, 0x80);
  Serial.print("< ");

  Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");
  Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
  Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");
  Serial.println((mod.bmRightGUI    == 1) ? "G" : " ");
  */
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  //Serial.print("DN ");
  //PrintKey(mod, key);
  uint8_t c = OemToAscii(mod, key);

  if (c)
    OnKeyPressed(c);
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {
    Serial.println("LeftCtrl changed");
  }
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  //Serial.print("UP ");
  //PrintKey(mod, key);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
  //Serial.print("ASCII: ");
  if (ENCRYPTION_KEY == 0x00){
    ENCRYPTION_KEY = (byte)key;
  }
  Serial.print((char)key ^ ENCRYPTION_KEY);
};

USB     Usb;
//USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

KbdRptParser Prs;

void setup()
{
  Serial.begin( 115200 );
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay( 200 );

  HidKeyboard.SetReportParser(0, &Prs);
}

void loop()
{
  Usb.Task();
}

