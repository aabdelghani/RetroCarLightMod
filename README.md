
# Vehicle Lighting and Ignition Control System

## Overview
This section of the codebase is designed to manage the ignition system of a vehicle using digital inputs and outputs, and relays. It includes functionalities for monitoring and controlling the ignition state, and for updating display elements to reflect these states.

## Variable Definitions
- `unsigned int beamState`: Represents the state of the vehicle's beam lights. This variable might be used to store different beam modes.
- `unsigned int bState`: Manages blink sequences or states, though its usage is not detailed in this snippet.
- `const unsigned long blinkDuration`: Duration (500 ms) for each state of a blink (on/off), used in blink related functionalities.
- `unsigned long previousMillis`: Stores the last update time for operations requiring timing, such as blinking.
- `bool markerFlag`: Flag to indicate the status of marker lights, interaction not shown in provided code.
- `bool hazardActive`: Indicates the activation state of hazard lights; usage not shown.
- `unsigned long lastDebounceTime`: Tracks the last time the output pin was toggled for debouncing purposes.
- `unsigned long debounceDelay`: Debounce time (50 ms) to handle input signal stability.
- `unsigned long time`: Tracks time between state changes for debouncing or timed functions.
- `unsigned long debounce`: Duration (200 ms) to prevent output flickering.
- `int state`: Current state (HIGH/LOW) of the output pin controlled by this code.
- `int reading`: Current reading from the input pin related to ignition or high beams.
- `int previous`: Previous reading from the input pin to detect changes.
- `bool IgnitionIsOn`: Tracks whether the ignition is active.
- `unsigned long lastIgnitionPressTime`: Timestamp of the last ignition press.
- `bool waitingIgnition`: Indicates waiting state post-ignition press; usage not detailed.
- `bool lastIgnitionState`: Previous state of the ignition input to detect changes.

## Function Descriptions

### `checkIgnition(int ignition, int ignitionRelay)`
- **Parameters**:
  - `ignition`: Index/identifier for the input pin associated with the ignition.
  - `ignitionRelay`: Index/identifier for the relay controlling the ignition.
- **Functionality**:
  - Reads the ignition state and updates the ignition relay accordingly.
  - Updates a custom character on the display to visually represent the ignition state.
- **Display Interaction**:
  - `setCustomChar(3, 0, 1, 1, 0, 0, 0, 0)`: Sets a specific pattern for when the ignition is on.
  - `setCustomChar(3, 0, 0, 0, 0, 0, 0, 0)`: Resets the display pattern for when the ignition is off.
  - `TubeDisplay4Bit()`: Likely pushes display data to the hardware, mechanism depends on specific hardware setup.

This function integrates hardware control with visual feedback, crucial for systems requiring operator awareness of the ignition state.


## Function: `checkHighBeam(int highBeam, int highBeamRelay)`

This function is responsible for controlling the high beam lights of a vehicle based on the ignition and headlight state. It interacts with the system's I/O values to adjust relay states and update display settings accordingly.

- **Parameters**:
  - `highBeam`: The index for the input pin associated with the high beam.
  - `highBeamRelay`: The index for the relay controlling the high beam.

- **Functionality**:
  - **Ignition On and Headlight Off**:
    - If the high beam input is high (`IO_HIGH`), the high beam is turned on. This includes setting a specific display pattern and activating the corresponding relay.
    - If the high beam input is low (`IO_LOW`), the high beam is turned off by resetting the relay.
  - **Ignition On and Headlight On**:
    - The function checks for changes in the high beam input and toggles the state of the high beam relay based on a debounce timer. This helps prevent flickering and ensures that the lights reflect the latest driver input accurately.
    - When toggling, the function also updates the display pattern to indicate the current state of the high beam.
  - **Ignition Off**:
    - Regardless of the high beam input, the high beam is turned off when the ignition is off.

- **Display Interaction**:
  - `setCustomChar(1, 0, 0, 0, 0, 1, 1, 0)`: Displays a pattern indicating that the high beam is on.
  - `TubeDisplay4Bit()`: Pushes the display data to the display unit, visually indicating the current state of the high beams.
  - When the high beam is turned off, the display is updated to a neutral pattern.

This method ensures that the high beam lights are managed automatically based on the driver’s input and the vehicle’s operational status, enhancing both safety and convenience.

