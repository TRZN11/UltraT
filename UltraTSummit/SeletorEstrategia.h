/*
 * ============================================================
 *  SeletorEstrategia.h — Seleção de estratégia via IR
 *  Robô Sumô Autônomo  |  ESP32 WROOM
 * ============================================================
 *
 *  Permite escolher entre 6 estratégias de combate usando
 *  qualquer controle remoto de TV, antes do round começar.
 *
 *  Como usar no .ino:
 *    1. Inclua ANTES do ModuloStart.h
 *       #include "SeletorEstrategia.h"
 *       #include "ModuloStart.h"
 *
 *    2. No setup():
 *       seletorEstrategia.begin();
 *
 *    3. No loop(), dentro do bloco PREPARADO:
 *       if (moduloStart.preparado()) {
 *         seletorEstrategia.atualizar();  // muda estratégia via IR
 *       }
 *
 *    4. No loop(), dentro do bloco COMBATE:
 *       if (moduloStart.emCombate()) {
 *         switch (seletorEstrategia.estrategiaAtual()) {
 *           case ESTRATEGIA_1: iSeeYou();        break;
 *           case ESTRATEGIA_2: whiplash();        break;
 *           case ESTRATEGIA_3: Sharingan();       break;
 *           case ESTRATEGIA_4: SeekAndDestroy_L();break;
 *           case ESTRATEGIA_5: SeekAndDestroy_R();break;
 *           case ESTRATEGIA_6: paraTras();        break;
 *         }
 *       }
 *
 *  Biblioteca necessária:
 *    IRremoteESP8266 (by crankyoldgit)
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
#define IR_PROTOCOLO   DECODE_SONY
#define IR_BITS        12
// ─── Pinos ───────────────────────────────────────────────────
// IR_RECV_PIN já definido no ModuloStart.h (pino 15)
// Se usar este arquivo sozinho, descomente a linha abaixo:
// #define IR_RECV_PIN  15
// ─────────────────────────────────────────────────────────────

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
  //  Chame no setup() do .ino
  // ----------------------------------------------------------
  void begin() {
    // Compartilha o mesmo pino IR do ModuloStart (pino 15)
    // O IRrecv é criado aqui com buffer tolerante a colisões
    _irrecv = new IRrecv(IR_RECV_PIN);
    _irrecv->enableIRIn();

    _estrategia = ESTRATEGIA_1; // padrão: estratégia 1

    Serial.println("  [Seletor] pronto — estrategia padrao: 1");
    Serial.println("------------------------------------------");
  }

  // ----------------------------------------------------------
  //  Chame no loop() enquanto moduloStart.preparado() for true
  //  Verifica se chegou um novo comando IR e muda a estratégia
  // ----------------------------------------------------------
  void atualizar() {
    uint64_t codigo = _lerIR();
    if (codigo == 0) return; // nada recebido

    ESTRATEGIAS anterior = _estrategia;

    if      (codigo == IR_CMD_EST1) _estrategia = ESTRATEGIA_1;
    else if (codigo == IR_CMD_EST2) _estrategia = ESTRATEGIA_2;
    else if (codigo == IR_CMD_EST3) _estrategia = ESTRATEGIA_3;
    else if (codigo == IR_CMD_EST4) _estrategia = ESTRATEGIA_4;
    else if (codigo == IR_CMD_EST5) _estrategia = ESTRATEGIA_5;
    else if (codigo == IR_CMD_EST6) _estrategia = ESTRATEGIA_6;
    else return; // código desconhecido, ignora

    // Só loga se mudou
    if (_estrategia != anterior) {
      Serial.print("  [Seletor] estrategia selecionada: ");
      Serial.println(_nomeEstrategia(_estrategia));
    }
  }

  // ----------------------------------------------------------
  //  Retorna a estratégia atual — use no switch do loop()
  // ----------------------------------------------------------
  ESTRATEGIAS estrategiaAtual() { return _estrategia; }

  // ----------------------------------------------------------
  //  Retorna o número da estratégia (1–6) — útil para display
  // ----------------------------------------------------------
  int numeroAtual() { return (int)_estrategia + 1; }

  // ----------------------------------------------------------
  //  Retorna o nome da estratégia como texto — útil para debug
  // ----------------------------------------------------------
  const char* nomeAtual() { return _nomeEstrategia(_estrategia); }

  // ----------------------------------------------------------
  //  Reseta para a estratégia padrão (1)
  // ----------------------------------------------------------
  void resetar() {
    _estrategia = ESTRATEGIA_1;
    Serial.println("  [Seletor] resetado para estrategia 1");
  }

// ─────────────────────────────────────────────────────────────
private:
  IRrecv*        _irrecv    = nullptr;
  decode_results _irResult;
  ESTRATEGIAS    _estrategia = ESTRATEGIA_1;

  // ----------------------------------------------------------
  uint64_t _lerIR() {
    if (!_irrecv->decode(&_irResult)) return 0;

    uint64_t codigo = _irResult.value;
    _irrecv->resume();

    // Filtra repeat e inválidos
    if (codigo == 0xFFFFFFFFFFFFFFFFULL || codigo == 0) return 0;

    Serial.print("  [Seletor IR] 0x");
    serialPrintUint64(codigo, HEX);
    Serial.print(" (");
    Serial.print(typeToString(_irResult.decode_type, _irResult.repeat));
    Serial.println(")");

    return codigo;
  }

  // ----------------------------------------------------------
  const char* _nomeEstrategia(ESTRATEGIAS e) {
    switch (e) {
      case ESTRATEGIA_1: return "1 — iSeeYou (PID)";
      case ESTRATEGIA_2: return "2 — Whiplash";
      case ESTRATEGIA_3: return "3 — Sharingan";
      case ESTRATEGIA_4: return "4 — SeekAndDestroy Esquerda";
      case ESTRATEGIA_5: return "5 — SeekAndDestroy Direita";
      case ESTRATEGIA_6: return "6 — Para Tras";
      default:           return "Desconhecida";
    }
  }
};

// Instância global — pronta para usar no .ino
SeletorEstrategia seletorEstrategia;

#endif // SELETOR_ESTRATEGIA_H
