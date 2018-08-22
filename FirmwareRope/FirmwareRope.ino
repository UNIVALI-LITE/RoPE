#include <EEPROM.h>
#include "libs/Button/Button.cpp"
#include "libs/RoPE_Steppers_28BYJ48/RoPE_Steppers_28BYJ48.cpp"


#define QUANTIDADE_MAXIMA_ACOES 45
bool testing_loop = 0;
bool sound_off = 0;
bool calibrando = 0;
int easter_egg;
int TURN_STEP_OVERWRITE = 170;
int WALK_STEP_OVERWRITE = 360;
int steps_frente = 0;
int steps_tras = 0;
int steps_direita = 0;
int steps_esquerda = 0;

int posicaoMemEsquerda = 0;
int posicaoMemDireita = 2;
int posicaoMemFrente = 4;
int posicaoMemTras = 6;

//Entradas
Button btnTras = Button (A1); 
Button btnDireita = Button (A2);
Button btnIr = Button (A3);
Button btnEsquerda = Button (A4);
Button btnFrente = Button (A5); 

//Saidas
#define LED_ESQUERDA 10
#define LED_FRENTE 11
#define LED_DIREITA 12
#define LED_TRAS 13

#define SAIDA_SOM A0

//Estados possíveis
#define ESTADO_AGUARDANDO 1
#define ESTADO_EXECUTANDO 2

//Variaveis
const int acaoTras = 1;
const int acaoFrente = -1;
const int acaoEsquerda = 2;
const int acaoDireita = -2;

int ESTADO_ATUAL;
int acoesContExec = 0;
int acoesContProg = 0;

int acoes[QUANTIDADE_MAXIMA_ACOES] = {0};


// !--- Acoes de Execucao ----

void feedback(int nota, int duracao, int led)
{
  if(!sound_off){
    digitalWrite(led, HIGH);
    tone(SAIDA_SOM, nota);
    delay(duracao);
    noTone(SAIDA_SOM);
    digitalWrite(led, LOW);
  }else{
    delay(duracao);
  }
}

void feedbackEsquerda(bool programando)
{
  switch (programando)
  {
    case true:
      feedback(440, 30, LED_ESQUERDA);
      break;
    case false:
      feedback(440, 30, LED_ESQUERDA);
      delay(50);
      feedback(660, 30, LED_ESQUERDA);
      delay(50);
      feedback(880, 90, LED_ESQUERDA);
      break;
    default:
      feedback(250, 250, LED_ESQUERDA);
      break;
  }
}

void feedbackDireita(bool programando)
{
  switch (programando)
  {
    case true:
      feedback(880, 30, LED_DIREITA);
      break;
    case false:
      feedback(880, 30, LED_DIREITA);
      delay(50);
      feedback(660, 30, LED_DIREITA);
      delay(50);
      feedback(440, 90, LED_DIREITA);
      break;
    default:
      feedback(250, 250, LED_DIREITA);
      break;
  }
}

void feedbackFrente(bool programando)
{
  switch (programando)
  {
    case true:
      feedback(880, 45, LED_FRENTE);
      break;
    case false:
      feedback(880, 45, LED_FRENTE);
      delay(75);
      feedback(1320, 45, LED_FRENTE);
      delay(75);
      feedback(704, 135, LED_FRENTE);
      break;
    default:                                                                                                                                                
      feedback(250, 250, LED_FRENTE);
      break;
  }
}

void feedbackTras(bool programando)
{
  switch (programando)
  {
    case true:
      feedback(880, 45, LED_TRAS);
      break;
    case false:
      feedback(880, 45, LED_TRAS);
      delay(75);
      feedback(729, 45, LED_TRAS);
      delay(75);
      feedback(1056, 135, LED_TRAS);
      break;
    default:
      feedback(250, 250, LED_TRAS);
      break;
  }
}

void feedbackFim()
{
      feedback(600, 45, LED_FRENTE);
      delay(75);
      feedback(900, 45, LED_DIREITA);
      delay(75);
      feedback(1200, 135, LED_TRAS);
      delay(75);
      feedback(1400, 135, LED_ESQUERDA);
      delay(75);
      feedback(900, 45, LED_TRAS);
      delay(75);
      feedback(700, 45, LED_DIREITA);
      delay(75);
      feedback(600, 135, LED_FRENTE);
}

