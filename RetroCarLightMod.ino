#include "lcdAndRelays.h"
#include "defsAndInputs.h"
#define RIGHT_BLINKER 0
#define LEFT_BLINKER 1
#define HEADLIGHT 2
#define HIGHBEAM 3
#define MARKER_LIGHT 4
#define BRAKE_LIGHT 5
#define HAZARD_PBTN 6


#define HEADLIGHT_RELAY 3
#define HIGHBEAM_RELAY 4
#define MARKER_LIGHT_RELAY 5
#define BRAKE_LIGHT_RELAY 6

#define IGNITION 7
#define IGNITION_RELAY 8

unsigned int beamState = 0;
unsigned int bState = 0;
const unsigned long blinkDuration = 500;  // Duration for each state of blink (on/off)
unsigned long previousMillis = 0;         // Stores last update time
bool markerFlag = false;
// Variables to track the state of the hazard lights and debounce timing
bool hazardActive = false;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time in milliseconds

// will quickly become a bigger number than can be stored in an int.
unsigned long time = 0;          // the last time the output pin was toggled
unsigned long debounce = 200UL;  // the debounce time, increase if the output flickers
int state = LOW;                 // the current state of the output pin
int reading;                     // the current reading from the input pin
int previous = LOW;              // the previous reading from the input pin


bool IgnitionIsOn = false;                // Tracks state of the ignition
unsigned long lastIgnitionPressTime = 0;  // Time when ignition was last pressed
bool waitingIgnition = false;             // True if we are waiting for 5 seconds after ignition
bool lastIgnitionState = LOW;             // Previous state of the ignition


void checkIgnition(int ignition, int ignitionRelay) {
  bool currentIgnitionState = (inValues[IGNITION] == IO_HIGH);
  if (inValues[IGNITION] == IO_HIGH) {
    setRelay(IGNITION_RELAY);
    setCustomChar(3, 0, 1, 1, 0, 0, 0, 0);  // Specific pattern for Marker Light
    TubeDisplay4Bit();
    IgnitionIsOn = true;
  } else if (inValues[IGNITION] == IO_LOW) {
    resetRelay(IGNITION_RELAY);
    setCustomChar(3, 0, 0, 0, 0, 0, 0, 0);  // Specific pattern for Marker Light
    TubeDisplay4Bit();
    IgnitionIsOn = false;
  }
}
void checkHighBeam(int highBeam, int highBeamRelay) {
  if (IgnitionIsOn && inValues[HEADLIGHT] == IO_LOW) {
    if (inValues[highBeam] == IO_HIGH) {
      setCustomChar(1, 0, 0, 0, 0, 1, 1, 0);  // Specific pattern for Marker Light
      TubeDisplay4Bit();
      setRelay(highBeamRelay);  // ON
    } else if (inValues[highBeam] == IO_LOW) {
      state = LOW;
      resetRelay(highBeamRelay);  // OFF
    }
  } else if (IgnitionIsOn && inValues[HEADLIGHT] == IO_HIGH) {
    reading = inValues[highBeam] == IO_HIGH;
    if (reading == HIGH && previous == LOW && millis() - time > debounce) {
      if (state == HIGH)
        state = LOW;
      else
        state = HIGH;

      time = millis();
    }
    if (state == HIGH) {
      resetRelay(HEADLIGHT_RELAY);            // ON
      markerFlag = true;
      setRelay(highBeamRelay);                // ON
      setCustomChar(1, 0, 0, 0, 0, 1, 1, 0);  // Specific pattern for Marker Light
      TubeDisplay4Bit();
    } else if (state == LOW) {
      setRelay(HEADLIGHT_RELAY);              // ON
      markerFlag = false;
      resetRelay(highBeamRelay);              // OFF
      setCustomChar(1, 0, 0, 0, 0, 0, 0, 0);  // Specific pattern for Marker Light
      TubeDisplay4Bit();
    }
    previous = reading;
  } else if (inValues[IGNITION] == IO_LOW) {
    state = LOW;
    resetRelay(highBeamRelay);  // OFF
  }
}

void checkBrakeLight(int brakeLight, int brakeLightRelay) {
  if (inValues[brakeLight] == IO_HIGH) {
    setCustomChar(2, 0, 0, 0, 0, 1, 1, 0);  // Specific pattern for Marker Light
    TubeDisplay4Bit();
    setRelay(brakeLightRelay);  // ON
  } else if (inValues[brakeLight] == IO_LOW) {
    resetRelay(brakeLightRelay);  // OFF
  }
}
void checkMarkerLight(int markerLight, int markerLightRelay) {
  if (inValues[markerLight] == IO_HIGH) {
    setCustomChar(2, 0, 0, 0, 0, 1, 1, 0);  // Specific pattern for Marker Light
    TubeDisplay4Bit();
    setRelay(markerLightRelay);  // ON
  } else if (inValues[markerLight] == IO_LOW) {
    resetRelay(markerLightRelay);  // OFF
  }
}

