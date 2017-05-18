#include "libs/Button/Button.cpp"
#include <Stepper.h>

#define PASSOS_IR 765
#define PASSOS_GIRAR 330

#define VEL_SENTIDO_ANTIHORARIO 1
#define VEL_SENTIDO_HORARIO -1

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

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
Stepper motor_direita(stepsPerRevolution, motor_direita_F1, motor_direita_F3, motor_direita_F2, motor_direita_F4);
Stepper motor_esquerda(stepsPerRevolution, motor_esquerda_F1, motor_esquerda_F3, motor_esquerda_F2, motor_esquerda_F4);

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

void acionarMotores(int motor_direita_sentido, int motor_esquerda_sentido)
{
  motor_direita.step((stepsPerRevolution*motor_direita_sentido) / 100);
  motor_esquerda.step((stepsPerRevolution*motor_esquerda_sentido) / 100);
}

void feedback(int nota, int duracao, int led)
{
  digitalWrite(led, HIGH);
  tone(SAIDA_SOM, nota);
  delay(duracao);
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

void feedbackAguardando()
{

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

void girarEsquerda()
{
  int passos = 0;
    
  while (passos < PASSOS_GIRAR && ESTADO_ATUAL == ESTADO_EXECUTANDO)
  {
    acionarMotores(VEL_SENTIDO_HORARIO, VEL_SENTIDO_HORARIO);
    btnIr.process();
    passos++;
  }
}

void girarDireita()
{
  int passos = 0;
    
  while (passos < PASSOS_GIRAR && ESTADO_ATUAL == ESTADO_EXECUTANDO)
  {
    acionarMotores(VEL_SENTIDO_ANTIHORARIO, VEL_SENTIDO_ANTIHORARIO);
    btnIr.process();
    passos++;
  }
}

void irFrente()
{
  int passos = 0;
    
  while (passos < PASSOS_IR && ESTADO_ATUAL == ESTADO_EXECUTANDO)
  {
    acionarMotores(VEL_SENTIDO_HORARIO, VEL_SENTIDO_ANTIHORARIO);
    btnIr.process();
    passos++;
  }
}

void irTras()
{
  int passos = 0;
    
  while (passos < PASSOS_IR && ESTADO_ATUAL == ESTADO_EXECUTANDO)
  {
    acionarMotores(VEL_SENTIDO_ANTIHORARIO, VEL_SENTIDO_HORARIO);
    btnIr.process();
    passos++;
  }
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

void executar() {
  motorLigado = true;

  if (acoesContExec < acoesContProg)
  {
    verificarFeedback(acoesContExec);

    verificarInstrucao(acoesContExec);

  }

  acoesContExec++;
  
  if (acoesContExec >= acoesContProg && ESTADO_ATUAL == ESTADO_EXECUTANDO)
  {
    feedbackFim();
  }
  
  if (acoesContExec >= acoesContProg || ESTADO_ATUAL == ESTADO_AGUARDANDO)
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
  int motorSpeed = 155;
  motor_direita.setSpeed(motorSpeed);
  motor_esquerda.setSpeed(motorSpeed);
}

void onPress(Button &b)
{
  if(ESTADO_ATUAL == ESTADO_EXECUTANDO){
    if(b.pin == btnIr.pin){
      ESTADO_ATUAL = ESTADO_AGUARDANDO;
      feedbackParar();
    }
  }else if(ESTADO_ATUAL == ESTADO_AGUARDANDO){
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
  
  
  delay(200);
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
}
