/*
  16/11/2025, para a summit;
  Auto e RC usando ESPNOW;
  Alimentar o ESP32 C3 Mini do rádio pistola com um power bank ou um celular, as pilhas podem não dar conta,
  afetando a direção do robô;

  https://github.com/eltonsrgit/UltraTSummit/
*/
#include "SumoIR.h"
#include "DRV8833.h"
#include "PID.h"
#include "Whiplash.h"
#include "Estrategias.h"
#include "Sharingan.h"
#include "LEDFX.h"

// começa no modo em que está "true"
// trocar se não funcionar

#define boot 0 

int strategy = 0;

SumoIR IR;
// protótipo da callback (declarado antes do setup para que o registro funcione)

void setup() {
  Serial.begin(115200);

  IR.begin(15); // sensor conectado no pino 15 (não mudar)
  
  setupSensores();

  pinMode(boot, INPUT_PULLUP);

}
int gatilhoNorm = 0; // -255 até 255
int rodaNorm = 0;    // -127 até 127

void loop() {
    IR.update();
 
    if (IR.prepare()) { // número 1 no controle
      pixels.clear();
      ledLight(255, 255, 255);
      motor.stop();
      Serial.println("-> sumo prepare"); // não retirar essa linha (aparentemente dá erro para iniciar com o IR
    }
    
    else if (IR.start()) {
      Serial.println("-> sumo start"); // não retirar essa linha (aparentemente dá erro para iniciar com o IR
    } 
    
    else if (IR.on()) { // número 2 no controle
      pixels.clear();
      ledLight(0, 255, 0);
      
      switch (strategy) {
        default: //fallthrough
        case 4:
          iSeeYou();
        break;

        case 5:
          paraTras();
        break;

        case 6:
          SeekAndDestroy_L();
        break;

        case 7:
          SeekAndDestroy_R();
        break; 

        case 8:
          whiplash();
        break;
      }
      Serial.println("-> sumo on"); // não retirar essa linha (aparentemente dá erro para iniciar com o IR
    }
    
    else if (IR.stop()) { // número 3 no controle
      pixels.clear();
      motor.stop();
      Serial.println("-> sumo stop"); // não retirar essa linha (aparentemente dá erro para iniciar com o IR
    }
    
    else { // robô inicia caindo aqui
      pixels.clear();
      strategySelection();     // seletor de estratégias
      ledDetection();
      Serial.println("-> sumo off"); // não retirar essa linha (aparentemente dá erro para iniciar com o IR)
    } 
}
void strategySelection() {
  int cmd = IR.read();
  if (cmd >= 4 && cmd <= 9) { 
    strategy = cmd;
  } else return;

  if (cmd <= 8) {
    const int num_leds = cmd % 8;
    for(uint8_t i = 0; i < num_leds; i++) {
      switch ((cmd-3) % 6) { 
        case 0: pixels.setPixelColor(i, pixels.Color(255, 50,  50  )); break; // Vermelho claro
        case 1: pixels.setPixelColor(i, pixels.Color(0,   255, 100 )); break; // Verde com toque de azul
        case 2: pixels.setPixelColor(i, pixels.Color(255, 0,   180 )); break; // Magenta
        case 3: pixels.setPixelColor(i, pixels.Color(255, 140, 0   )); break; // Laranja
        case 4: pixels.setPixelColor(i, pixels.Color(100, 200, 255 )); break; // Azul claro
        case 5: pixels.setPixelColor(i, pixels.Color(180, 255, 0   )); break; // Verde-amarelado
      } pixels.show();
    }
    delay(80);
    for(uint8_t i = 0; i < num_leds; i++) { 
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Desliga os LEDs
      pixels.show();
    }
    delay(80);
  }
}
