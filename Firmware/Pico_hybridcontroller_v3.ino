#include <Adafruit_TinyUSB.h>

// Create separate HID instances for keyboard and mouse
Adafruit_USBD_HID usb_keyboard;
Adafruit_USBD_HID usb_mouse;

// HID report descriptors
uint8_t const desc_keyboard_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

uint8_t const desc_mouse_report[] = {
  TUD_HID_REPORT_DESC_MOUSE()
};

// Joystick pins (UNCHANGED)
const int horzPin = 26;
const int vertPin = 27;
const int selPin = 16;  // Toggle switch

// Button pins - REMAPPED GP01 to GP11
const int btnE = 1;              // GP01 - E key
const int btnLeftShift = 2;      // GP02 - Left Shift key
const int btnSpace = 3;          // GP03 - Space key
const int btnTab = 4;            // GP04 - Tab key
const int btnArrowUp = 5;        // GP05 - Arrow Up
const int btnArrowDown = 6;      // GP06 - Arrow Down
const int btnArrowLeft = 7;      // GP07 - Arrow Left
const int btnArrowRight = 8;     // GP08 - Arrow Right
const int btnRMB = 9;            // GP09 - Right Mouse Button
const int btnLMB = 10;           // GP10 - Left Mouse Button
const int btnMMB = 11;           // GP11 - Middle Mouse Button

int horzZero, vertZero; 
int horzValue, vertValue;

const int sensitivity = 10;
const int keyThreshold = 300;

// Toggle mode variables
bool isMouseMode = true;
bool lastButtonState = HIGH;
bool buttonPressed = false;

// Keyboard state tracking for WASD
bool wPressed = false, aPressed = false, sPressed = false, dPressed = false;

// Button state tracking - PRESSED states
bool ePressedState = false;
bool shiftPressedState = false;
bool spacePressedState = false;
bool tabPressedState = false;
bool arrowUpPressedState = false;
bool arrowDownPressedState = false;
bool arrowLeftPressedState = false;
bool arrowRightPressedState = false;

// Button state tracking - LAST READ states (for debouncing)
bool lastBtnE = HIGH, lastBtnLeftShift = HIGH, lastBtnSpace = HIGH, lastBtnTab = HIGH;
bool lastBtnArrowUp = HIGH, lastBtnArrowDown = HIGH, lastBtnArrowLeft = HIGH, lastBtnArrowRight = HIGH;
bool lastBtnLMB = HIGH, lastBtnRMB = HIGH, lastBtnMMB = HIGH;

void setup() {
  // Joystick pins
  pinMode(horzPin, INPUT);
  pinMode(vertPin, INPUT);
  pinMode(selPin, INPUT_PULLUP);
  
  // Button pins - all with pull-up resistors
  pinMode(btnE, INPUT_PULLUP);
  pinMode(btnLeftShift, INPUT_PULLUP);
  pinMode(btnSpace, INPUT_PULLUP);
  pinMode(btnTab, INPUT_PULLUP);
  pinMode(btnArrowUp, INPUT_PULLUP);
  pinMode(btnArrowDown, INPUT_PULLUP);
  pinMode(btnArrowLeft, INPUT_PULLUP);
  pinMode(btnArrowRight, INPUT_PULLUP);
  pinMode(btnLMB, INPUT_PULLUP);
  pinMode(btnRMB, INPUT_PULLUP);
  pinMode(btnMMB, INPUT_PULLUP);
  
  // Setup keyboard HID
  usb_keyboard.setPollInterval(2);
  usb_keyboard.setReportDescriptor(desc_keyboard_report, sizeof(desc_keyboard_report));
  usb_keyboard.setStringDescriptor("TinyUSB Keyboard");
  
  // Setup mouse HID
  usb_mouse.setPollInterval(2);
  usb_mouse.setReportDescriptor(desc_mouse_report, sizeof(desc_mouse_report));
  usb_mouse.setStringDescriptor("TinyUSB Mouse");
  
  usb_keyboard.begin();
  usb_mouse.begin();
  
  while (!TinyUSBDevice.mounted()) delay(1);
  
  delay(1000); 
  
  vertZero = analogRead(vertPin);
  horzZero = analogRead(horzPin);
}

