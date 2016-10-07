#include <AccelStepper.h>
#include <Button.h>
#include <TimerObject.h>

TimerObject *tempo = new TimerObject(3000);

TimerObject *tempoEsperar = new TimerObject(100);

#define TEMPO_IR 3000
#define TEMPO_GIRAR 1275

#define VEL_SENTIDO_HORARIO 1000
#define VEL_SENTIDO_ANTIHORARIO -1000

bool motorLigado = false;

bool emEspera = false;

//Entradas

Button btnDireita = Button (A4); //Tras
Button btnEsquerda = Button (A3); //Frente
Button btnFrente = Button (A1); //
Button btnTras = Button (A2);
Button btnIr = Button (A5);

//Saidas
#define LED_1 10
#define LED_2 11
#define LED_3 12
#define LED_4 13

#define SAIDA_SOM A0

#define MOTOR1_F1  2
#define MOTOR1_F2  3
#define MOTOR1_F3  4
#define MOTOR1_F4  5

#define MOTOR2_F1  6
#define MOTOR2_F2  7
#define MOTOR2_F3  8
#define MOTOR2_F4  9

AccelStepper motor1(8, MOTOR1_F1, MOTOR1_F3, MOTOR1_F2, MOTOR1_F4);
AccelStepper motor2(8, MOTOR2_F1, MOTOR2_F3, MOTOR2_F2, MOTOR2_F4);

//Estados possíveis
#define ESTADO_AGUARDANDO 1
#define ESTADO_PROGRAMANDO 2
#define ESTADO_EXECUTANDO 3
#define ESTADO_EM_ESPERA 4

//Flags Possíveis
bool temSom = false;

bool estaProgramando = false;

bool LED_1Aceso = false;
bool LED_2Aceso = false;
bool LED_3Aceso = false;
bool LED_4Aceso = false;

const int acaoDireita = 1;
const int acaoEsquerda = -1;
const int acaoFrente = 2;
const int acaoTras = -2;

int ESTADO_ATUAL;

const int qtdAcoes = 45;

int acoesContExec = 0;
int acoesContProg = 0;

int acoes[qtdAcoes] = {0};


// !--- Acoes de Execucao ----
void resetarMotores()
{
  motor1.setCurrentPosition(0);
  motor2.setCurrentPosition(0);
}

void acionarMotores(int motor1Vel, int motor2Vel)
{
  motor1.setSpeed(motor1Vel);
  motor2.setSpeed(motor2Vel);

  motor1.runSpeed();
  motor2.runSpeed();
}

void esperar(int tempo)
{
  if(!tempoEsperar->isEnabled())
  {
  tempoEsperar->setInterval(tempo);
  tempoEsperar->setOnTimer(&pararEsperar);

  emEspera = true;

  tempoEsperar->Start();

  while (emEspera)
  {
    tempoEsperar->Update();
  }
  }
}

void pararEsperar()
{
  emEspera = false;

  tempoEsperar->Stop();
}

void feedback(int nota, int duracao, int led)
{
  digitalWrite(led, HIGH);

  tone(SAIDA_SOM, nota);

  esperar(duracao);

  noTone(SAIDA_SOM);

  digitalWrite(led, LOW);
}

void feedbackFrente(bool programando)
{
  switch (programando)
  {
    case true:
      feedback(440, 30, LED_1);
      break;
    case false:
      feedback(440, 30, LED_1);
      esperar(50);
      feedback(660, 30, LED_1);
      esperar(50);
      feedback(880, 90, LED_1);
      break;
  }
}

void feedbackTras(bool programando)
{
  switch (programando)
  {
    case true:
      feedback(880, 30, LED_2);
      break;
    case false:
      feedback(880, 30, LED_2);
      esperar(50);
      feedback(660, 30, LED_2);
      esperar(50);
      feedback(440, 90, LED_2);
      break;
  }
}

void feedbackEsquerda(bool programando)
{
  switch (programando)
  {
    case true:
      feedback(880, 45, LED_3);
      break;
    case false:
      feedback(880, 45, LED_3);
      esperar(75);
      feedback(1320, 45, LED_3);
      esperar(75);
      feedback(704, 135, LED_3);
      break;
  }
}

void feedbackDireita(bool programando)
{
  switch (programando)
  {
    case true:
      feedback(880, 45, LED_4);
      break;
    case false:
      feedback(880, 45, LED_4);
      esperar(75);
      feedback(729, 45, LED_4);
      esperar(75);
      feedback(1056, 135, LED_4);
      break;
  }
}

void feedbackAguardando()
{

}

void feedbackEspera()
{

}

void irFrente()
{
  if(!tempo->isEnabled())
  {
  tempo->setInterval(TEMPO_GIRAR);
  tempo->setOnTimer(&pararMotor);
  tempo->Start();
  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_HORARIO, VEL_SENTIDO_HORARIO);
    tempo->Update();
  }
  tempo->Stop();
  }
}

void irTras()
{
  if(!tempo->isEnabled())
  {
  tempo->setInterval(TEMPO_GIRAR);
  tempo->setOnTimer(&pararMotor);
  tempo->Start();
  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_ANTIHORARIO, VEL_SENTIDO_ANTIHORARIO);
    tempo->Update();
  }
  tempo->Stop();
  }
}

