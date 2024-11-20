// Constantes e definições para LEDs e botões
const byte CHOICE_OFF   = 0; // LEDs desligados
const byte CHOICE_NONE  = 0; // Nenhum botão pressionado
const byte CHOICE_RED   = (1 << 0);
const byte CHOICE_GREEN = (1 << 1);
const byte CHOICE_BLUE  = (1 << 2);
const byte CHOICE_YELLOW = (1 << 3);

// Pinos dos LEDs
const byte LED_RED     = 3;
const byte LED_GREEN   = 5;
const byte LED_BLUE    = 13;
const byte LED_YELLOW  = 10;

// Pinos dos botões
const byte BUTTON_RED    = 2;
const byte BUTTON_GREEN  = 6;
const byte BUTTON_BLUE   = 12;
const byte BUTTON_YELLOW = 9;

// Parâmetros do jogo
const byte ROUNDS_TO_WIN      = 13;   // Número de rodadas para vencer o jogo.
const unsigned long ENTRY_TIME_LIMIT = 3000; // Tempo limite para pressionar um botão em milissegundos.

const byte MODE_MEMORY = 0;
const byte MODE_BATTLE = 1;

// Variáveis de estado do jogo
byte gameMode = MODE_MEMORY;    // Modo de jogo: memória por padrão.
byte gameBoard[32];             // Sequência de botões a ser reproduzida.
byte gameRound = 0;             // Rodada atual.

void setup() {
  // Configuração dos pinos como entrada/saída
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  gameMode = MODE_MEMORY; // Define o modo inicial
  play_winner();          // Indica o início do jogo com LEDs piscando.
}

void loop() {
  attractMode(); // Modo de espera com LEDs piscando enquanto aguarda um botão.

  // Indique o início do jogo
  setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Liga todos os LEDs
  delay(1000);
  setLEDs(CHOICE_OFF); // Desliga todos os LEDs
  delay(250);

  if (gameMode == MODE_MEMORY) {
    // Jogue o modo memória
    if (play_memory())
      play_winner(); // Vitória: LEDs piscam em comemoração.
    else
      play_loser();  // Derrota: LEDs piscam para indicar falha.
  } else if (gameMode == MODE_BATTLE) {
    play_battle();   // Jogue o modo batalha
    play_loser();    // Derrota: LEDs piscam para indicar falha.
  }
}

// Funções principais do jogo
boolean play_memory() {
  randomSeed(millis()); // Inicializa o gerador de números aleatórios
  gameRound = 0;        // Reinicia o jogo para a primeira rodada

  while (gameRound < ROUNDS_TO_WIN) {
    add_to_moves();  // Adiciona um botão à sequência
    playMoves();     // Mostra a sequência atual

    // Solicite ao jogador que repita a sequência
    for (byte currentMove = 0; currentMove < gameRound; currentMove++) {
      byte choice = wait_for_button(); // Aguarda o jogador pressionar um botão
      if (choice == CHOICE_NONE) return false;  // Tempo esgotado
      if (choice != gameBoard[currentMove]) return false; // Escolha incorreta
    }
    delay(1000); // Aguarda um segundo antes da próxima rodada
  }
  return true; // Vitória se todas as rodadas forem completadas
}

boolean play_battle() {
  gameRound = 0; // Reinicia o jogo para a primeira rodada

  while (true) {
    byte newButton = wait_for_button();       // Aguarda um botão ser pressionado
    gameBoard[gameRound++] = newButton;      // Adiciona o botão à sequência

    // Solicite ao jogador que repita a sequência
    for (byte currentMove = 0; currentMove < gameRound; currentMove++) {
      byte choice = wait_for_button();
      if (choice == CHOICE_NONE) return false; // Tempo esgotado
      if (choice != gameBoard[currentMove]) return false; // Escolha incorreta
    }
    delay(100); // Pausa entre jogadores
  }
}

void playMoves() {
  for (byte currentMove = 0; currentMove < gameRound; currentMove++) {
    setLEDs(gameBoard[currentMove]); // Liga o LED correspondente
    delay(300);                      // Tempo do LED ligado
    setLEDs(CHOICE_OFF);             // Desliga LEDs
    delay(150);                      // Pausa entre LEDs
  }
}

void add_to_moves() {
  byte newButton = random(0, 4); // Gera um número aleatório de 0 a 3

  if (newButton == 0) newButton = CHOICE_RED;
  else if (newButton == 1) newButton = CHOICE_GREEN;
  else if (newButton == 2) newButton = CHOICE_BLUE;
  else newButton = CHOICE_YELLOW;

  gameBoard[gameRound++] = newButton; // Adiciona à sequência do jogo
}

void setLEDs(byte leds) {
  digitalWrite(LED_RED, (leds & CHOICE_RED) ? HIGH : LOW);
  digitalWrite(LED_GREEN, (leds & CHOICE_GREEN) ? HIGH : LOW);
  digitalWrite(LED_BLUE, (leds & CHOICE_BLUE) ? HIGH : LOW);
  digitalWrite(LED_YELLOW, (leds & CHOICE_YELLOW) ? HIGH : LOW);
}

byte wait_for_button() {
  long startTime = millis(); // Marca o início do tempo de espera

  while ((millis() - startTime) < ENTRY_TIME_LIMIT) {
    byte button = checkButton();
    if (button != CHOICE_NONE) {
      while (checkButton() != CHOICE_NONE); // Aguarda liberação do botão
      delay(10); // Evita leituras múltiplas
      return button;
    }
  }
  return CHOICE_NONE; // Retorna se o tempo se esgotar
}

byte checkButton() {
  if (digitalRead(BUTTON_RED) == LOW) return CHOICE_RED;
  else if (digitalRead(BUTTON_GREEN) == LOW) return CHOICE_GREEN;
  else if (digitalRead(BUTTON_BLUE) == LOW) return CHOICE_BLUE;
  else if (digitalRead(BUTTON_YELLOW) == LOW) return CHOICE_YELLOW;
  return CHOICE_NONE; // Nenhum botão pressionado
}

void play_winner() {
  for (int i = 0; i < 3; i++) {
    setLEDs(CHOICE_RED | CHOICE_GREEN);
    delay(300);
    setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
    delay(300);
  }
}

void play_loser() {
  for (int i = 0; i < 3; i++) {
    setLEDs(CHOICE_RED | CHOICE_BLUE);
    delay(300);
    setLEDs(CHOICE_GREEN | CHOICE_YELLOW);
    delay(300);
  }
}

void attractMode() {
  while (true) {
    setLEDs(CHOICE_RED);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;

    setLEDs(CHOICE_BLUE);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;

    setLEDs(CHOICE_GREEN);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;

    setLEDs(CHOICE_YELLOW);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
  }
}
