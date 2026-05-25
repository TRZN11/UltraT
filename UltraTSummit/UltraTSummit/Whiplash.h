#ifndef Whiplash_H
#define Whiplash_H

#include "PID.h"

bool achou = false;
void whiplash() { 
  // andar para frente e, no momento em que um dos sensores laterais detectarem, acionar e travar em iSeeYou()
  leituraSensores();
  if (leitura[1] || leitura[2]) {
    achou = true;
    motor.stop();
  }
  
  if (achou) {
    iSeeYou();
  } else {
    motor.move(1023, 1023);
  }
}

#endif
