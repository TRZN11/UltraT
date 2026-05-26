/*
 * ============================================================
 *  SeletorEstrategia.h — Seleção de estratégia via IR
 *  Robô Sumô Autônomo  |  ESP32 WROOM
 * ============================================================
 *
 *  NÃO tem receptor IR próprio — usa o do ModuloStart.
 *  NÃO precisa de begin() nem de nenhuma inicialização.
 *
 *  Como usar no .ino:
 *
 *    // No loop(), estado PREPARADO:
 *    if (moduloStart.preparado()) {
 *      seletorEstrategia.atualizar(moduloStart.ultimoResultado());
 *      Serial.println(seletorEstrategia.nomeAtual());
 *    }
 *
 *    // No loop(), estado COMBATE:
 *    if (moduloStart.emCombate()) {
 *      switch (seletorEstrategia.estrategiaAtual()) {
 *        case ESTRATEGIA_1: iSeeYou();          break;
 *        case ESTRATEGIA_2: whiplash();         break;
 *        case ESTRATEGIA_3: Sharingan();        break;
 *        case ESTRATEGIA_4: SeekAndDestroy_L(); break;
 *        case ESTRATEGIA_5: SeekAndDestroy_R(); break;
 *        case ESTRATEGIA_6: paraTras();         break;
 *      }
 *    }
 * ============================================================
 */

#ifndef SELETOR_ESTRATEGIA_H
#define SELETOR_ESTRATEGIA_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// ╔══════════════════════════════════════════════════════════╗
// ║          >>>  CONFIGURE OS BOTÕES AQUI  <<<              ║
// ╠══════════════════════════════════════════════════════════╣
#define IR_CMD_EST1   0xC10UL   // Estratégia 1
#define IR_CMD_EST2   0x210UL   // Estratégia 2
#define IR_CMD_EST3   0xA10UL   // Estratégia 3
#define IR_CMD_EST4   0x610UL   // Estratégia 4
#define IR_CMD_EST5   0xE10UL   // Estratégia 5
#define IR_CMD_EST6   0x110UL   // Estratégia 6
// ╚══════════════════════════════════════════════════════════╝

// ─── Enum das estratégias ────────────────────────────────────
enum ESTRATEGIAS {
  ESTRATEGIA_1 = 0,
  ESTRATEGIA_2,
  ESTRATEGIA_3,
  ESTRATEGIA_4,
  ESTRATEGIA_5,
  ESTRATEGIA_6
};
// ─────────────────────────────────────────────────────────────

class SeletorEstrategia {
public:

  // ----------------------------------------------------------
  //  Chame no loop() enquanto moduloStart.preparado() for true
  //  Recebe o ponteiro do último resultado IR do ModuloStart
  // ----------------------------------------------------------
  void atualizar(decode_results* resultado) {
    if (!resultado) return;  // nenhum sinal novo, sai

    uint64_t codigo = resultado->value;
    if (codigo == 0xFFFFFFFFFFFFFFFFULL || codigo == 0) return;

    ESTRATEGIAS anterior = _estrategia;

    if      (codigo == IR_CMD_EST1) _estrategia = ESTRATEGIA_1;
    else if (codigo == IR_CMD_EST2) _estrategia = ESTRATEGIA_2;
    else if (codigo == IR_CMD_EST3) _estrategia = ESTRATEGIA_3;
    else if (codigo == IR_CMD_EST4) _estrategia = ESTRATEGIA_4;
    else if (codigo == IR_CMD_EST5) _estrategia = ESTRATEGIA_5;
    else if (codigo == IR_CMD_EST6) _estrategia = ESTRATEGIA_6;
    else return; // código não é de estratégia, ignora

    if (_estrategia != anterior) {
      Serial.print("  [Seletor] estrategia: ");
      Serial.println(_nomeEstrategia(_estrategia));
    }
  }

  // ----------------------------------------------------------
  ESTRATEGIAS estrategiaAtual() { return _estrategia;                    }
  int         numeroAtual()     { return (int)_estrategia + 1;           }
  const char* nomeAtual()       { return _nomeEstrategia(_estrategia);   }

  void resetar() {
    _estrategia = ESTRATEGIA_1;
    Serial.println("  [Seletor] resetado para estrategia 1");
  }

// ─────────────────────────────────────────────────────────────
private:
  ESTRATEGIAS _estrategia = ESTRATEGIA_1;

  const char* _nomeEstrategia(ESTRATEGIAS e) {
    switch (e) {
      case ESTRATEGIA_1: return "1 - iSeeYou (PID)";
      case ESTRATEGIA_2: return "2 - Whiplash";
      case ESTRATEGIA_3: return "3 - Sharingan";
      case ESTRATEGIA_4: return "4 - SeekAndDestroy Esquerda";
      case ESTRATEGIA_5: return "5 - SeekAndDestroy Direita";
      case ESTRATEGIA_6: return "6 - Para Tras";
      default:           return "Desconhecida";
    }
  }
};

// Instância global — pronta para usar no .ino
SeletorEstrategia seletorEstrategia;

#endif // SELETOR_ESTRATEGIA_H
