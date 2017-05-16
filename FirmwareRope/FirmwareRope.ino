#include "libs/AccelStepper/AccelStepper.cpp"
#include "libs/ArduinoTimerObject/TimerObject.cpp"
#include "libs/Button/Button.cpp"

TimerObject *Timer = new TimerObject(0);

#define TEMPO_IR 3000
#define TEMPO_GIRAR 1285

#define VEL_SENTIDO_ANTIHORARIO 1000
#define VEL_SENTIDO_HORARIO -1000

#define QUANTIDADE_MAXIMA_ACOES 45

bool motorLigado = false;

bool emEspera = false;

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

#define motor_direita_F1  2
#define motor_direita_F2  3
#define motor_direita_F3  4
#define motor_direita_F4  5

#define motor_esquerda_F1  6
#define motor_esquerda_F2  7
#define motor_esquerda_F3  8
#define motor_esquerda_F4  9

AccelStepper motor_direita(8, motor_direita_F1, motor_direita_F3, motor_direita_F2, motor_direita_F4);
AccelStepper motor_esquerda(8, motor_esquerda_F1, motor_esquerda_F3, motor_esquerda_F2, motor_esquerda_F4);

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
void resetarMotores()
{
  motor_direita.setCurrentPosition(0);
  motor_esquerda.setCurrentPosition(0);
}

void acionarMotores(int motor_direitaVel, int motor_esquerdaVel)
{
  motor_direita.setSpeed(motor_direitaVel);
  motor_esquerda.setSpeed(motor_esquerdaVel);

  motor_direita.runSpeed();
  motor_esquerda.runSpeed();
}

void esperar(int duracao)
{
  Timer->setInterval(duracao);
  Timer->setOnTimer(&pararEsperar);

  emEspera = true;

  Timer->Start();

  while (emEspera)
  {
    Timer->Update();
  }
}

void pararEsperar()
{
  emEspera = false;
  Timer->Stop();
}

void feedback(int nota, int duracao, int led)
{
  digitalWrite(led, HIGH);
  tone(SAIDA_SOM, nota);
  esperar(duracao);
  noTone(SAIDA_SOM);
  digitalWrite(led, LOW);
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
      esperar(50);
      feedback(660, 30, LED_ESQUERDA);
      esperar(50);
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
      esperar(50);
      feedback(660, 30, LED_DIREITA);
      esperar(50);
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
      esperar(75);
      feedback(1320, 45, LED_FRENTE);
      esperar(75);
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
      esperar(75);
      feedback(729, 45, LED_TRAS);
      esperar(75);
      feedback(1056, 135, LED_TRAS);
      break;
    default:
      feedback(250, 250, LED_TRAS);
      break;
  }
}

void feedbackAguardando()
{

}

void feedbackEspera()
{

}

void girarEsquerda()
{
  Timer->setInterval(TEMPO_GIRAR);
  Timer->setOnTimer(&pararMotor);

  Timer->Start();

  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_HORARIO,VEL_SENTIDO_HORARIO);
    Timer->Update();
  }
  Timer->Stop();
}

void girarDireita()
{
  Timer->setInterval(TEMPO_GIRAR);
  Timer->setOnTimer(&pararMotor);

  Timer->Start();

  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_ANTIHORARIO, VEL_SENTIDO_ANTIHORARIO);
    Timer->Update();
  }
  Timer->Stop();
}

void irFrente()
{
  Timer->setInterval(TEMPO_IR);
  Timer->setOnTimer(&pararMotor);

  Timer->Start();

  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_HORARIO,VEL_SENTIDO_ANTIHORARIO);
    Timer->Update();
  }
  Timer->Stop();
}

void irTras()
{
  Timer->setInterval(TEMPO_IR);
  Timer->setOnTimer(&pararMotor);

  Timer->Start();

  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_ANTIHORARIO,VEL_SENTIDO_HORARIO);
    Timer->Update();
  }
  Timer->Stop();
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

