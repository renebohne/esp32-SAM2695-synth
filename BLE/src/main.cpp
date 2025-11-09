/**
 * @file
 * @brief This sketch turns an ESP32 into an advanced MIDI utility.
 */
#include <Arduino.h>

#include <Control_Surface.h>
#include <MIDI_Interfaces/BluetoothMIDI_Interface.hpp>
#include <MIDI_Interfaces/USBMIDI_Interface.hpp>

using namespace cs;


// --- Pin Definitions ---
#define BUTTON_A_PIN 5 // Decrease Program
#define BUTTON_B_PIN 4 // Increase Program
#define BUTTON_C_PIN 3 // Toggle Bank
#define BUTTON_D_PIN 2 // Toggle Reverb

#if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define LED_PIN 10
    #define LED_BUILTIN 10
#else
    #define LED_PIN LED_BUILTIN
#endif


BluetoothMIDI_Interface midi_ble;
HardwareSerialMIDI_Interface midi_ser {Serial0};
BidirectionalMIDI_Pipe ble_pipe;

#if defined(CONFIG_IDF_TARGET_ESP32S3)
USBMIDI_Interface midi_usb;
BidirectionalMIDI_Pipe usb_pipe;
#endif

AH::Button buttonA_phys = {BUTTON_A_PIN}; // Decrement
AH::Button buttonB_phys = {BUTTON_B_PIN}; // Increment
AH::Button buttonC_phys = {BUTTON_C_PIN}; // Toggle Bank
AH::Button buttonD_phys = {BUTTON_D_PIN}; // Toggle Reverb

// --- State Variables for Toggles ---
bool mt32_mode = false;     // false = GM Mode (Bank 0), true = MT-32 Mode (Bank 127)
bool reverbState = false; // false = Reverb Off, true = Reverb On
uint8_t currentProgram[16] = {0}; // Holds the current program number (0-127) for each channel

void toggleBank(cs::Channel chan) {
  mt32_mode = !mt32_mode;
  // CC #0 (Bank Select MSB)
  // Value 0 = General MIDI Bank
  // Value 127 = MT-32 Compatible Bank
  uint8_t bankValue = mt32_mode ? 127 : 0;
  Control_Surface.sendControlChange({0, chan}, bankValue);
  // Resend the program for this channel to load the patch from the new bank
  Control_Surface.sendProgramChange(chan, currentProgram[chan.getRaw()]);
}

void toggleReverb() {
  reverbState = !reverbState;
  // CC #91 is standard GM Reverb Send Level
  // This function remains hard-coded to Channel 1
  uint8_t reverbValue = reverbState ? 96 : 0;
  Control_Surface.sendControlChange({91, Channel_1}, reverbValue);
}

void incrementProgram(cs::Channel chan) {
  if (currentProgram[chan.getRaw()] < 127) {
    currentProgram[chan.getRaw()]++;
    Control_Surface.sendProgramChange(chan, currentProgram[chan.getRaw()]);
  }
}

void decrementProgram(cs::Channel chan) {
  if (currentProgram[chan.getRaw()] > 0) {
    currentProgram[chan.getRaw()]--;
    Control_Surface.sendProgramChange(chan, currentProgram[chan.getRaw()]);
  }
}

// --- MIDI Input Callback Class ---
class MyCallbacks : public MIDI_Callbacks {
  public:
    void onChannelMessage(MIDI_Interface &, ChannelMessage msg) override {
      if (msg.getMessageType() == MIDIMessageType::ControlChange)
      {
        uint8_t controller = msg.getData1();
        uint8_t value = msg.getData2();
        if ( value > 0) {
            if (controller == 14) {
                decrementProgram(msg.getChannel());
            } else if (controller == 15) {
                incrementProgram(msg.getChannel());
            } else if (controller == 13) {
                toggleBank(msg.getChannel());
            }
        }
      }
      else if (msg.getMessageType() == MIDIMessageType::ProgramChange)
      {
        currentProgram[msg.getChannel().getRaw()] = msg.getData1();
      }
    }
};

MyCallbacks callbacks;

void setup() {
  midi_ble.setCallbacks(callbacks);

  #if defined(CONFIG_IDF_TARGET_ESP32S3)
  midi_usb.setCallbacks(callbacks);
  #endif

  midi_ble.setName("XIAO Synth");
  
  midi_ser | ble_pipe | midi_ble;
  
  #if defined(CONFIG_IDF_TARGET_ESP32S3)
  midi_ser | usb_pipe | midi_usb;
  #endif
  
  buttonA_phys.begin();
  buttonB_phys.begin();
  buttonC_phys.begin();
  buttonD_phys.begin();

  Control_Surface.begin();
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  Control_Surface.loop();

  if (buttonA_phys.update() == AH::Button::Falling) {
    decrementProgram(Channel_1);
  }
  if (buttonB_phys.update() == AH::Button::Falling) {
    incrementProgram(Channel_1);
  }
  if (buttonC_phys.update() == AH::Button::Falling) {
    toggleBank(Channel_1);
  }
  if (buttonD_phys.update() == AH::Button::Falling) {
    toggleReverb();
  }
  
  digitalWrite(LED_BUILTIN, midi_ble.isConnected() ? HIGH : LOW);
}