/*
  ======================================================================
  AX CONTROL - FASE 1: LIGAÇÃO PURIFICADA DE CAMPO
  - Gás totalmente manual (passthrough sem timer)
  - Intertravamento de ventilação mantido
  - LEDs do painel frontal sincronizados com os relés
  ======================================================================
*/

#include <Arduino.h>

// --- MAPEAMENTO DE ENTRADAS ---
const int IN_REFRIG  = A0; // I1: Chave Refrigeração
const int IN_VENT    = A1; // I2: Chave Ventilação
const int IN_GAS     = A2; // I3: Chave Gás Etil (Sem Timer)
const int IN_UMIDIF  = A3; // I4: Chave Umidificador

// --- MAPEAMENTO DE SAÍDAS (Relés Físicos) ---
const int OUT_REFRIG = D0; // Vai para o R1 (Sitrad)
const int OUT_VENT   = D1; // Vai para o R2 (Ventilação)
const int OUT_GAS    = D2; // Vai para o R3 (Gás Etil)
const int OUT_UMIDIF = D3; // Vai para o R4 (Umidificador)

// --- MAPEAMENTO DE SAÍDAS (LEDs do Painel Frontal) ---
const int LED_REFRIG = LED_D0;
const int LED_VENT   = LED_D1;
const int LED_GAS    = LED_D2;
const int LED_UMIDIF = LED_D3;

// --- CONFIGURAÇÃO DE DEBOUNCE ANTI-RUÍDO ---
const int NUM_ENTRADAS = 4;
const int ENTRADAS[NUM_ENTRADAS] = {IN_REFRIG, IN_VENT, IN_GAS, IN_UMIDIF}; 

const unsigned long DEBOUNCE_DELAY = 50;
bool estadoEstavel[NUM_ENTRADAS] = {LOW, LOW, LOW, LOW};
bool ultimoLeitura[NUM_ENTRADAS] = {LOW, LOW, LOW, LOW};
unsigned long ultimoTempoDebounce[NUM_ENTRADAS] = {0, 0, 0, 0};

void setup() {
  Serial.begin(115200);

  // Configuração das Entradas
  for (int i = 0; i < NUM_ENTRADAS; i++) {
    pinMode(ENTRADAS[i], INPUT);
  }

  // Configuração dos Relés de Potência
  pinMode(OUT_REFRIG, OUTPUT);
  pinMode(OUT_VENT, OUTPUT);
  pinMode(OUT_GAS, OUTPUT);
  pinMode(OUT_UMIDIF, OUTPUT);

  // Configuração dos LEDs do Painel
  pinMode(LED_REFRIG, OUTPUT);
  pinMode(LED_VENT, OUTPUT);
  pinMode(LED_GAS, OUTPUT);
  pinMode(LED_UMIDIF, OUTPUT);

  // Garantia de Segurança: Força TUDO a iniciar desligado
  digitalWrite(OUT_REFRIG, LOW); digitalWrite(LED_REFRIG, LOW);
  digitalWrite(OUT_VENT, LOW);   digitalWrite(LED_VENT, LOW);
  digitalWrite(OUT_GAS, LOW);    digitalWrite(LED_GAS, LOW);
  digitalWrite(OUT_UMIDIF, LOW); digitalWrite(LED_UMIDIF, LOW);
}

void loop() {
  unsigned long agora = millis();

  // ========================================================================
  // 1. LEITURA FÍSICA DAS CHAVES (FILTRO DE RUÍDO EM LOOP)
  // ========================================================================
  for (int i = 0; i < NUM_ENTRADAS; i++) {
    bool leituraAtual = digitalRead(ENTRADAS[i]);

    if (leituraAtual != ultimoLeitura[i]) {
      ultimoTempoDebounce[i] = agora;
    }

    if ((agora - ultimoTempoDebounce[i]) > DEBOUNCE_DELAY) {
      estadoEstavel[i] = leituraAtual;
    }
    
    ultimoLeitura[i] = leituraAtual;
  }

  // Captura dos estados estabilizados
  bool chaveRefrig = estadoEstavel[0]; // I1 (A0)
  bool chaveVent   = estadoEstavel[1]; // I2 (A1)
  bool chaveGas    = estadoEstavel[2]; // I3 (A2)
  bool chaveUmidif = estadoEstavel[3]; // I4 (A3)

  // ========================================================================
  // 2. LÓGICA DE INTERTRAVAMENTO INDUSTRIAL
  // ========================================================================
  bool comandoRefrig = chaveRefrig;
  bool comandoVent   = chaveVent;
  bool comandoGas    = chaveGas;    
  bool comandoUmidif = chaveUmidif;

  // Regra 1: Refrigeração exige ventilação ligada
  if (comandoRefrig) {
    comandoVent = true; 
  }

  // Regra 2: Umidificador exige ventilação ligada
  if (comandoUmidif) {
    comandoVent = true;
  }

  // ========================================================================
  // 3. ACIONAMENTO DE POTÊNCIA (RELÉS FÍSICOS)
  // ========================================================================
  digitalWrite(OUT_REFRIG, comandoRefrig ? HIGH : LOW);
  digitalWrite(OUT_VENT,   comandoVent   ? HIGH : LOW);
  digitalWrite(OUT_GAS,    comandoGas    ? HIGH : LOW);
  digitalWrite(OUT_UMIDIF, comandoUmidif ? HIGH : LOW);

  // ========================================================================
  // 4. ACIONAMENTO VISUAL (LEDs DO PAINEL)
  // ========================================================================
  digitalWrite(LED_REFRIG, comandoRefrig ? HIGH : LOW);
  digitalWrite(LED_VENT,   comandoVent   ? HIGH : LOW);
  digitalWrite(LED_GAS,    comandoGas    ? HIGH : LOW);
  digitalWrite(LED_UMIDIF, comandoUmidif ? HIGH : LOW);
}