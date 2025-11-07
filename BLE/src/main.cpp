#include <Arduino.h>
#include "soc/soc_caps.h"

#define DEVICE_NAME "XIAO-SYNTH"

#ifndef SOC_BLE_SUPPORTED	
#error This ESP32 SoC has no BLE 
#else

#include "SAM2695Synth.h"

uint8_t current_instrument = 0;

SAM2695Synth<HardwareSerial> synth = SAM2695Synth<HardwareSerial>::getInstance();

#if defined(CONFIG_IDF_TARGET_ESP32S3)
    #define BUTTON_A_PIN 4
    #define BUTTON_B_PIN 3
    #define BUTTON_C_PIN 2 
    #define BUTTON_D_PIN 1 
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    #define BUTTON_A_PIN 5
    #define BUTTON_B_PIN 4
    #define BUTTON_C_PIN 3 
    #define BUTTON_D_PIN 2 
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    #define BUTTON_A_PIN 21
    #define BUTTON_B_PIN 2
    #define BUTTON_C_PIN 1 
    #define BUTTON_D_PIN 0
#endif

#if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define LED_PIN 10              //You need to connect a light yourself
#else
    #define LED_PIN LED_BUILTIN
#endif

#include <Control_Surface.h>
#include <MIDI_Interfaces/BluetoothMIDI_Interface.hpp>

// Instantiate a MIDI over BLE interface
BluetoothMIDI_Interface midi_ble;

struct MyMIDI_Callbacks : FineGrainedMIDI_Callbacks<MyMIDI_Callbacks> {
  // Note how this ^ name is identical to the argument used here ^

  void onNoteOff(Channel channel, uint8_t note, uint8_t velocity, Cable cable) {
    Serial << "Note Off: " << channel << ", note " << note << ", velocity "
           << velocity << ", " << cable << endl;
    synth.setNoteOff(channel.getRaw(), note);
  }
  void onNoteOn(Channel channel, uint8_t note, uint8_t velocity, Cable cable) {
    Serial << "Note On: " << channel << ", note " << note << ", velocity "
           << velocity << ", " << cable << endl;
    synth.setNoteOn(channel.getRaw(), note, velocity);
  }
  void onKeyPressure(Channel channel, uint8_t note, uint8_t pressure,
                     Cable cable) {
    Serial << "Key Pressure: " << channel << ", note " << note << ", pressure "
           << pressure << ", " << cable << endl;
  }
  void onControlChange(Channel channel, uint8_t controller, uint8_t value,
                       Cable cable) {
    Serial << "Control Change: " << channel << ", controller " << controller
           << ", value " << value << ", " << cable << endl;

    if(controller == 7)
    {
      synth.setVolume(channel.getRaw(), value);
    }
    else if(controller == 14)
    {
      current_instrument--;
      if(current_instrument ==255)
      {
        current_instrument = 127;
      }
      synth.setInstrument(0,channel.getRaw(),current_instrument);
      Serial.print("set instrument to: ");
      Serial.println(current_instrument);
    }
    else if(controller == 15)
    {
      current_instrument++;
      if(current_instrument ==128)
      {
        current_instrument = 0;
      }
      synth.setInstrument(0,channel.getRaw(),current_instrument);
      Serial.print("set instrument to: ");
      Serial.println(current_instrument);
    }



  }
  void onProgramChange(Channel channel, uint8_t program, Cable cable) {
    Serial << "Program Change: " << channel << ", program " << program << ", "
           << cable << endl;
    synth.setInstrument(0,channel.getRaw(),program);
  }
  void onChannelPressure(Channel channel, uint8_t pressure, Cable cable) {
    Serial << "Channel Pressure: " << channel << ", pressure " << pressure
           << ", " << cable << endl;
  }
  void onPitchBend(Channel channel, uint16_t bend, Cable cable) {
    Serial << "Pitch Bend: " << channel << ", bend " << bend << ", " << cable
           << endl;
  }
  void onSystemExclusive(SysExMessage se) {
    Serial << F("System Exclusive: [") << se.length << "] "
           << AH::HexDump(se.data, se.length) << ", " << se.cable << endl;
  }
  void onTimeCodeQuarterFrame(uint8_t data, Cable cable) {
    Serial << "MTC Quarter Frame: " << data << ", " << cable << endl;
  }
  void onSongPosition(uint16_t beats, Cable cable) {
    Serial << "Song Position Pointer: " << beats << ", " << cable << endl;
  }
  void onSongSelect(uint8_t songnumber, Cable cable) {
    Serial << "Song Select: " << songnumber << ", " << cable << endl;
  }
  void onTuneRequest(Cable cable) {
    Serial << "Tune Request: " << cable << endl;
  }
  //void onClock(Cable cable) { Serial << "Timing Clock: " << cable << endl; }
  void onStart(Cable cable) { Serial << "Start: " << cable << endl; }

  void onContinue(Cable cable) { Serial << "Continue: " << cable << endl; }

  void onStop(Cable cable){
     Serial << "Stop: " << cable << endl;
     synth.setAllNotesOff(0); 
     synth.setAllNotesOff(10); 
  }

  void onActiveSensing(Cable cable) {
    Serial << "Active Sensing: " << cable << endl;
  }
  void onSystemReset(Cable cable) {
    Serial << "System Reset: " << cable << endl;
  }

} callback;

void setup() {
  Serial.begin(115200);
  delay(3000);
 
  

  midi_ble.setName(DEVICE_NAME);
  midi_ble.begin();
  midi_ble.setCallbacks(callback);

  synth.begin(Serial0, MIDI_SERIAL_BAUD_RATE);
  synth.setInstrument(0,CHANNEL_0,current_instrument);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_A_PIN,INPUT_PULLUP);
  pinMode(BUTTON_B_PIN,INPUT_PULLUP);
  pinMode(BUTTON_C_PIN,INPUT_PULLUP);
  pinMode(BUTTON_D_PIN,INPUT_PULLUP);

  Serial.println("XIAO-SYNTH BLE MIDI");
}

bool ble_connected = false;
void loop() {
  // Continuously poll all interfaces and route the traffic between them
  // MIDI_Interface::updateAll();
  midi_ble.updateAll();
  
  // Display the connection status using the built-in LED
  bool blestatus = midi_ble.isConnected();
  if(ble_connected != blestatus)
  {
    ble_connected = blestatus;
    digitalWrite(LED_PIN,  ble_connected);
    if(ble_connected){
      Serial.println("BLE connected");
    }
    else{
      Serial.println("BLE disconnected");
    }
  }
}
#endif 