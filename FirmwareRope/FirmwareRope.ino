#include "libs/Button/Button.cpp"
#include "libs/RoPE_Steppers_28BYJ48/RoPE_Steppers_28BYJ48.cpp"

const char CARACTER_FIM_INSTRUCAO_BLUETOOTH = '&';
String mensagemBluetooth;
bool debugAtivo = false;
bool deveExecutarProximaAcao = false;
bool ehPlacaRoPE = false;
const byte DELAY_MSG_BLUETOOTH = 60;

#define QUANTIDADE_MAXIMA_ACOES 45
bool testing_loop = 0;
bool sound_off = 1;
int easter_egg;
int TURN_STEP_OVERWRITE = 170;
int WALK_STEP_OVERWRITE = 360;
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

const char ACAO_AVANCAR    = 'a',
           ACAO_RETROCEDER = 'b',
           ACAO_ESQUERDA   = 'l',
           ACAO_DIREITA    = 'r';

int ESTADO_ATUAL;
byte acoesContExec = 0;
byte acoesContProg = 0;

char acoes[QUANTIDADE_MAXIMA_ACOES] = {0};


// !--- Acoes de Execucao ----

void feedback(int nota, int duracao, int led)
{
  if(!ehPlacaRoPE){
    return;
  } 
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
    case ACAO_ESQUERDA:
      feedbackEsquerda(false);
      break;

    case ACAO_DIREITA:
      feedbackDireita(false);
      break;

    case ACAO_AVANCAR:
      feedbackFrente(false);
      break;

    case ACAO_RETROCEDER:
      feedbackTras(false);
      break;
  }
}

void verificarInstrucao(int acoesContExec)
{
  if(!ehPlacaRoPE){
    delay(500);
    acoesContExec++;
    return;
  }
  switch (acoes[acoesContExec])
  {
    case ACAO_ESQUERDA:
      motores_esquerda(rope_foi_parado,TURN_STEP_OVERWRITE);
      break;

    case ACAO_DIREITA:
      motores_direita(rope_foi_parado,TURN_STEP_OVERWRITE);
      break;

    case ACAO_AVANCAR:
      motores_frente(rope_foi_parado);
      break;

    case ACAO_RETROCEDER:
      motores_tras(rope_foi_parado);
      break;

    default:
      break;
  }
}

bool easter_egg_infinite_loop(){
  char acoes_infinite[8] = {ACAO_AVANCAR,ACAO_RETROCEDER,ACAO_ESQUERDA,ACAO_DIREITA,ACAO_AVANCAR,ACAO_RETROCEDER,ACAO_ESQUERDA,ACAO_DIREITA};
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

  if(debugAtivo){
    if(!deveExecutarProximaAcao){
      return;
    } else {
      deveExecutarProximaAcao = false;
    }
  }

  if(easter_egg == 42){
    easter_egg_infinite_loop();
    reiniciarProgramacao();
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
  }
  easter_egg = -1;
  if (acoesContExec < acoesContProg)
  {
    notificaBluetoothInicio(acoesContExec);
    verificarFeedback(acoesContExec);
    verificarInstrucao(acoesContExec);
    notificaBluetoothFim(acoesContExec);
  }
 
  if(ESTADO_ATUAL == ESTADO_AGUARDANDO && acoesContExec < acoesContProg){
    feedbackParar();
  }
  
  acoesContExec++;

  if (ESTADO_ATUAL == ESTADO_EXECUTANDO && acoesContExec >= acoesContProg) 
  { 
    if(testing_loop){
      acoesContExec = 0;
      return;
    }
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    feedbackFim();
  }
  
  if(ESTADO_ATUAL == ESTADO_AGUARDANDO){
    msgBluetooth("parou");
    reiniciarProgramacao();
  }
}

void reiniciarProgramacao(){
  zerarArrayInstrucoes();
  acoesContProg = 0;
  acoesContExec = 0;
  if(ehPlacaRoPE){
    desligar_motores();
  }
}

void zerarArrayInstrucoes(){
  for (int i = 0; i < QUANTIDADE_MAXIMA_ACOES; i++) {
    acoes[i] = 0;
  }
}

bool rope_foi_parado(){
  btnIr.process();
  return ESTADO_ATUAL == ESTADO_AGUARDANDO;
}

