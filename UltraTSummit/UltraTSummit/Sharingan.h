#ifndef Sharingan_H
#define Sharingan_H

// testar ainda

#include "Sensores.h"
#include "PID.h"
#include "Whiplash.h"



int sorteio = 0; 
int tempoGiro = 150; // em millis
bool escolha = false; // indica se já sorteou

void Sharingan(){
  // faz o sorteio apenas uma vez
  if (!escolha) {
    sorteio = random(1, 3); // sorteio de 1 à 2
    escolha = true;         // marca que já foi sorteado
  }

  // executa sempre o mesmo caso sorteado
  switch (sorteio){
    case 1:
      motor.move(-900, 900);
      delay(tempoGiro);
      whiplash();
      break;
    case 2:
      motor.move(900, -900);
      delay(tempoGiro);
      whiplash();
      break;
  }
}

#endif