void feedbackParar() 
{ 
   feedback(500, 45, LED_FRENTE); 
   delay(50); 
   feedback(400, 45, LED_DIREITA); 
   delay(50); 
   feedback(600, 135, LED_TRAS); 
   delay(50); 
   feedback(1000, 135, LED_ESQUERDA); 
}
void feedbackEasterEggActivated()
{
  digitalWrite(LED_FRENTE, HIGH);
  digitalWrite(LED_DIREITA, HIGH);
  digitalWrite(LED_TRAS, HIGH);
  digitalWrite(LED_ESQUERDA, HIGH);
  delay(1000);
  feedback(1000, 100, LED_FRENTE); 
   delay(50); 
   feedback(600, 200, LED_DIREITA); 
   delay(50); 
   feedback(200, 300, LED_TRAS); 
   delay(50); 
   feedback(1500, 400, LED_ESQUERDA); 
}

void verificarFeedback(int acoesContExec)
{
  switch (acoes[acoesContExec])
  {
    case acaoEsquerda:
      feedbackEsquerda(false);
      break;

    case acaoDireita:
      feedbackDireita(false);
      break;

    case acaoFrente:
      feedbackFrente(false);
      break;

    case acaoTras:
      feedbackTras(false);
      break;
  }
}

void salvarStep(int endereco, int valor)
{
  byte hiByte = highByte(valor);
  byte loByte = lowByte(valor);
  EEPROM.write(endereco, hiByte);
  EEPROM.write(endereco+1, loByte);
}

int lerStep(int endereco)
{
  byte hiByte = EEPROM.read(endereco);
  byte loByte = EEPROM.read(endereco+1);
  return word(hiByte, loByte);
}

void limparMemoria()
{
  steps_frente = 0;
  steps_tras = 0;
  steps_direita = 0;
  steps_esquerda = 0;
  salvarStep(posicaoMemEsquerda, 0);
  salvarStep(posicaoMemDireita, 0);
  salvarStep(posicaoMemFrente, 0);
  salvarStep(posicaoMemTras, 0);
}

void verificarInstrucao(int acoesContExec)
{
  switch (acoes[acoesContExec])
  {
    case acaoEsquerda:
      if(steps_esquerda>0){
        motores_esquerda(rope_foi_parado,steps_esquerda);
      }else{
        motores_esquerda(rope_foi_parado,TURN_STEP_OVERWRITE);
      }      
      break;

    case acaoDireita:
      if(steps_direita>0){
        motores_direita(rope_foi_parado,steps_direita);    
      }else{
        motores_direita(rope_foi_parado,TURN_STEP_OVERWRITE);
      }      
      break;

    case acaoFrente:
      if(steps_frente>0){
        motores_frente(rope_foi_parado, steps_frente);  
      }else{
        motores_frente(rope_foi_parado);
      }      
      break;

    case acaoTras:
      if(steps_tras>0){
        motores_tras(rope_foi_parado, steps_tras);
      }else{
        motores_tras(rope_foi_parado);
      }      
      break;

    default:
      //ESTADO_ATUAL = ESTADO_AGUARDANDO;
      break;
  }
}

bool easter_egg_infinite_loop(){
  int acoes_infinite[8] = {acaoFrente,acaoTras,acaoEsquerda,acaoDireita,acaoFrente,acaoTras,acaoEsquerda,acaoDireita};
  for(int i = 0; i < 8; i++){
    if(acoes[i] != acoes_infinite[i]){
      return false;
    }
  }
  testing_loop = 1;
  feedbackEasterEggActivated();
  return true;
}

void executar() {
  if(easter_egg == 42){
    easter_egg_infinite_loop();
    reiniciarProgramacao();
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
  }
  easter_egg = -1;
  if (acoesContExec < acoesContProg)
  {
    if(!testing_loop)
    {
      verificarFeedback(acoesContExec);
    }
    verificarInstrucao(acoesContExec);
  }

  if(ESTADO_ATUAL == ESTADO_AGUARDANDO && acoesContExec < acoesContProg){
    feedbackParar();
  }
  
  acoesContExec++;

  if (acoesContExec >= acoesContProg && ESTADO_ATUAL == ESTADO_EXECUTANDO) 
  { 
    if(testing_loop){
      acoesContExec = 0;
      return;
    }
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    feedbackFim(); 
  }
  
  if(ESTADO_ATUAL == ESTADO_AGUARDANDO){
    reiniciarProgramacao();
  }
}

void reiniciarProgramacao()
{
  
  zerarArrayInstrucoes();
  acoesContProg = 0;
  acoesContExec = 0;
  desligar_motores();
  
  //feedbackAguardando();
}

