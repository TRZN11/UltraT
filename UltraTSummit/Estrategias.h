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
// ============================================================
//  Enquanto os sensores frontais não veem o oponente, o robô anda
//  para frente em velocidade máxima só que fazendo uma curva (arco),
//  percorrendo a lateral esquerda (ou direita) da arena — como um
//  semicírculo. Assim que qualquer sensor frontal detecta o oponente,
//  o robô larga a curva e entrega o controle para o PID (iSeeYou),
//  que faz o alinhamento fino e o ataque.
//
//  CALIBRAÇÃO DO RAIO DA CURVA (é aqui que você ajusta o robô):
//   *_EXTERNO = roda de FORA do arco -> deixe sempre no máximo (1023)
//   *_INTERNO = roda de DENTRO do arco -> ajuste este valor:
//       perto de 1023 (igual ao externo)  -> quase reto, curva bem aberta
//       valores baixos (ex.: 200 a 400)   -> curva mais fechada
//       0                                  -> gira apoiado numa roda só
//       negativo (ex.: -300)               -> curva bem fechada, quase um giro no próprio eixo
// ============================================================

int SND_L_EXTERNO = 1023;  // roda direita (de fora) na busca ESQUERDA
int SND_L_INTERNO = 300;   // roda esquerda (de dentro) na busca ESQUERDA — CALIBRE AQUI

int SND_R_EXTERNO = 1023;  // roda esquerda (de fora) na busca DIREITA
int SND_R_INTERNO = 300;   // roda direita (de dentro) na busca DIREITA — CALIBRE AQUI

void SeekAndDestroy_L(){ // estratégia número 4 no controle — busca pela lateral ESQUERDA
  leituraSensoresSD();
  bool viuOponente = leitura[1] || leitura[2]; // algum sensor frontal viu o oponente

  if (viuOponente) {
    Serial.println("SeekAndDestroy_L: oponente na frente -> PID (iSeeYou)");
    iSeeYou(); // solta o arco e deixa o PID alinhar/atacar
  } else {
    Serial.println("SeekAndDestroy_L: buscando (arco pela lateral esquerda)");
    motor.move(SND_L_INTERNO, SND_L_EXTERNO); // esquerda mais lenta -> curva pra esquerda
  }
}

void SeekAndDestroy_R(){ // estratégia número 5 no controle — busca pela lateral DIREITA
  leituraSensoresSD();
  bool viuOponente = leitura[1] || leitura[2];

  if (viuOponente) {
    Serial.println("SeekAndDestroy_R: oponente na frente -> PID (iSeeYou)");
    iSeeYou();
  } else {
    Serial.println("SeekAndDestroy_R: buscando (arco pela lateral direita)");
    motor.move(SND_R_EXTERNO, SND_R_INTERNO); // direita mais lenta -> curva pra direita
  }
}

#endif