void onEsquerdaPress(Button &b){
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = ACAO_ESQUERDA;
    acoesContProg++;
    msgBluetoothAcoes();
    feedbackEsquerda(true);
    delay(100);
  }
}

void onDireitaPress(Button &b){
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = ACAO_DIREITA;
    acoesContProg++;
    msgBluetoothAcoes();
    feedbackDireita(true);
    delay(100);
  }
}

void onFrentePress(Button &b){
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = ACAO_AVANCAR;
    acoesContProg++;
    msgBluetoothAcoes();
    feedbackFrente(true);
    delay(100);
  }
}

void onTrasPress(Button &b){
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = ACAO_RETROCEDER;
    acoesContProg++;
    msgBluetoothAcoes();
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
    msgBluetooth("iniciou");
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
  }
  delay(100);
}

void definirCallBack(){
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

  if(ehPlacaRoPE){
    steppers_setup();
  }
  definirCallBack();

  ESTADO_ATUAL = ESTADO_AGUARDANDO;

  if(ehPlacaRoPE){
    setup_processar_estados_invalidos_iniciacao();
  }
  easter_egg = 0;
  mensagemBluetooth = "";
}

void loop(){
  receberInstrucoesBluetooth();

  switch (ESTADO_ATUAL)
  {
    case ESTADO_AGUARDANDO:
      if(ehPlacaRoPE){
        btnTras.process();
        btnFrente.process();
        btnEsquerda.process();
        btnDireita.process();
        btnIr.process();
      }
      break;
    case ESTADO_EXECUTANDO:
      executar(); 
  }
}

// !--- Funções bluetooth  ----
void receberInstrucoesBluetooth(){
  // Flush espera enviar as instruções que estiver enviando, pra poder receber
  Serial.flush();
  if(Serial.available()){
    mensagemBluetooth = Serial.readString();
    msgBluetooth(mensagemBluetooth);
    executaInstrucaoBluetooth();
  }
}

void executaInstrucaoBluetooth(){
  if( mensagemBluetooth.startsWith("teste") ){
    msgBluetoothAcoes();
  } else
  if( mensagemBluetooth.startsWith("comandos:") ){
    return copiaComandosBluetoothParaArrayDeAcoes();
  } else 
  if( mensagemBluetooth.startsWith("s") ){
    
    if(ESTADO_ATUAL == ESTADO_EXECUTANDO) {
      msgBluetooth("parou");
      ESTADO_ATUAL = ESTADO_AGUARDANDO;
    }
    else
    if(ESTADO_ATUAL == ESTADO_AGUARDANDO && acoesContProg > 0){
      msgBluetooth("iniciou");
      ESTADO_ATUAL = ESTADO_EXECUTANDO;
    }
    
  } else 
  if(mensagemBluetoothIgual("l")){ // limpa quando conecta primeira vez
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    reiniciarProgramacao();
  } else 
  if(mensagemBluetoothIgual("d:1")){
    debugAtivo = true;
  } else 
  if(mensagemBluetoothIgual("d:0")){
    debugAtivo = false;
  } else
  if(mensagemBluetoothIgual("n")){
    deveExecutarProximaAcao = true;
  }
}

// altera estado rope

void copiaComandosBluetoothParaArrayDeAcoes(){
  reiniciarProgramacao();
  // 9 pq "comandos:" tem tamanho 9 
  for(byte i=9; i < mensagemBluetooth.length(); i++){
    acoes[acoesContProg++] = mensagemBluetooth.charAt(i);
  }
}

// verificações na mensagem bluetooth

bool mensagemBluetoothIgual(String str){
  return mensagemBluetooth.equals(str);
}

// funcoes uteis

void notificaBluetoothInicio(int acoesContExec){
  delay(DELAY_MSG_BLUETOOTH);
  Serial.print("ini:");
  Serial.println(acoesContExec);
}

void notificaBluetoothFim(int acoesContExec){
  msgBluetooth("fim");
}

void msgBluetooth(String msg){
  delay(DELAY_MSG_BLUETOOTH);
  Serial.println(msg);
}

void msgBluetoothAcoes(){
  String comandos = "";
  for(int i=0; i<acoesContProg; i++){
    comandos += acoes[i];
  }
  msgBluetooth("alt_cmds:"+comandos+"&");
}