void checkHeadLight(int headLight, int headLightRelay) {
  if (IgnitionIsOn) {
    if (inValues[headLight] == IO_HIGH && markerFlag) {
      setCustomChar(2, 0, 0, 0, 0, 1, 1, 0);  // Specific pattern for Marker Light
      TubeDisplay4Bit();
      setRelay(headLightRelay);  // ON
    } else if (inValues[headLight] == IO_LOW) {
      resetRelay(headLightRelay);  // OFF
    }
  } else {
    resetRelay(headLightRelay);             // OFF
    setCustomChar(2, 0, 0, 0, 0, 0, 0, 0);  // Specific pattern for Marker Light
    TubeDisplay4Bit();
  }
}

void checkBlinkers(int rightBlinker, int leftBlinker, int relayOffset, unsigned long currentTime) {
  if (IgnitionIsOn && bState <= 0) {  // Only proceed if input at IGNITION is HIGH
    if (inValues[rightBlinker] == IO_HIGH && inValues[leftBlinker] == IO_HIGH) {
      // Ensure corresponding relays are reset to prevent both blinkers from working
      resetRelay(relayOffset);
      resetRelay(relayOffset + 1);
    } else if (inValues[rightBlinker] == IO_HIGH && inValues[leftBlinker] == IO_LOW) {
      setCustomChar(0, 0, 0, 0, 0, 1, 1, 0);  // Specific pattern for rightBlinker
      checkSingleButton(rightBlinker, relayOffset, currentTime);
    } else if (inValues[leftBlinker] == IO_HIGH && inValues[rightBlinker] == IO_LOW) {
      setCustomChar(0, 0, 1, 1, 0, 0, 0, 0);  // Specific pattern for leftBlinker
      checkSingleButton(leftBlinker, relayOffset + 1, currentTime);
    } else {
      resetRelay(relayOffset);  // Ensure relays are reset if the condition is not met
      resetRelay(relayOffset + 1);
    }
  } else {
    resetRelay(relayOffset);  // Ensure relays are reset if the condition is not met
    resetRelay(relayOffset + 1);
  }
}

void checkSingleButton(int blinker, int relay, unsigned long currentTime) {
  // Set custom character based on input index
  TubeDisplay4Bit();
  // Continue blinking
  if ((currentTime / blinkDuration) % 2 == 0) {
    setRelay(relay);  // ON
  } else {
    resetRelay(relay);  // OFF
  }
}

void setup() {
  initIO();  // Initialize the I/O pins for inputs and relays
}

void loop() {
  readINPUTS();  // Update the state of all inputs
  readKEYS();
  unsigned long currentMillis = millis();

  // Check for button press or input condition
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(K1);
  if (inValues[6] == IO_HIGH) {
    currentButtonState = !currentButtonState;
  }
  // Evaluate if there is a change in state or the input condition is met
  if (currentButtonState != lastButtonState) {
    if (currentButtonState == HIGH) {
      bState += 1;
    }
    lastButtonState = currentButtonState;
  }

  // Blink LED based on bState
  if (bState >= 1) {
    if (currentMillis - previousMillis >= blinkDuration) {
      previousMillis = currentMillis;  // Save the last update time

      // Toggle LED state
      bool ledState = getRelay(7);
      if (ledState == HIGH) {
        resetRelay(7);
      } else if (ledState == LOW) {
        setRelay(7);
      }
      // Reset bState if it's even, after one cycle of blinking
      if (!ledState && bState % 2 == 0) {  // This ensures it happens after the LED has turned off
        resetRelay(7);
        bState = 0;  // Reset bState
      }
    }
  } else {
    resetRelay(7);
  }
  // Check each pair of inputs

  checkBlinkers(RIGHT_BLINKER, LEFT_BLINKER, 1, currentMillis);  // Check IN1 (relay 1) and IN2 (relay 2)
  checkMarkerLight(MARKER_LIGHT, MARKER_LIGHT_RELAY);
  checkBrakeLight(BRAKE_LIGHT, BRAKE_LIGHT_RELAY);
  checkHeadLight(HEADLIGHT, HEADLIGHT_RELAY);
  checkHighBeam(HIGHBEAM, HIGHBEAM_RELAY);
  checkIgnition(IGNITION, IGNITION_RELAY);
  for (int i = 0; i < 4; i++) {
    setCustomChar(i, 0, 0, 0, 0, 0, 0, 0);  // Specific pattern for leftBlinker
  }
}
