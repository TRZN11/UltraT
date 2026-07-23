#ifndef Estrategias_H
#define Estrategias_H

#include "PID.h"

void paraTras() { // estratégia número 6 no controle
  // Usa timers não-bloqueantes em vez de delay()
  // Move para frente por 500ms, depois para trás por 350ms, depois executa iSeeYou
  motor.move_for_then(1023, 1023, 500,
                      -1023, 1023, 350);
  iSeeYou();
}

// ============================================================
//  SeekAndDestroy — busca em semicírculo pela lateral da arena
//  (100% BLOQUEANTE: completa o semicírculo inteiro sem checar
//   sensores nem a parada de emergência, e só DEPOIS aciona o PID)
// ============================================================
//  CALIBRAÇÃO:
//   *_EXTERNO    = roda de fora do arco -> deixe sempre no máximo (1023)
//   *_INTERNO    = roda de dentro do arco -> ajusta a curvatura
//   *_DURACAO_MS = por quanto tempo o semicírculo roda antes de acionar o PID
// ============================================================

int      SND_L_EXTERNO    = 1023;
int      SND_L_INTERNO    = 300;   // CALIBRE AQUI — curvatura da busca ESQUERDA
uint32_t SND_L_DURACAO_MS = 800;   // CALIBRE AQUI — duração do semicírculo esquerdo (ms)

int      SND_R_EXTERNO    = 1023;
int      SND_R_INTERNO    = 300;   // CALIBRE AQUI — curvatura da busca DIREITA
uint32_t SND_R_DURACAO_MS = 800;   // CALIBRE AQUI — duração do semicírculo direito (ms)

bool _SND_L_feito = false;
bool _SND_R_feito = false;

// Chame no início de cada round (o .ino já faz isso sozinho) pra permitir
// que o semicírculo rode de novo na próxima vez que a estratégia for usada
void resetSeekAndDestroy() {
  _SND_L_feito = false;
  _SND_R_feito = false;
}

// Movimento 100% bloqueante — não sai por nada até o tempo acabar
void _semicirculoBloqueante(int vl, int vr, uint32_t duracao_ms) {
  motor.move(vl, vr);
  delay(duracao_ms);
}

void SeekAndDestroy_L(){ // estratégia número 4 no controle — busca pela lateral ESQUERDA
  if (!_SND_L_feito) {
    Serial.println("SeekAndDestroy_L: executando semicirculo (bloqueante)...");
    _semicirculoBloqueante(SND_L_INTERNO, SND_L_EXTERNO, SND_L_DURACAO_MS);
    _SND_L_feito = true;
    Serial.println("SeekAndDestroy_L: semicirculo concluido -> PID (iSeeYou)");
  }
  iSeeYou(); // depois do semicírculo, PID assume o resto do round
}

void SeekAndDestroy_R(){ // estratégia número 5 no controle — busca pela lateral DIREITA
  if (!_SND_R_feito) {
    Serial.println("SeekAndDestroy_R: executando semicirculo (bloqueante)...");
    _semicirculoBloqueante(SND_R_EXTERNO, SND_R_INTERNO, SND_R_DURACAO_MS);
    _SND_R_feito = true;
    Serial.println("SeekAndDestroy_R: semicirculo concluido -> PID (iSeeYou)");
  }
  iSeeYou();
}

#endif