void girarEsquerda()
{
  if(!tempo->isEnabled())
  {
  tempo->setInterval(TEMPO_IR);
  tempo->setOnTimer(&pararMotor);
  tempo->Start();
  while (motorLigado)
  {

    acionarMotores(VEL_SENTIDO_HORARIO, VEL_SENTIDO_ANTIHORARIO);
    tempo->Update();
  }
  tempo->Stop();
  }
}

void girarDireita()
{
  if(!tempo->isEnabled())
  {
  tempo->setInterval(TEMPO_IR);
  tempo->setOnTimer(&pararMotor);
  tempo->Start();
  while (motorLigado)
  {

    acionarMotores(VEL_SENTIDO_ANTIHORARIO, VEL_SENTIDO_HORARIO);
    tempo->Update();
  }
  tempo->Stop();
  }
}

void verificarFeedback(int acoesContExec)
{
  switch (acoes[acoesContExec])
  {
    case acaoFrente:
      feedbackFrente(false);
      break;

    case acaoTras:
      feedbackTras(false);
      break;

    case acaoEsquerda:
      feedbackEsquerda(false);
      break;

    case acaoDireita:
      feedbackDireita(false);
      break;
  }
}

void verificarInstrucao(int acoesContExec)
{
  switch (acoes[acoesContExec])
  {
    case acaoFrente:
      irFrente();
      break;

    case acaoTras:
      irTras();
      break;

    case acaoEsquerda:
      girarEsquerda();
      break;

    case acaoDireita:
      girarDireita();
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
    return;
  }
}

void desligarMotor()
{
  digitalWrite(MOTOR1_F1, LOW);
  digitalWrite(MOTOR1_F2, LOW);
  digitalWrite(MOTOR1_F3, LOW);
  digitalWrite(MOTOR1_F4, LOW);

  digitalWrite(MOTOR2_F1, LOW);
  digitalWrite(MOTOR2_F2, LOW);
  digitalWrite(MOTOR2_F3, LOW);
  digitalWrite(MOTOR2_F4, LOW);
}

void aguardar()
{
  desligarMotor();
  tempo->Stop();
  zerarArrayInstrucoes();
  motorLigado = true;
  acoesContProg = 0;
  acoesContExec = 0;

  //feedbackAguardando();
}

void zerarArrayInstrucoes()
{
  for (int i = 0; i < qtdAcoes; i++) {
    acoes[i] = 0;
  }
}

void definirMotor()
{
  motor1.setMaxSpeed(2000);
  motor1.setSpeed(VEL_SENTIDO_HORARIO);

  motor2.setMaxSpeed(2000);
  motor2.setSpeed(VEL_SENTIDO_HORARIO);
}

void onPress(Button &b)
{
  if (ESTADO_ATUAL == ESTADO_AGUARDANDO && b.pin != btnIr.pin)
  {
    ESTADO_ATUAL = ESTADO_PROGRAMANDO;
  }

  if (acoesContProg > qtdAcoes)
  {
    ESTADO_ATUAL = ESTADO_EM_ESPERA;
    //feedbackEspera();
    return;
  }

  if (b.pin == btnFrente.pin && ESTADO_ATUAL == ESTADO_PROGRAMANDO)
  {
    acoes[acoesContProg] = acaoFrente;
    acoesContProg++;
    feedbackFrente(true);
  }

  else if (b.pin == btnTras.pin && ESTADO_ATUAL == ESTADO_PROGRAMANDO)
  {
    acoes[acoesContProg] = acaoTras;
    acoesContProg++;
    feedbackTras(true);
  }

  else if (b.pin == btnEsquerda.pin && ESTADO_ATUAL == ESTADO_PROGRAMANDO)
  {
    acoes[acoesContProg] = acaoEsquerda;
    acoesContProg++;
    feedbackEsquerda(true);
  }

  else if (b.pin == btnDireita.pin && ESTADO_ATUAL == ESTADO_PROGRAMANDO)
  {
    acoes[acoesContProg] = acaoDireita;
    acoesContProg++;
    feedbackDireita(true);
  }

  else if (b.pin == btnIr.pin && acoesContProg > 0)
  {
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
  }
}

void definirCallBack()
{
  btnDireita.pressHandler(onPress);
  btnEsquerda.pressHandler(onPress);
  btnFrente.pressHandler(onPress);
  btnTras.pressHandler(onPress);
  btnIr.pressHandler(onPress);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);

  pinMode(SAIDA_SOM, OUTPUT);

  definirMotor();
  definirCallBack();

  ESTADO_ATUAL = ESTADO_AGUARDANDO;
}

void loop()
{
  btnDireita.process();
  btnEsquerda.process();
  btnFrente.process();
  btnTras.process();
  btnIr.process();

  switch (ESTADO_ATUAL)
  {
    case ESTADO_AGUARDANDO:
      aguardar();
      break;
    case ESTADO_PROGRAMANDO:
      break;
    case ESTADO_EXECUTANDO:
      executar();
      break;
    case ESTADO_EM_ESPERA:

      break;
    default:
      break;
  }
}
