#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pinos do LED RGB
#define pinLedRed 12
#define pinLedGreen 8
#define pinLedBlue 13


#define saiSom 3
#define botaoEsq 6
#define botaoDir 7


// Endereço I2C do display
#define I2C_ADDR 0x27

LiquidCrystal_I2C disp(I2C_ADDR, 16, 2);

// Variáveis globais para a posição do jogador e dos obstáculos, a pontuação e o atraso do jogo.
int posicaoJogador = 0;                // Posição do jogador no eixo Y (linha do display)
int posicaoObstaculo1[2] = { 0, 15 };  // Posição do primeiro obstáculo (linha e coluna)
int posicaoObstaculo2[2] = { 1, 12 };  // Posição do segundo obstáculo (linha e coluna)
int tempoAtraso = 100;                 // Tempo de atraso entre as movimentações
int pontos = 0;                        // Pontuação do jogador

void semaforoDeLargada() {

  // Desliga todos os LEDs
  digitalWrite(pinLedRed, HIGH);
  digitalWrite(pinLedGreen, HIGH);
  digitalWrite(pinLedBlue, HIGH);

  tone(saiSom, 2000, 50);
  // Semaforo Vermelho
  digitalWrite(pinLedRed, LOW);
  digitalWrite(pinLedGreen, HIGH);
  digitalWrite(pinLedBlue, HIGH);
  tone(saiSom, 2000, 30);         // Toca um som de partida (2000Hz) por 1 segundo
  delay(1000);                    // Espera 2 segundos

  // Semaforo Amarelo
  digitalWrite(pinLedRed, LOW);
  digitalWrite(pinLedGreen, LOW);
  digitalWrite(pinLedBlue, HIGH);
  tone(saiSom, 2000, 30);         // Toca um som de partida (2000Hz) por 1 segundo

  delay(1000);                    // Espera 1 segundo

  // Semaforo Verde
  digitalWrite(pinLedRed, HIGH);
  digitalWrite(pinLedGreen, LOW);
  digitalWrite(pinLedBlue, HIGH);
  tone(saiSom, 2000, 30);         // Toca um som de partida (2000Hz) por 1 segundo

  delay(1000);                    // Espera 1 segundo
  tone(saiSom, 2500, 100);        // Toca um som de partida (2000Hz) por 1 segundo

  // Desliga todos os LEDs
  digitalWrite(pinLedRed, HIGH);
  digitalWrite(pinLedGreen, HIGH);
  digitalWrite(pinLedBlue, HIGH);
}

// Função setup: Inicializa o display e configura os pinos do Arduino.
void setup() {
  
  // Configura os pinos do LED RGB como saída
  pinMode(pinLedRed, OUTPUT);
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedBlue, OUTPUT);

  // Configuração inicial do display LCD com interface I2C
  disp.init(); 
  disp.setBacklight(1);             // Inicializa o display
  pinMode(botaoDir, INPUT_PULLUP);  // Configura o pino 7 como entrada com pull-up interno (botão direito)
  pinMode(botaoEsq, INPUT_PULLUP);  // Configura o pino 6 como entrada com pull-up interno (botão esquerdo)
  pinMode(saiSom, OUTPUT);          // Configura o pino 9 como saída (para o som)

  randomSeed(analogRead(A0));       // Inicializa o gerador de números aleatórios com uma leitura analógica

  byte car[8] = {                   // Cria o símbolo do jogador no display
                  B00000,
                  B00000,
                  B10001,
                  B11111,
                  B11111,
                  B10001,
                  B00000,
                  B00000
  };
  disp.createChar(0, car);          // Armazena o símbolo do jogador no LCD

  byte obstaculo[8] = {             // Cria o símbolo do obstáculo no display
                        B00000,
                        B11111,
                        B10101,
                        B11011,
                        B11011,
                        B10101,
                        B11111,
                        B00000
  };
  disp.createChar(1, obstaculo);      // Armazena o símbolo do obstáculo no LCD

  semaforoDeLargada();                // Chama a função do semáforo de largada antes do loop principal
}

// Função atualizaDisplay: Atualiza o display LCD com as posições do jogador e dos obstáculos.
void atualizaDisplay() {
  disp.clear();                                                // Limpa o display
  disp.setCursor(3, posicaoJogador);                           // Define a posição do cursor e escreve o símbolo do jogador
  disp.write(byte(0));                                         // Escreve o símbolo do jogador
  disp.setCursor(posicaoObstaculo1[1], posicaoObstaculo1[0]);  // Define a posição do cursor para o primeiro obstáculo
  disp.write(byte(1));                                         // Escreve o símbolo do primeiro obstáculo
  disp.setCursor(posicaoObstaculo2[1], posicaoObstaculo2[0]);  // Define a posição do cursor para o segundo obstáculo
  disp.write(byte(1));                                         // Escreve o símbolo do segundo obstáculo
  delay(tempoAtraso);                                          // Atraso para atualização do display
}

