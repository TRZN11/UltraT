/*
  16/11/2025, para a summit;
  Auto e RC usando ESPNOW;
  Alimentar o ESP32 C3 Mini do rádio pistola com um power bank ou um celular, as pilhas podem não dar conta,
  afetando a direção do robô;

  https://github.com/TRZN11/UltraT/blob/main/UltraTSummit
*/

#include "DRV8833.h"
#include "PID.h"
#include "Whiplash.h"
#include "Estrategias.h"
#include "Sharingan.h"
#include "LEDFX.h"
#include "ModuloStart.h"
#include "SeletorEstrategia.h"

#define boot 0

void setup() {
  Serial.begin(115200);
  motor.begin();
  moduloStart.begin();   // sensor IR no pino 15 (não mudar)
  setupSensores();
  pinMode(boot, INPUT_PULLUP);
  pixels.begin();
}

void loop() {
  moduloStart.atualizar();
  motor.update();  // Processa fila de movimentos com timers

  if (moduloStart.preparado()) {
    // passa o resultado IR do ModuloStart para o seletor
    seletorEstrategia.atualizar(moduloStart.ultimoResultado());
    Serial.println(seletorEstrategia.nomeAtual());
  }

  // ── COMBATE: executa a estratégia escolhida ───────────────
  else if (moduloStart.emCombate()) {
    pixels.clear();
    ledLight(0, 255, 0); // LED verde = combate ativo

    switch (seletorEstrategia.estrategiaAtual()) {
      case ESTRATEGIA_1: iSeeYou();          break;
      case ESTRATEGIA_2: whiplash();         break;
      case ESTRATEGIA_3: Sharingan();        break;
      case ESTRATEGIA_4: SeekAndDestroy_L(); break;
      case ESTRATEGIA_5: SeekAndDestroy_R(); break;
      case ESTRATEGIA_6: paraTras();         break;
    }
  }

    
    else if (moduloStart.preparado()) {
      Serial.println("(PREPARAR recebido novamente — ja preparado)"); // não retirar essa linha (aparentemente dá erro para iniciar com o IR
    } 
    
    else if (moduloStart.emCombate()) { // número 2 no controle
      pixels.clear();
      ledLight(0, 125, 0);
     Serial.println(seletorEstrategia.nomeAtual());
    }
    else if (moduloStart.parado()) { // número 3 no controle
      motor.clear_moving();  // Limpa fila de movimentos pendentes
      motor.stop();
      pixels.clear();
      Serial.println("-> sumo stop");
    }
  }
}
void LED_Estrategias() {

  decode_results* resultado = moduloStart.ultimoResultado();
  if (!resultado) return;
  uint64_t cmd = resultado->value;

  if (cmd >= 4 && cmd <= 9) { 
    strategy = cmd;
  } else return;

  if (cmd <= 8) {
    const int num_leds = cmd % 8;
    for(uint8_t i = 0; i < num_leds; i++) {
      switch ((cmd-3) % 6) { 
        case ESTRATEGIA_1: pixels.setPixelColor(i, pixels.Color(255, 50,  50  )); break; // Vermelho claro
        case ESTRATEGIA_2: pixels.setPixelColor(i, pixels.Color(0,   255, 100 )); break; // Verde com toque de azul
        case ESTRATEGIA_3: pixels.setPixelColor(i, pixels.Color(255, 0,   180 )); break; // Magenta
        case ESTRATEGIA_4: pixels.setPixelColor(i, pixels.Color(255, 140, 0   )); break; // Laranja
        case ESTRATEGIA_5: pixels.setPixelColor(i, pixels.Color(100, 200, 255 )); break; // Azul claro
        case ESTRATEGIA_6: pixels.setPixelColor(i, pixels.Color(180, 255, 0   )); break; // Verde-amarelado
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
