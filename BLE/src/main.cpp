#include <Arduino.h>
#include "soc/soc_caps.h"
#include <SAM2695_MIDI_Mapper.h>
#include <MIDI_Interfaces/BluetoothMIDI_Interface.hpp>
#include <MIDI_Interfaces/SerialMIDI_Interface.hpp>

#define DEVICE_NAME "XIAO-SYNTH"

#ifndef SOC_BLE_SUPPORTED	
#error This ESP32 SoC has no BLE 
#else

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
    #define LED_PIN 10              
#else
    #define LED_PIN LED_BUILTIN
#endif

BluetoothMIDI_Interface bleMIDI;
SerialMIDI_Interface sam_synth = {Serial0, 31250}; 

SAM2695_MIDI_Mapper mapper(sam_synth);

void setup() {
  sam_synth.begin(); 
  
  bleMIDI.setName(DEVICE_NAME);
  bleMIDI.begin();

  mapper.begin();
  bleMIDI.setCallbacks(mapper);

}

void loop() {
  bleMIDI.update();
  sam_synth.update();
}

#endif 