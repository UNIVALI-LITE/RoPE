// This sketch uses Button presses to control a stepper motor.
//Combined projects from the SIK GUIDE and LEARN ARDUINO BASICS books


#include <Stepper.h> //including stepper motor library
#include <Wire.h>

//defining pins section

int pino1Motor1 = 5;         
int pino2Motor1 = 6;
int pino3Motor1 = 7;
int pino4Motor1 = 8;
int passosPorRevolucao = 2048; 

const int AFalante1 = A0;
const int AFalante2 = A1;
const int botaoExecucao = 4;

int vetorInstrucoesMotor1[45] = {0};
int vetorInstrucoesMotor2[45] = {0};

const float palma = 2.5;
const float batida = 0.75;

int tempo = 0;

const int tamanhoAmostra = 50; // amostra window width in mS (50 mS = 20Hz)
unsigned int amostra;   

int contador1 = 0;
int contador2 = 0;
int indice1 = 0;
int indice2 = 0;

unsigned long tempoAnteriorExecucao = 0;
const long intervaloTempo = 2000;

#define HORARIO 1
#define ANTI_HORARIO -1

#define ESTADO_EXECUTANDO 2
#define ESTADO_PROGRAMANDO 1
#define ESTADO_ESPERANDO 0

int estadoAtual;

Stepper motor1(passosPorRevolucao, pino1Motor1, pino3Motor1, pino2Motor1, pino4Motor1);

void setup() {
   Serial.begin(9600);
  // Set up the pushbutton pins to be an input:
  pinMode(botaoExecucao, INPUT);
  motor1.setSpeed(15);
  indice1 = 0;
  contador1 = 0;
  estadoAtual = ESTADO_ESPERANDO;
}

float capaturarModulo(int porta){
   unsigned long tempoInicial= millis();  // Start of amostra window
   unsigned int pontoAPonto = 0;   // peak-to-peak level
 
   unsigned int sinalMaximo = 0;
   unsigned int sinalMinimo = 1024;
 
   while (millis() - tempoInicial < tamanhoAmostra)
   {
      amostra = analogRead(porta); 
      if (amostra < 1024)  // toss out spurious readings
      {
         if (amostra > sinalMaximo)
         {
            sinalMaximo = amostra;  // save just the max levels
         }
         else if (amostra < sinalMinimo)
         {
            sinalMinimo = amostra;  // save just the min levels
         }
      }
   }

   tempo+= 50;
   pontoAPonto = sinalMaximo - sinalMinimo;  // max - min = peak-peak amplitude
   double volts = (pontoAPonto * 5.0) / 1024;  // convert to volts
   return volts;
}

void capturarSom(){
   float resultadoAFalante1 = capaturarModulo(AFalante1);
   //float resultadoAFalante2 = capturaModulo(AFalante2);
   
   if(resultadoAFalante1 >= palma){
      vetorInstrucoesMotor1[contador1] = HORARIO; 
      contador1++;
   }else if(resultadoAFalante1 < palma && resultadoAFalante1 >= batida){
      vetorInstrucoesMotor1[contador1] = ANTI_HORARIO;
      contador1++;
   }else if(contador1 > 45){
      estadoAtual = ESTADO_EXECUTANDO;
   }
}

void finalizarProcesso(){
  contador1 = 0;
  indice1 = 0;
  estadoAtual = ESTADO_ESPERANDO;
}

void executarMotor(){
  unsigned long tempoAtual = millis();
  while (tempoAtual - tempoAnteriorExecucao >= intervaloTempo) {
     tempoAnteriorExecucao = tempoAtual;  
     motor1.step(passosPorRevolucao/8 * vetorInstrucoesMotor1[indice1]);
     tempoAtual = millis();
     if(indice1 == ANTI_HORARIO){
        Serial.println("batida"); 
     }else{
        Serial.println("Palma");
     }
  }

  indice1++;
  delay(1000);
  if(indice1 > contador1)
    finalizarProcesso();
 
  //motor1.step(passosPorRevolucao/8 * vetorInstrucoesMotor1[indice1]);
}

void loop() {
 int sinalBotao = digitalRead(botaoExecucao);
 digitalWrite(13, LOW);
 if(sinalBotao && estadoAtual == ESTADO_ESPERANDO){
    estadoAtual = ESTADO_PROGRAMANDO;
    digitalWrite(13, HIGH);
 }else if(estadoAtual == ESTADO_PROGRAMANDO && !sinalBotao){ 
    capturarSom();
 }else if(estadoAtual == ESTADO_PROGRAMANDO && sinalBotao && contador1 > 0){
    estadoAtual = ESTADO_EXECUTANDO;
    executarMotor();
 }else if(estadoAtual == ESTADO_EXECUTANDO){
    executarMotor();
 }
 //Serial.println(estadoAtual);
 //delay(50);
}  
  //motor1.step(stepsPerRevolution/8);
 