void verificarInstrucao(int acoesContExec)
{
  switch (acoes[acoesContExec])
  {
    case acaoEsquerda:
      girarEsquerda();
      break;

    case acaoDireita:
      girarDireita();
      break;

    case acaoFrente:
      irFrente();
      break;

    case acaoTras:
      irTras();
      break;

    default:
      //ESTADO_ATUAL = ESTADO_AGUARDANDO;
      break;
  }
}

void pararMotor()
{
  motorLigado = false;
}

void executar() {
  motorLigado = true;

  if (acoesContExec < acoesContProg)
  {
    resetarMotores();

    verificarFeedback(acoesContExec);

    verificarInstrucao(acoesContExec);

  }

  acoesContExec++;

  if (acoesContExec >= acoesContProg)
  {
    ESTADO_ATUAL = ESTADO_AGUARDANDO;
    reiniciarProgramacao();
    return;
  }
}

void desligarMotor()
{
  digitalWrite(motor_direita_F1, LOW);
  digitalWrite(motor_direita_F2, LOW);
  digitalWrite(motor_direita_F3, LOW);
  digitalWrite(motor_direita_F4, LOW);

  digitalWrite(motor_esquerda_F1, LOW);
  digitalWrite(motor_esquerda_F2, LOW);
  digitalWrite(motor_esquerda_F3, LOW);
  digitalWrite(motor_esquerda_F4, LOW);
}

void reiniciarProgramacao()
{
  desligarMotor();
  Timer->Stop();
  zerarArrayInstrucoes();
  motorLigado = true;
  acoesContProg = 0;
  acoesContExec = 0;

  //feedbackAguardando();
}

void zerarArrayInstrucoes()
{
  for (int i = 0; i < QUANTIDADE_MAXIMA_ACOES; i++) {
    acoes[i] = 0;
  }
}

void definirMotor()
{
  motor_direita.setMaxSpeed(2000);
  motor_direita.setSpeed(VEL_SENTIDO_ANTIHORARIO);

  motor_esquerda.setMaxSpeed(2000);
  motor_esquerda.setSpeed(VEL_SENTIDO_ANTIHORARIO);
}

void onPress(Button &b)
{
  if(ESTADO_ATUAL != ESTADO_AGUARDANDO){
    return;
  }
  
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    if (b.pin == btnEsquerda.pin)
    {
      acoes[acoesContProg] = acaoEsquerda;
      acoesContProg++;
      feedbackEsquerda(true);
    }
  
    else if (b.pin == btnDireita.pin)
    {
      acoes[acoesContProg] = acaoDireita;
      acoesContProg++;
      feedbackDireita(true);
    }
  
    else if (b.pin == btnFrente.pin)
    {
      acoes[acoesContProg] = acaoFrente;
      acoesContProg++;
      feedbackFrente(true);
    }
  
    else if (b.pin == btnTras.pin)
    {
      acoes[acoesContProg] = acaoTras;
      acoesContProg++;
      feedbackTras(true);
    }
  }

  if (b.pin == btnIr.pin && acoesContProg > 0)
  {
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
  }
}

void definirCallBack()
{
  btnTras.pressHandler(onPress);
  btnFrente.pressHandler(onPress);
  btnEsquerda.pressHandler(onPress);
  btnDireita.pressHandler(onPress);
  btnIr.pressHandler(onPress);
}

void setup_processar_estados_invalidos_iniciacao(){
  btnTras.process();
  btnFrente.process();
  btnEsquerda.process();
  btnDireita.process();
  reiniciarProgramacao();
  btnIr.process();
}

void instrucoes_de_teste(){
  onPress(btnFrente);
  onPress(btnTras);
  onPress(btnEsquerda);
  onPress(btnDireita);
  onPress(btnIr);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_ESQUERDA, OUTPUT);
  pinMode(LED_DIREITA, OUTPUT);
  pinMode(LED_FRENTE, OUTPUT);
  pinMode(LED_TRAS, OUTPUT);

  pinMode(SAIDA_SOM, OUTPUT);

  definirMotor();
  definirCallBack();

  ESTADO_ATUAL = ESTADO_AGUARDANDO;

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