void zerarArrayInstrucoes()
{
  for (int i = 0; i < QUANTIDADE_MAXIMA_ACOES; i++) {
    acoes[i] = 0;
  }
}
bool rope_foi_parado(){
  btnIr.process();
  return ESTADO_ATUAL == ESTADO_AGUARDANDO ? true : false;
}
void onEsquerdaPress(Button &b){
  if(calibrando)
  {
    feedbackEsquerda(true);
    delay(100);
    ESTADO_ATUAL = ESTADO_EXECUTANDO;    
    steps_esquerda = motores_esquerda_calibracao(rope_foi_parado);
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    feedbackEsquerda(true);
    delay(100);
    salvarStep(posicaoMemEsquerda, steps_esquerda);
    return;
  }
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = acaoEsquerda;
    acoesContProg++;
    feedbackEsquerda(true);
    delay(100);
  }
}
void onDireitaPress(Button &b){
  if(calibrando)
  {
    feedbackDireita(true);
    delay(100);
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
    steps_direita = motores_direita_calibracao(rope_foi_parado);
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    feedbackDireita(true);
    delay(100);
    salvarStep(posicaoMemDireita, steps_direita);
    return;
  }
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = acaoDireita;
    acoesContProg++;
    feedbackDireita(true);
    delay(100);
  }
}
void onFrentePress(Button &b){
  if(calibrando)
  {
    feedbackFrente(true);
    delay(100);
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
    steps_frente = motores_frente_calibracao(rope_foi_parado);
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    feedbackFrente(true);
    delay(100);
    salvarStep(posicaoMemFrente, steps_frente);
    return;
  }
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = acaoFrente;
    acoesContProg++;
    feedbackFrente(true);
    delay(100);
  }
}
void onTrasPress(Button &b){
  if(calibrando)
  {
    feedbackTras(true);
    delay(100);
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
    steps_tras = motores_tras_calibracao(rope_foi_parado);
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    feedbackTras(true);
    delay(100);
    salvarStep(posicaoMemTras, steps_tras);
    return;
  }
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = acaoTras;
    acoesContProg++;
    feedbackTras(true);
    delay(100);
  }
}
void onIrPress(Button &b){
  if(acoesContProg == 0 && easter_egg >= 0){
    easter_egg++;
  }
  if (b.pin == btnIr.pin && ESTADO_ATUAL == ESTADO_EXECUTANDO)
  {
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
  }
  else if (b.pin == btnIr.pin && acoesContProg > 0)
  {
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
  }
  delay(100);
}

void onIrHold(Button &b){
  feedbackEasterEggActivated();

  if(easter_egg > 5)
  {
    limparMemoria();
    feedbackFrente(true);
    delay(50);
    feedbackTras(true);
    delay(50);
    feedbackEsquerda(true);
    delay(50);
    feedbackDireita(true);
    delay(50);
    return;
  }
  acoesContProg = 0;
  if(calibrando)
  {
    calibrando = 0;
  }
  else{
    calibrando = 1;
  }  
  delay(100);
}

void definirCallBack()
{
  btnTras.pressHandler(onTrasPress);
  btnFrente.pressHandler(onFrentePress);
  btnEsquerda.pressHandler(onEsquerdaPress);
  btnDireita.pressHandler(onDireitaPress);
  btnIr.pressHandler(onIrPress);
  btnIr.holdHandler(onIrHold, 5000);
}

void setup_processar_estados_invalidos_iniciacao(){
  btnTras.process();
  btnFrente.process();
  btnEsquerda.process();
  btnDireita.process();
  reiniciarProgramacao();
  btnIr.process();
}

void load_steps(){
  steps_direita = lerStep(posicaoMemDireita);
  steps_esquerda = lerStep(posicaoMemEsquerda);
  steps_frente = lerStep(posicaoMemFrente);
  steps_tras = lerStep(posicaoMemTras);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_ESQUERDA, OUTPUT);
  pinMode(LED_DIREITA, OUTPUT);
  pinMode(LED_FRENTE, OUTPUT);
  pinMode(LED_TRAS, OUTPUT);

  pinMode(SAIDA_SOM, OUTPUT);

  steppers_setup();
  load_steps(); 

  definirCallBack();
  ESTADO_ATUAL = ESTADO_AGUARDANDO;
  easter_egg = 0;

  setup_processar_estados_invalidos_iniciacao();  
}

void loop()
{
  switch (ESTADO_ATUAL)
  {
    case ESTADO_AGUARDANDO:
      btnTras.process();
      btnFrente.process();
      btnEsquerda.process();
      btnDireita.process();
      btnIr.process();
      break;
    case ESTADO_EXECUTANDO:
      executar();
      break;
  }
  //instrucoes_de_teste();
}
