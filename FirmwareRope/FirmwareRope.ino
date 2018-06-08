#include "libs/Button/Button.cpp"
#include "libs/RoPE_Steppers_28BYJ48/RoPE_Steppers_28BYJ48.cpp"

String mensagemBluetooth;
bool debugAtivo = 0;
bool deveExecutarProximaAcao = 0;
bool terminouTransmissaoComandos = 0;
const bool EH_PLACA_ROPE = true;

#define QUANTIDADE_MAXIMA_ACOES 45
bool testing_loop = 0;
bool sound_off = 0;
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

const char ACAO_FRENTE     = 'f',
           ACAO_TRAS       = 't',
           ACAO_ESQUERDA   = 'e',
           ACAO_DIREITA    = 'd';

int ESTADO_ATUAL;
byte acoesContExec = 0;
byte acoesContProg = 0;

char acoes[QUANTIDADE_MAXIMA_ACOES] = {0};


// !--- Acoes de Execucao ----

void feedback(int nota, int duracao, int led)
{
  if(!EH_PLACA_ROPE){
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

void feedbackAtivouDebug(){
  feedback(200, 135, LED_FRENTE);
  feedback(600, 135, LED_DIREITA);
  feedback(200, 135, LED_ESQUERDA);
  feedback(600, 135, LED_TRAS);
}

void feedbackDesativouDebug(){
  feedback(400, 135, LED_FRENTE);
  feedback(800, 135, LED_DIREITA);
  feedback(400, 135, LED_ESQUERDA);
  feedback(800, 135, LED_TRAS);
}

void verificarFeedback(int acoesContExec)
{
  char acao = acoes[acoesContExec];
  executaFeedback(acao, false);
}

void executaFeedback(char acao, bool programando){
  switch (acao)
  {
    case ACAO_ESQUERDA:
      feedbackEsquerda(programando);
      break;

    case ACAO_DIREITA:
      feedbackDireita(programando);
      break;

    case ACAO_FRENTE:
      feedbackFrente(programando);
      break;

    case ACAO_TRAS:
      feedbackTras(programando);
      break;
  }
}

void verificarInstrucao(int acoesContExec)
{
  if(!EH_PLACA_ROPE){
    acoesContExec++;
    delay(1000);
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

    case ACAO_FRENTE:
      motores_frente(rope_foi_parado);
      break;

    case ACAO_TRAS:
      motores_tras(rope_foi_parado);
      break;

    default:
      break;
  }
}

bool easter_egg_infinite_loop(){
  char acoes_infinite[8] = {ACAO_FRENTE,ACAO_TRAS,ACAO_ESQUERDA,ACAO_DIREITA,ACAO_FRENTE,ACAO_TRAS,ACAO_ESQUERDA,ACAO_DIREITA};
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
    }
    deveExecutarProximaAcao = false;
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
    reiniciarProgramacao();
    notificaParou();
  }
}

void reiniciarProgramacao(){
  zerarArrayInstrucoes();
  acoesContProg = 0;
  acoesContExec = 0;
  if(EH_PLACA_ROPE){
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
    acoes[acoesContProg] = ACAO_FRENTE;
    acoesContProg++;
    msgBluetoothAcoes();
    feedbackFrente(true);
    delay(100);
  }
}

void onTrasPress(Button &b){
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    acoes[acoesContProg] = ACAO_TRAS;
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
    notificaIniciou();
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

  if(EH_PLACA_ROPE){
    steppers_setup();
  }
  definirCallBack();

  ESTADO_ATUAL = ESTADO_AGUARDANDO;

  if(EH_PLACA_ROPE){
    setup_processar_estados_invalidos_iniciacao();
  }
  easter_egg = 0;
  mensagemBluetooth = "";
}

void loop(){
  receberMensagemBluetooth();

  switch (ESTADO_ATUAL)
  {
    case ESTADO_AGUARDANDO:
      if(EH_PLACA_ROPE){
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

void receberMensagemBluetooth(){
  // Flush espera enviar as instruções que estiver enviando, pra poder receber
  Serial.flush();
  if(Serial.available()){
    mensagemBluetooth = Serial.readString();
    executaInstrucaoBluetooth();
  }
}

void executaInstrucaoBluetooth(){
  if( mensagemBluetooth.indexOf("cmds") != -1 ){
    reiniciarProgramacao();
    byte i = mensagemBluetooth.lastIndexOf("<cmds:") + 6;
    copiaComandosBluetoothParaArrayDeAcoes(i); // começa copia do indice 5 ("cmds:[f]fafa")
  }
  if( mensagemBluetooth.indexOf("<i>") != -1 ){
    return trocaDeEstado(); 
  } 
  if( mensagemBluetooth.indexOf("<l>") != -1 ){ // limpa quando conecta primeira vez
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    feedbackConectouBluetooth();
    debugAtivo=false;
    reiniciarProgramacao();
  }
  if( mensagemBluetooth.indexOf("<d:1>") != -1 ){
    debugAtivo = true;
    feedbackAtivouDebug();
    msgBluetooth("<d:1>"); // retorna confirmação que ativou
  }
  if( mensagemBluetooth.indexOf("<d:0>") != -1 ){
    debugAtivo = false;
    feedbackDesativouDebug();
    msgBluetooth("<d:0>"); // retorna confirmação que desativou
  } else
  if(mensagemBluetoothIgual("<n>")){
    deveExecutarProximaAcao = true;
  } else 
  if(!terminouTransmissaoComandos){
    copiaComandosBluetoothParaArrayDeAcoes(0);
  }
}

void feedbackConectouBluetooth(){
  feedbackFrente(true);
  delay(200);
  feedbackFrente(true);
  delay(200);
  feedbackFrente(true);
}

void copiaComandosBluetoothParaArrayDeAcoes(byte i){
  terminouTransmissaoComandos = false;
  for(; i < mensagemBluetooth.length(); i++){
    char c = mensagemBluetooth.charAt(i);
    if( c != '>' && c != ACAO_FRENTE && c != ACAO_TRAS && c != ACAO_ESQUERDA && c != ACAO_DIREITA){
      continue;
    }
    if(c == '>'){
      executaFeedback(mensagemBluetooth[i-1], true);
      terminouTransmissaoComandos = true;
      break;
    } else {
      acoes[acoesContProg++] = c;
    }
  }
}

bool mensagemBluetoothIgual(String str){
  return mensagemBluetooth.equals(str);
}

void trocaDeEstado(){
  if(ESTADO_ATUAL == ESTADO_EXECUTANDO) {
    if(acoesContProg >= acoesContExec){
      feedbackParar();
    }
    reiniciarProgramacao();
    notificaParou();
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
  }
  else if(ESTADO_ATUAL == ESTADO_AGUARDANDO && acoesContProg > 0){
    notificaIniciou();
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
  }
}

void notificaBluetoothInicio(int acoesContExec){
  delay(60);
  Serial.print("<ini:");
  Serial.print(acoesContExec);
  Serial.println(">");
}

void notificaBluetoothFim(int acoesContExec){
  msgBluetooth("<fim>");
}

void notificaParou(){
  msgBluetooth("<parou>");
}

void notificaIniciou(){
  msgBluetooth("<iniciou>");
}

void msgBluetooth(String msg){
  delay(60);
  Serial.println(msg);
}

void msgBluetoothAcoes(){
  String comandos = "";
  for(int i=0; i<acoesContProg; i++){
    comandos += acoes[i];
  }
  msgBluetooth("<cmds:"+comandos+">");
}

void monitorDebug(String msg){
  Serial.println("debug: "+msg);
}