// Função posicionaObstaculo: Move os obstáculos para a esquerda e reposiciona-os quando saem da tela.
void posicionaObstaculo() {
  if (posicaoObstaculo1[1] < 0) {         // Se o primeiro obstáculo saiu da tela (esquerda)
    posicaoObstaculo1[1] = 15;            // Reposiciona o primeiro obstáculo na coluna inicial
    posicaoObstaculo1[0] = random(0, 2);  // Define aleatoriamente a linha do primeiro obstáculo
  } else {
    posicaoObstaculo1[1]--;               // Move o primeiro obstáculo para a esquerda
  }

  if (posicaoObstaculo2[1] < 0) {         // Se o segundo obstáculo saiu da tela (esquerda)
    posicaoObstaculo2[1] = 15;            // Reposiciona o segundo obstáculo na coluna inicial
    posicaoObstaculo2[0] = random(0, 2);  // Define aleatoriamente a linha do segundo obstáculo
  } else {
    posicaoObstaculo2[1]--;               // Move o segundo obstáculo para a esquerda
  }

  if (posicaoObstaculo1[1] == 2 || posicaoObstaculo2[1] == 2) {  // Se algum obstáculo está na coluna 2
    pontos++;                                                    // Aumenta a pontuação
    tempoAtraso--;                          // Diminui o atraso conforme a pontuação aumenta
  }
  delay(tempoAtraso);                                            // Atraso para movimentação dos obstáculos
}

// Função colisao: Verifica se o jogador colidiu com algum obstáculo e exibe mensagem de perda.
void colisao() {
  if ((posicaoObstaculo1[1] == 3 && posicaoObstaculo1[0] == posicaoJogador)           // verifica se o primeiro obstáculo está na mesma coluna e na mesma linha  que o jogador
      || (posicaoObstaculo2[1] == 3 && posicaoObstaculo2[0] == posicaoJogador)) {     // faz o mesmo para o segundo obstáculo.

    disp.clear();                       // Limpa o display LCD para preparar a exibição da animação de perda.
    
    for (int x = 0; x < 16; x++) { 
      tone(saiSom, 1000, 30);           // Toca um som agudo (1000Hz) por 30 milissegundos. 
      disp.print("8");                  // Imprime o caractere "8" no display LCD.    
      delay(30);                        // Atraso de 30 milissegundos entre cada impressão do caractere "8".
    }  
    disp.setCursor(0, 1);               // Define a posição do cursor para escrever na segunda linha do display LCD. 

    for (int x = 0; x < 16; x++) {
      tone(saiSom, 800, 30);            // Toca um som grave (800Hz) por 30 milissegundos.  
      disp.print("8");                  // Imprime o caractere "8" no display LCD. 
      delay(30);                        // Atraso de 30 milissegundos entre cada impressão do caractere "8".
    }
    
    while (true) {                      // Loop infinito que será executado após a animação de perda. 
      disp.clear();                     // Limpa o display LCD. 
      disp.print("Perdeu!");            // Imprime a mensagem "Perdeu!" no display LCD. 
      disp.setCursor(0, 1);             // Define a posição do cursor para escrever na segunda linha do display LCD. 
      disp.print("Pontos: ");           // Imprime a mensagem "Pontos: " no display LCD. 
      disp.print(pontos);               // Imprime a pontuação atual (armazenada na variável 'pontos') no display LCD.      
      digitalWrite(pinLedRed, LOW);    // Liga o LED vermelho, acendendo-o com intensidade máxima. 
      digitalWrite(pinLedGreen, HIGH);   // Desliga o LED verde. 
      digitalWrite(pinLedBlue, HIGH);    // Desliga o LED azul.       
      delay(500);                       // Atraso de meio segundo. 
      digitalWrite(pinLedRed, HIGH);     // Desliga o LED vermelho. 
      digitalWrite(pinLedGreen, HIGH);   // Desliga o LED verde. 
      digitalWrite(pinLedBlue, HIGH);    // Desliga o LED azul.      
      delay(500);                       // Atraso de meio segundo.
      
      // Se o botão esquerdo e direito forem pressionados
      if ((!digitalRead(botaoDir)) && (!digitalRead(botaoEsq))) {  
        reiniciarJogo();      //chama função pra reiniciar a partida
        break;                //saí do loop
      }

    }
  }
}

// Função reiniciarJogo: Reinicia o ciclo do jogo
void reiniciarJogo() {
  posicaoJogador = 0;       // Reinicia a posição do jogador
  tempoAtraso = 100;        // Reinicia o tempo de atraso entre movimentações
  pontos = 0;               // Reinicia os pontos
  semaforoDeLargada();      //chama a função da largada novamente          
}

// Função loop: Controla o ciclo principal do jogo, lendo a entrada dos botões, movendo os obstáculos, verificando colisões e atualizando o display.
void loop() {
  for (int x = 0; x < 2; x++) {           // Leitura dos botões para mover o jogador
    if (x == 0) {
      if (!digitalRead(botaoDir)) {       // Se o botão direito foi pressionado
        posicaoJogador = 1;               // Muda a posição do jogador para a linha 1
      }
      if (!digitalRead(botaoEsq)) {       // Se o botão esquerdo foi pressionado
        posicaoJogador = 0;               // Muda a posição do jogador para a linha 0
      }
    }
  }
  
  posicionaObstaculo();  // Move os obstáculos
  colisao();             // Verifica colisões
  atualizaDisplay();     // Renderiza o display
}