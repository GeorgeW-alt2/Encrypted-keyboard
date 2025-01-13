/*
  Keyboard Controller HID Example with Encryption/Decryption
  Shows the output of a USB Keyboard connected to the USB
  controller of an Arduino Due Board, with encryption and decryption modes.
  Using 8-character password encryption and decoding.
*/

// Require keyboard control library
#include <KeyboardController.h>

// Initialize USB Controller
USBHost usb;

// Attach keyboard controller to USB
KeyboardController keyboard(usb);

// Encryption key (password)
String encryptionKey = "";

// Flag to indicate if encryption key is set
bool isKeySet = false;

// Decode mode flag
bool decodeMode = false;

// This function intercepts key press
void keyPressed() {
  Serial.print("Pressed:  ");
  printKey();
}

// This function intercepts key release
void keyReleased() {
  Serial.print("Released: ");
  printKey();
}

void printKey() {
  // getOemKey() returns the OEM-code associated with the key
  Serial.print(" key:");
  Serial.print(keyboard.getOemKey());

  // getModifiers() returns a bits field with the modifiers-keys
  int mod = keyboard.getModifiers();
  Serial.print(" mod:");
  Serial.print(mod);
  Serial.print(" => ");

  // Print modifier keys
  if (mod & LeftCtrl) Serial.print("L-Ctrl ");
  if (mod & LeftShift) Serial.print("L-Shift ");
  if (mod & Alt) Serial.print("Alt ");
  if (mod & LeftCmd) Serial.print("L-Cmd ");
  if (mod & RightCtrl) Serial.print("R-Ctrl ");
  if (mod & RightShift) Serial.print("R-Shift ");
  if (mod & AltGr) Serial.print("AltGr ");
  if (mod & RightCmd) Serial.print("R-Cmd ");

  // Check for Control key to toggle modes
  if (mod & (LeftCtrl | RightCtrl)) {
    toggleMode();
  }

  // Get the key and process it
  char key = keyboard.getKey();
  if (isKeySet) {
    if (decodeMode) {
      char decryptedKey = decryptCharacter(key);
      Serial.print("Decrypted Key: ");
      Serial.println(decryptedKey);
    } else {
      char encryptedKey = encryptCharacter(key);
      Serial.print("Encrypted Key: ");
      Serial.println(encryptedKey);
    }
  } else {
    Serial.println("Encryption key not set.");
  }
}

// Function to encrypt a character using XOR encryption with an 8-character key
char encryptCharacter(char inputChar) {
  static int keyIndex = 0;  // Keep track of the current key character
  char encryptedChar = inputChar ^ encryptionKey[keyIndex];
  keyIndex = (keyIndex + 1) % 8;  // Loop through the 8-character key
  return encryptedChar;
}

// Function to decrypt a character using XOR encryption with an 8-character key
char decryptCharacter(char inputChar) {
  return encryptCharacter(inputChar);  // XOR is symmetric for encryption and decryption
}

// Function to set the 8-character encryption key
void setEncryptionKey() {
  Serial.println("Enter an 8-character encryption key:");
  while (!Serial.available()) {
    // Wait for user input
  }
  encryptionKey = Serial.readStringUntil('\n');
  encryptionKey.trim();  // Remove any trailing whitespace or newline

  if (encryptionKey.length() == 8) {
    isKeySet = true;
    Serial.print("Encryption key set to: ");
    Serial.println(encryptionKey);
  } else {
    Serial.println("Invalid key. Please enter exactly 8 characters.");
    setEncryptionKey();  // Prompt user again
  }
}

// Function to toggle between encode and decode modes
void toggleMode() {
  if (decodeMode) {
    decodeMode = false;
    Serial.println("Switched to ENCODE mode.");
  } else {
    decodeMode = true;
    Serial.println("Switched to DECODE mode.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Program started");
  delay(200);

  // Prompt user to set the encryption key
  setEncryptionKey();
  Serial.println("Press CTRL to switch encryption mode.");
}

void loop() {
  // Process USB tasks
  usb.Task();
}
