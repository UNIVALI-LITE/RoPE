#include "libs/AccelStepper/AccelStepper.cpp"
#include "libs/ArduinoTimerObject/TimerObject.cpp"
#include "libs/Button/Button.cpp"

TimerObject *Timer = new TimerObject(0);

#define TEMPO_IR 3000
#define TEMPO_GIRAR 1285

#define VEL_SENTIDO_HORARIO 1000
#define VEL_SENTIDO_ANTIHORARIO -1000

#define QUANTIDADE_MAXIMA_ACOES 45

bool motorLigado = false;

bool emEspera = false;

//Entradas

Button btnFrente = Button (A1); 
Button btnTras = Button (A5); 
Button btnDireita = Button (A4); //
Button btnEsquerda = Button (A2);
Button btnIr = Button (A3);

//A1 = Tras
//A2 = Direita 
//A3 = Confirmar
//A4 = Esquerda
//A5 = Frente

//Saidas
#define LED_1 10 //Esquerda OK
#define LED_2 12 //Direita OK
#define LED_3 11 //Frente OK
#define LED_4 13 //Tras OK

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
#define ESTADO_EXECUTANDO 2

//Variaveis
const int acaoDireita = 1;
const int acaoEsquerda = -1;
const int acaoFrente = 2;
const int acaoTras = -2;

int ESTADO_ATUAL;
int acoesContExec = 0;
int acoesContProg = 0;

int acoes[QUANTIDADE_MAXIMA_ACOES] = {0};


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
    default:
      feedback(250, 250, LED_1);
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
    default:
      feedback(250, 250, LED_2);
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
    default:                                                                                                                                                
      feedback(250, 250, LED_3);
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
    default:
      feedback(250, 250, LED_4);
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
  Timer->setInterval(TEMPO_GIRAR);
  Timer->setOnTimer(&pararMotor);

  Timer->Start();

  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_HORARIO, VEL_SENTIDO_HORARIO);
    Timer->Update();
  }
  Timer->Stop();
}

void irTras()
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

void girarEsquerda()
{
  Timer->setInterval(TEMPO_IR);
  Timer->setOnTimer(&pararMotor);

  Timer->Start();

  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_HORARIO, VEL_SENTIDO_ANTIHORARIO);
    Timer->Update();
  }
  Timer->Stop();
}

void girarDireita()
{
  Timer->setInterval(TEMPO_IR);
  Timer->setOnTimer(&pararMotor);

  Timer->Start();

  while (motorLigado)
  {
    acionarMotores(VEL_SENTIDO_ANTIHORARIO, VEL_SENTIDO_HORARIO);
    Timer->Update();
  }
  Timer->Stop();
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
    reiniciarProgramacao();
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
  motor1.setMaxSpeed(2000);
  motor1.setSpeed(VEL_SENTIDO_HORARIO);

  motor2.setMaxSpeed(2000);
  motor2.setSpeed(VEL_SENTIDO_HORARIO);
}

void onPress(Button &b)
{
  if(ESTADO_ATUAL != ESTADO_AGUARDANDO){
    return;
  }
  
  if (acoesContProg < QUANTIDADE_MAXIMA_ACOES)
  {
    if (b.pin == btnDireita.pin)
    {
      acoes[acoesContProg] = acaoFrente;
      acoesContProg++;
      feedbackFrente(true);
    }
  
    else if (b.pin == btnEsquerda.pin)
    {
      acoes[acoesContProg] = acaoTras;
      acoesContProg++;
      feedbackTras(true);
    }
  
    else if (b.pin == btnTras.pin)
    {
      acoes[acoesContProg] = acaoEsquerda;
      acoesContProg++;
      feedbackEsquerda(true);
    }
  
    else if (b.pin == btnFrente.pin)
    {
      acoes[acoesContProg] = acaoDireita;
      acoesContProg++;
      feedbackDireita(true);
    }
  }

  if (b.pin == btnIr.pin && acoesContProg > 0)
  {
    ESTADO_ATUAL = ESTADO_EXECUTANDO;
  }
}

void definirCallBack()
{
  btnFrente.pressHandler(onPress);
  btnTras.pressHandler(onPress);
  btnDireita.pressHandler(onPress);
  btnEsquerda.pressHandler(onPress);
  btnIr.pressHandler(onPress);
}

void setup_processar_estados_invalidos_iniciacao(){
  btnFrente.process();
  btnTras.process();
  btnDireita.process();
  btnEsquerda.process();
  reiniciarProgramacao();
  btnIr.process();
}

void instrucoes_de_teste(){
  onPress(btnTras);
  onPress(btnFrente);
  onPress(btnDireita);
  onPress(btnEsquerda);
  onPress(btnIr);
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

  setup_processar_estados_invalidos_iniciacao();
}

void loop()
{
  switch (ESTADO_ATUAL)
  {
    case ESTADO_AGUARDANDO:
      btnFrente.process();
      btnTras.process();
      btnDireita.process();
      btnEsquerda.process();
      btnIr.process();
      break;
    case ESTADO_EXECUTANDO:
      executar();
      break;
  }
  //instrucoes_de_teste();
}