void loop() {
  if (TinyUSBDevice.suspended()) {
    TinyUSBDevice.remoteWakeup();
  }
  
  // Handle toggle button (joystick switch)
  bool currentButtonState = digitalRead(selPin);
  
  if (currentButtonState == LOW && lastButtonState == HIGH && !buttonPressed) {
    buttonPressed = true;
    isMouseMode = !isMouseMode;
    
    if (isMouseMode) {
      releaseAllKeys();
    }
    
    delay(300);
  }
  
  if (currentButtonState == HIGH) {
    buttonPressed = false;
  }
  
  lastButtonState = currentButtonState;
  
  // Read joystick values
  vertValue = analogRead(vertPin) - vertZero; 
  horzValue = analogRead(horzPin) - horzZero;
  
  if (isMouseMode) {
    // ===== MOUSE MODE =====
    int8_t mouseX = 0;
    int8_t mouseY = 0;
    
    if (abs(vertValue) > sensitivity) {
      mouseY = vertValue / -sensitivity;
    }
    
    if (abs(horzValue) > sensitivity) {
      mouseX = horzValue / sensitivity;
    }
    
    // Handle mouse buttons
    uint8_t mouseButtons = 0;
    
    if (digitalRead(btnLMB) == LOW) {
      mouseButtons |= MOUSE_BUTTON_LEFT;
    }
    
    if (digitalRead(btnRMB) == LOW) {
      mouseButtons |= MOUSE_BUTTON_RIGHT;
    }
    
    if (digitalRead(btnMMB) == LOW) {
      mouseButtons |= MOUSE_BUTTON_MIDDLE;
    }
    
    usb_mouse.mouseReport(0, mouseButtons, mouseX, mouseY, 0, 0);
    
  } else {
    // ===== KEYBOARD MODE (WASD) =====
    bool currentW = vertValue < -keyThreshold;
    bool currentS = vertValue > keyThreshold;
    bool currentA = horzValue < -keyThreshold;
    bool currentD = horzValue > keyThreshold;
    
    if (currentW != wPressed || currentS != sPressed || 
        currentA != aPressed || currentD != dPressed) {
      
      wPressed = currentW;
      sPressed = currentS;
      aPressed = currentA;
      dPressed = currentD;
    }
  }
  
  // ===== HANDLE KEYBOARD BUTTONS (work in both modes) =====
  handleKeyboardButtons();
  
  // Send the complete keyboard report
  sendAllKeys();
  
  delay(10);
}

