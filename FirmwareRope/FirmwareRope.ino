#include "libs/Button/Button.cpp"
#include "libs/RoPE_Steppers_28BYJ48/RoPE_Steppers_28BYJ48.cpp"

#define QUANTIDADE_MAXIMA_ACOES 45
bool testing_loop = 0;
bool sound_off = 0;
int easter_egg;
int TURN_STEP_OVERWRITE = 170;
int WALK_STEP_OVERWRITE = 360;
//Entradas

#define INTERVALO_SOM_BOTOES_1 45
#define INTERVALO_SOM_BOTOES_2 135

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

#define FEEDBACK_PROGRAMANDO 1
#define FEEDBACK_EXECUTANDO 2

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

void feedbackEsquerda(uint8_t tipo_feedback)
{
  switch (tipo_feedback)
  {
    case FEEDBACK_PROGRAMANDO:
      feedback(880, INTERVALO_SOM_BOTOES_1, LED_ESQUERDA);
      break;
    case FEEDBACK_EXECUTANDO:
      feedback(880, INTERVALO_SOM_BOTOES_1, LED_ESQUERDA);
      delay(50);
      feedback(660, INTERVALO_SOM_BOTOES_1, LED_ESQUERDA);
      delay(50);
      feedback(880, INTERVALO_SOM_BOTOES_2, LED_ESQUERDA);
      break;
    default:
      feedback(250, 250, LED_ESQUERDA);
      break;
  }
}

void feedbackDireita(uint8_t tipo_feedback)
{
  switch (tipo_feedback)
  {
    case FEEDBACK_PROGRAMANDO:
      feedback(880, INTERVALO_SOM_BOTOES_1, LED_DIREITA);
      break;
    case FEEDBACK_EXECUTANDO:
      feedback(880, INTERVALO_SOM_BOTOES_1, LED_DIREITA);
      delay(50);
      feedback(660, INTERVALO_SOM_BOTOES_1, LED_DIREITA);
      delay(50);
      feedback(440, INTERVALO_SOM_BOTOES_2, LED_DIREITA);
      break;
    default:
      feedback(250, 250, LED_DIREITA);
      break;
  }
}

void feedbackFrente(uint8_t tipo_feedback)
{
  switch (tipo_feedback)
  {
    case FEEDBACK_PROGRAMANDO:
      feedback(880, INTERVALO_SOM_BOTOES_1, LED_FRENTE);
      break;
    case FEEDBACK_EXECUTANDO:
      feedback(880, INTERVALO_SOM_BOTOES_1, LED_FRENTE);
      delay(75);
      feedback(1320, INTERVALO_SOM_BOTOES_1, LED_FRENTE);
      delay(75);
      feedback(704, INTERVALO_SOM_BOTOES_2, LED_FRENTE);
      break;
    default:                                                                                                                                                
      feedback(250, 250, LED_FRENTE);
      break;
  }
}

void feedbackTras(uint8_t tipo_feedback)
{
  switch (tipo_feedback)
  {
    case FEEDBACK_PROGRAMANDO:
      feedback(880, INTERVALO_SOM_BOTOES_1, LED_TRAS);
      break;
    case FEEDBACK_EXECUTANDO:
      feedback(880, INTERVALO_SOM_BOTOES_1, LED_TRAS);
      delay(75);
      feedback(729, INTERVALO_SOM_BOTOES_1, LED_TRAS);
      delay(75);
      feedback(1056, INTERVALO_SOM_BOTOES_2, LED_TRAS);
      break;
    default:
      feedback(250, 250, LED_TRAS);
      break;
  }
}

void feedbackFim()
{
      feedback(600, INTERVALO_SOM_BOTOES_1, LED_FRENTE);
      delay(75);
      feedback(900, INTERVALO_SOM_BOTOES_1, LED_DIREITA);
      delay(75);
      feedback(1200, 135, LED_TRAS);
      delay(75);
      feedback(1400, 135, LED_ESQUERDA);
      delay(75);
      feedback(900, INTERVALO_SOM_BOTOES_1, LED_TRAS);
      delay(75);
      feedback(700, INTERVALO_SOM_BOTOES_1, LED_DIREITA);
      delay(75);
      feedback(600, 135, LED_FRENTE);
}

void feedbackParar() 
{ 
   feedback(500, INTERVALO_SOM_BOTOES_1, LED_FRENTE); 
   delay(50); 
   feedback(400, INTERVALO_SOM_BOTOES_1, LED_DIREITA); 
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
      feedbackEsquerda(FEEDBACK_EXECUTANDO);
      break;

    case acaoDireita:
      feedbackDireita(FEEDBACK_EXECUTANDO);
      break;

    case acaoFrente:
      feedbackFrente(FEEDBACK_EXECUTANDO);
      break;

    case acaoTras:
      feedbackTras(FEEDBACK_EXECUTANDO);
      break;
  }
}

void verificarInstrucao(int acoesContExec)
{
  switch (acoes[acoesContExec])
  {
    case acaoEsquerda:
      motores_esquerda(rope_foi_parado,TURN_STEP_OVERWRITE);
      break;

    case acaoDireita:
      motores_direita(rope_foi_parado,TURN_STEP_OVERWRITE);
      break;

    case acaoFrente:
      motores_frente(rope_foi_parado);
      break;

    case acaoTras:
      motores_tras(rope_foi_parado);
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
    verificarFeedback(acoesContExec);
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
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = acaoEsquerda;
    acoesContProg++;
    feedbackEsquerda(FEEDBACK_PROGRAMANDO);
    delay(100);
  }
}
void onDireitaPress(Button &b){
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = acaoDireita;
    acoesContProg++;
    feedbackDireita(FEEDBACK_PROGRAMANDO);
    delay(100);
  }
}
void onFrentePress(Button &b){
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = acaoFrente;
    acoesContProg++;
    feedbackFrente(FEEDBACK_PROGRAMANDO);
    delay(100);
  }
}
void onTrasPress(Button &b){
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = acaoTras;
    acoesContProg++;
    feedbackTras(FEEDBACK_PROGRAMANDO);
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

void definirCallBack()
{
  btnTras.pressHandler(onTrasPress);
  btnFrente.pressHandler(onFrentePress);
  btnEsquerda.pressHandler(onEsquerdaPress);
  btnDireita.pressHandler(onDireitaPress);
  btnIr.pressHandler(onIrPress);
}

void setup_processar_estados_invalidos_iniciacao(){
  btnTras.process();
  btnFrente.process();
  btnEsquerda.process();
  btnDireita.process();
  reiniciarProgramacao();
  btnIr.process();
}

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_ESQUERDA, OUTPUT);
  pinMode(LED_DIREITA, OUTPUT);
  pinMode(LED_FRENTE, OUTPUT);
  pinMode(LED_TRAS, OUTPUT);

  pinMode(SAIDA_SOM, OUTPUT);

  steppers_setup();
  definirCallBack();

  ESTADO_ATUAL = ESTADO_AGUARDANDO;

  setup_processar_estados_invalidos_iniciacao();
  easter_egg = 0;
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
