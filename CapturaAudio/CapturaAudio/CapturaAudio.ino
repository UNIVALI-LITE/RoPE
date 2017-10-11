

#include <Stepper.h> 
#include <Wire.h>


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

const float palma = 2.34;
const float desvPadraoPalma = 0.69;
const float batida = 0.20;
const float desvPadraoBatida = 0;

int tempo = 0;

const int tamanhoAmostra = 50; 
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

  pinMode(botaoExecucao, INPUT);
  motor1.setSpeed(15);
  indice1 = 0;
  contador1 = 0;
  estadoAtual = ESTADO_ESPERANDO;
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

float capaturarModulo(int porta){
   unsigned long tempoInicial= millis(); 
   unsigned int pontoAPonto = 0;   
 
   unsigned int sinalMaximo = 0;
   unsigned int sinalMinimo = 1024;
 
   while (millis() - tempoInicial < tamanhoAmostra)
   {
      amostra = analogRead(porta); 
      if (amostra < 1024)  // toss out spurious readings
      {
         if (amostra > sinalMaximo)
         {
            sinalMaximo = amostra; 
         }
         else if (amostra < sinalMinimo)
         {
            sinalMinimo = amostra;  
         }
      }
   }

   tempo+= 50;
   pontoAPonto = sinalMaximo - sinalMinimo; 
   double volts = (pontoAPonto * 5.0) / 1024;  
   
   return volts;
}

void capturarSom(){
   float resultadoAFalante1 = capaturarModulo(AFalante1);

   
   if(resultadoAFalante1 >= (palma - desvPadraoPalma) && resultadoAFalante1  <= (palma + desvPadraoPalma)){
      vetorInstrucoesMotor1[contador1] = HORARIO; 
      contador1++;
      Serial.print("Gravei Palma com sinal: ");
      Serial.print(resultadoAFalante1);
      Serial.println("");
   }else if(resultadoAFalante1 >= batida && resultadoAFalante1 < palma - desvPadraoPalma){
      vetorInstrucoesMotor1[contador1] = ANTI_HORARIO;
      contador1++;
      Serial.print("Gravei Batida com sinal: ");
      Serial.print(resultadoAFalante1);
      Serial.println("");
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
     if(vetorInstrucoesMotor1[indice1] == ANTI_HORARIO){
        Serial.println("Executei Batida"); 
     }else if(vetorInstrucoesMotor1[indice1] == HORARIO){
        Serial.println("Executei Palma");
     }else{
        Serial.println("Executei um Erro!");
     }
  }

  indice1++;
  delay(1000);
  if(indice1 > contador1)
    finalizarProcesso();

}

void loop() {
 int sinalBotao = digitalRead(botaoExecucao);
 
 if(sinalBotao && estadoAtual == ESTADO_ESPERANDO){
    estadoAtual = ESTADO_PROGRAMANDO;
    digitalWrite(13, LOW);
    
 }else if(estadoAtual == ESTADO_PROGRAMANDO && !sinalBotao){ 
    capturarSom();
    digitalWrite(13, HIGH);
 }else if(estadoAtual == ESTADO_PROGRAMANDO && sinalBotao && contador1 > 0){
    estadoAtual = ESTADO_EXECUTANDO;
    executarMotor();
    digitalWrite(13, LOW);
 }else if(estadoAtual == ESTADO_EXECUTANDO){
    executarMotor();
    digitalWrite(13, LOW);
 }

}  