void handleKeyboardButtons() {
  // E key
  bool currentBtnE = digitalRead(btnE);
  if (currentBtnE == LOW && lastBtnE == HIGH) {
    ePressedState = true;
  } else if (currentBtnE == HIGH && lastBtnE == LOW) {
    ePressedState = false;
  }
  lastBtnE = currentBtnE;
  
  // Left Shift key
  bool currentBtnLeftShift = digitalRead(btnLeftShift);
  if (currentBtnLeftShift == LOW && lastBtnLeftShift == HIGH) {
    shiftPressedState = true;
  } else if (currentBtnLeftShift == HIGH && lastBtnLeftShift == LOW) {
    shiftPressedState = false;
  }
  lastBtnLeftShift = currentBtnLeftShift;
  
  // Space key
  bool currentBtnSpace = digitalRead(btnSpace);
  if (currentBtnSpace == LOW && lastBtnSpace == HIGH) {
    spacePressedState = true;
  } else if (currentBtnSpace == HIGH && lastBtnSpace == LOW) {
    spacePressedState = false;
  }
  lastBtnSpace = currentBtnSpace;
  
  // Tab key
  bool currentBtnTab = digitalRead(btnTab);
  if (currentBtnTab == LOW && lastBtnTab == HIGH) {
    tabPressedState = true;
  } else if (currentBtnTab == HIGH && lastBtnTab == LOW) {
    tabPressedState = false;
  }
  lastBtnTab = currentBtnTab;
  
  // Arrow Up key
  bool currentBtnArrowUp = digitalRead(btnArrowUp);
  if (currentBtnArrowUp == LOW && lastBtnArrowUp == HIGH) {
    arrowUpPressedState = true;
  } else if (currentBtnArrowUp == HIGH && lastBtnArrowUp == LOW) {
    arrowUpPressedState = false;
  }
  lastBtnArrowUp = currentBtnArrowUp;
  
  // Arrow Down key
  bool currentBtnArrowDown = digitalRead(btnArrowDown);
  if (currentBtnArrowDown == LOW && lastBtnArrowDown == HIGH) {
    arrowDownPressedState = true;
  } else if (currentBtnArrowDown == HIGH && lastBtnArrowDown == LOW) {
    arrowDownPressedState = false;
  }
  lastBtnArrowDown = currentBtnArrowDown;
  
  // Arrow Left key
  bool currentBtnArrowLeft = digitalRead(btnArrowLeft);
  if (currentBtnArrowLeft == LOW && lastBtnArrowLeft == HIGH) {
    arrowLeftPressedState = true;
  } else if (currentBtnArrowLeft == HIGH && lastBtnArrowLeft == LOW) {
    arrowLeftPressedState = false;
  }
  lastBtnArrowLeft = currentBtnArrowLeft;
  
  // Arrow Right key
  bool currentBtnArrowRight = digitalRead(btnArrowRight);
  if (currentBtnArrowRight == LOW && lastBtnArrowRight == HIGH) {
    arrowRightPressedState = true;
  } else if (currentBtnArrowRight == HIGH && lastBtnArrowRight == LOW) {
    arrowRightPressedState = false;
  }
  lastBtnArrowRight = currentBtnArrowRight;
}

void sendAllKeys() {
  uint8_t keycodes[6] = {0};
  uint8_t modifier = 0;
  int index = 0;
  
  // Add WASD keys (only in keyboard mode)
  if (!isMouseMode) {
    if (wPressed && index < 6) keycodes[index++] = HID_KEY_W;
    if (aPressed && index < 6) keycodes[index++] = HID_KEY_A;
    if (sPressed && index < 6) keycodes[index++] = HID_KEY_S;
    if (dPressed && index < 6) keycodes[index++] = HID_KEY_D;
  }
  
  // Add button keys (work in both modes)
  if (ePressedState && index < 6) keycodes[index++] = HID_KEY_E;
  if (spacePressedState && index < 6) keycodes[index++] = HID_KEY_SPACE;
  if (tabPressedState && index < 6) keycodes[index++] = HID_KEY_TAB;
  if (arrowUpPressedState && index < 6) keycodes[index++] = HID_KEY_ARROW_UP;
  if (arrowDownPressedState && index < 6) keycodes[index++] = HID_KEY_ARROW_DOWN;
  if (arrowLeftPressedState && index < 6) keycodes[index++] = HID_KEY_ARROW_LEFT;
  if (arrowRightPressedState && index < 6) keycodes[index++] = HID_KEY_ARROW_RIGHT;
  
  // Handle modifier key (Shift) separately
  if (shiftPressedState) {
    modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
  }
  
  usb_keyboard.keyboardReport(0, modifier, keycodes);
}

void releaseAllKeys() {
  wPressed = false;
  aPressed = false;
  sPressed = false;
  dPressed = false;
  
  ePressedState = false;
  shiftPressedState = false;
  spacePressedState = false;
  tabPressedState = false;
  arrowUpPressedState = false;
  arrowDownPressedState = false;
  arrowLeftPressedState = false;
  arrowRightPressedState = false;
  
  uint8_t keycodes[6] = {0};
  usb_keyboard.keyboardReport(0, 0, keycodes);
}