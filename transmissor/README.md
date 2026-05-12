# Código do transmissor

O ESP32 C3 Mini é usado como transmissor do rádio pistola, enviando o sinal dos potenciômetros  
do controle para o receptor, que é o ESP32 do robô.

O protocolo de comunicação é chamado de ESPNOW, para melhores informações, acessar o site Random Nerd. 
https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/ 

Cada ESP32 tem um endereço MAC Wi-Fi. O código é bem auto explicativo nessa parte, basta preencher 
o campo com o MAC do receptor, que os dados serão enviados, desde que o struct de ambos sejam os mesmos.

O código para descobrir o endereço MAC de um ESP32 se encontra nesse mesmo repositório. Ressaltando que encontrei dificuldades 
para fazer o ESP32 C3 Mini imprimir algo no Serial Monitor. Recomendo testes, experimentos e simulações para a verificação.

 
