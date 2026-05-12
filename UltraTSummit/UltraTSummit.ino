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
#include "Empate.h"
#include "Sharingan.h"
#include "LEDFX.h"
#include <esp_now.h>
#include <WiFi.h>

// começa no modo em que está "true"
// trocar se não funcionar
bool modoAutonomo = true;  
bool RC = false;

#define boot 0 

int strategy = 0;

SumoIR IR;

unsigned long tempoPressionado = 0;  // armazena o tempo que o botão foi pressionado
bool botaoPressionado = false;       // indica se o botão foi pressionado

// protótipo da callback (declarado antes do setup para que o registro funcione)
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  IR.begin(15); // sensor conectado no pino 15 (não mudar)
  
  motor.begin();
  motor.bip(5, 250, 2500); // motor bipa (x vezes, intervalo (ms), frequencia em Hz)

  setupSensores();

  pinMode(boot, INPUT_PULLUP);

  ////////////////////////////////// para o LED ////////////////////////////////////
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  
  pixels.begin();
  pixels.setBrightness(50);
  pixels.clear();
  ledLight(0, 0, 0);
  //////////////////////////////////////////////////////////////////////////////////
  
  Serial.println("Sistema iniciado no modo RC (ESP-NOW)");
  pixels.clear();
  ledLight(0, 150, 0); // LED verde indicando modo RC
}

typedef struct rc {
  int gatilho;
  int roda;
} rc;

rc pack_rc;

// Variáveis normalizadas
int gatilhoNorm = 0; // -255 até 255
int rodaNorm = 0;    // -127 até 127

void loop() {
  // Verifica se o botão foi pressionado
  if (digitalRead(boot) == LOW && !botaoPressionado) {
    tempoPressionado = millis();  // Armazena o tempo atual
    botaoPressionado = true;      // Marca o botão como pressionado
  }

  // Verifica se o botão foi pressionado por mais de 1000ms (1 segundo)
  if (botaoPressionado && (millis() - tempoPressionado >= 1000)) {
    modoAutonomo = !modoAutonomo;  // Alterna o modo
    RC = !modoAutonomo;
    
    Serial.print("Modo alternado para: ");
    Serial.println(modoAutonomo ? "Autônomo." : "RC.");
    
    // Efeito visual indicando mudança de modo
    pixels.clear();
    if (modoAutonomo) {
      ledLight(0, 0, 150); // Azul para modo autônomo
    } else {
      ledLight(0, 150, 0); // Verde para modo RC
    }

    // Para os motores ao alternar modos
    motor.stop();
    
    // Reseta a variável do botão pressionado
    botaoPressionado = false;
  }

  // Libera o botão se soltou antes de 1 segundo
  if (botaoPressionado && digitalRead(boot) == HIGH && (millis() - tempoPressionado < 1000)) {
    botaoPressionado = false;
  }

  if (modoAutonomo) {
    // Código autônomo
    RC = false;
  
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
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  if (modoAutonomo) return;

  memcpy(&pack_rc, incomingData, sizeof(pack_rc));

  pixels.clear();
  ledLight(0, 150, 0);

  // Normalização do gatilho
  if (pack_rc.gatilho < 1800) {
    gatilhoNorm = map(pack_rc.gatilho, 820, 1800, 255, 0);
  } else if (pack_rc.gatilho > 2300) {
    gatilhoNorm = map(pack_rc.gatilho, 2300, 2970, 0, -255);
  } else {
    gatilhoNorm = 0;
  }

  // Normalização da roda
  if (pack_rc.roda < 1800) {
    rodaNorm = map(pack_rc.roda, 150, 1800, 127, 0);
  } else if (pack_rc.roda > 2300) {
    rodaNorm = map(pack_rc.roda, 2300, 4050, 0, -127);
  } else {
    rodaNorm = 0;
  }

  Serial.print("Bruto -> G: "); Serial.print(pack_rc.gatilho);
  Serial.print(" R: "); Serial.print(pack_rc.roda);
  Serial.print(" | Normalizado -> G: "); Serial.print(gatilhoNorm);
  Serial.print(" R: "); Serial.println(rodaNorm);

  int y = map(gatilhoNorm, -280, 180, -1023, 1023);
  int z = map(rodaNorm, -130, 120, -1023, 1023);

  int motorEsq = constrain(-z + y, -1023, 1023);
  int motorDir = constrain(z + y, -1023, 1023);


  const int deadzone = 200; // acho que de 200 à 500 é bom

  if (abs(motorEsq) < deadzone && abs(motorDir) < deadzone) {
    motor.stop();  
    Serial.println("MOTORES PARADOS");
  } else {
    motor.move(motorEsq, motorDir);
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
