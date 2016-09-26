#include <AccelStepper.h>
#include <Button.h>
#include <TimerObject.h>

//Tempo
TimerObject *tempo;
//unsigned long tempoInicial;
float intervalo = 3000; //Define

#define SENTIDO_HORARIO 1000
#define SENTIDO_ANTIHORARIO -1000

bool motorLigado = false;

//Entradas

Button btnDireita = Button (A4);
Button btnEsquerda = Button (A3);
Button btnFrente = Button (A1);
Button btnTras = Button (A2);
Button btnIr = Button (A5);

//Saidas
#define led1 10
#define led2 11
#define led3 12
#define led4 13

#define motor1e1  2 // IN1 no driver 1 modelo: ULN2803. //
#define motor1e2  3 // IN2 no driver 1 modelo: ULN2803. //
#define motor1e3  4 // IN3 no driver 1 modelo: ULN2803. //
#define motor1e4  5 // IN4 no driver 1 modelo: ULN2803. //

#define motor2e1  6 // IN5 no driver 2 modelo: ULN2803. //
#define motor2e2  7 // IN6 no driver 2 modelo: ULN2803. //
#define motor2e3  8 // IN7 no driver 2 modelo: ULN2803. //
#define motor2e4  9 // IN8 no driver 2 modelo: ULN2803. //

AccelStepper motor1(8, motor1e1, motor1e3, motor1e2, motor1e4); // Define motor 1, declarando todos os fios no ordem; 1,3,2,4. //
AccelStepper motor2(8, motor2e1, motor2e3, motor2e2, motor2e4);

//Estados possíveis
#define ESTADO_AGUARDANDO 1
#define ESTADO_PROGRAMANDO 2
#define ESTADO_EXECUTANDO 3
#define ESTADO_EM_ESPERA 4

//Flags Possíveis
bool temSom = false;
bool estaProgramando = false;
bool led1Aceso = false;
bool led2Aceso = false;
bool led3Aceso = false;
bool led4Aceso = false;

const int acaoDireita = 1;
const int acaoEsquerda = -1;
const int acaoFrente = 3;
const int acaoTras = 4;

int ESTADO_ATUAL;

const int qtdAcoes = 45;
int acoesContExec = 0;
int acoesContProg = 0;
int acoes[qtdAcoes] = {0};


// !--- Acoes de Execucao ----
void resetarMotores(){
  motor1.setCurrentPosition(0);
  motor2.setCurrentPosition(0);
}

void acionarMotores(int motor1Vel, int motor2Vel){
    motor1.setSpeed(motor1Vel);
    motor2.setSpeed(motor2Vel);
    motor1.runSpeed(); 
    motor2.runSpeed(); 
}

void feedbackFrente(bool programando){
  
}

void feedbackTras(bool programando){
  
}

void feedbackEsquerda(bool programando){
  
}

void feedbackDireita(bool programando){
  
}

void feedbackAguardando(){
  
}

void feedbackEspera(){

}

void irFrente(){
   acionarMotores(SENTIDO_HORARIO,SENTIDO_ANTIHORARIO);
   feedbackFrente(false);    
}

void irTras(){
   acionarMotores(-velocidade,SENTIDO_ANTIHORARIO);
   feedbackTras(false);
}

void girarEsquerda(){
   acionarMotores(SENTIDO_HORARIO,SENTIDO_ANTIHORARIO);
   feedbackEsquerda(false);
}

void girarDireita(){
   acionarMotores(SENTIDO_ANTIHORARIO,SENTIDO_HORARIO);
   feedbackDireita(false);
}



void verificarInstrucao(int acoesContExec){
    switch(acoes[acoesContExec]){
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
          ESTADO_ATUAL = ESTADO_AGUARDANDO;
          break;
    }
}

void pararMotor(){
   motorLigado = false;
   Serial.println("Batata - ");
}

// !--- Estados ---
void executar(){
    motorLigado = true;
    resetarMotores();

    if(acoesContExec < acoesContProg){
        tempo->Start();
        while(motorLigado){
          verificarInstrucao(acoesContExec);
          tempo->Update();
        }
    }
    
    acoesContExec++;

    if(acoesContExec > acoesContProg){
        ESTADO_ATUAL = ESTADO_AGUARDANDO;
        return;
    }
}

void desligarMotor(){
    digitalWrite(motor1e1,LOW);
    digitalWrite(motor1e2,LOW);
    digitalWrite(motor1e3,LOW);
    digitalWrite(motor1e4,LOW);

    digitalWrite(motor2e1,LOW);
    digitalWrite(motor2e2,LOW);
    digitalWrite(motor2e3,LOW);
    digitalWrite(motor2e4,LOW);
}

void aguardar(){
  acoesContProg = 0;
  acoesContExec = 0;
  tempo->Stop();
  motorLigado = true;
  zerarArrayInstrucoes();
  desligarMotor();
  feedbackAguardando();
  
}

void zerarArrayInstrucoes(){
  for (int i=0;i<qtdAcoes;i++){
    acoes[i] = 0;
  }
}

void definirMotor(){

  motor1.setMaxSpeed(2000);
  motor1.setSpeed(velocidade); 
  
  motor2.setMaxSpeed(2000);
  motor2.setSpeed(velocidade); 
}

void onPress(Button &b){
    
    if(ESTADO_ATUAL == ESTADO_AGUARDANDO && b.pin != btnIr.pin){
        ESTADO_ATUAL = ESTADO_PROGRAMANDO;
    }

    if(acoesContProg > qtdAcoes){
        ESTADO_ATUAL = ESTADO_EM_ESPERA;
        feedbackEspera();
    }

    if(b.pin == btnFrente.pin && ESTADO_ATUAL == ESTADO_PROGRAMANDO){
        Serial.println ("Programei pra frente!");
        acoes[acoesContProg] = acaoFrente;
        acoesContProg++;
        
    }else if(b.pin == btnTras.pin && ESTADO_ATUAL == ESTADO_PROGRAMANDO){
       Serial.println ("Programei pra Tras!");
        acoes[acoesContProg] = acaoTras;
        acoesContProg++;
        
    }else if(b.pin == btnEsquerda.pin && ESTADO_ATUAL == ESTADO_PROGRAMANDO){
       
        Serial.println ("Programei pra Esquerda!");
        acoes[acoesContProg] = acaoEsquerda;
        acoesContProg++;
        
    }else if(b.pin == btnDireita.pin && ESTADO_ATUAL == ESTADO_PROGRAMANDO){
       Serial.println ("Programei pra Direita!");
        acoes[acoesContProg] = acaoDireita;
        acoesContProg++;
        
    }else if(b.pin == btnIr.pin && acoesContProg > 0){
        ESTADO_ATUAL = ESTADO_EXECUTANDO;
    }    
}

void definirCallBack(){
  btnDireita.pressHandler(onPress);
  btnEsquerda.pressHandler(onPress);
  btnFrente.pressHandler(onPress);
  btnTras.pressHandler(onPress);
  btnIr.pressHandler(onPress);
}

void setup(){
  Serial.begin(9600); // Inicia o Serial.(padrao do arduino) //
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT); 
  pinMode(led3, OUTPUT); 
  pinMode(led4, OUTPUT); 

  definirMotor();
  definirCallBack();

  ESTADO_ATUAL = ESTADO_AGUARDANDO;

  tempo = new TimerObject(intervalo);
  tempo->setOnTimer(&pararMotor);
}

void loop(){
    btnDireita.process();
    btnEsquerda.process();
    btnFrente.process();
    btnTras.process();
    btnIr.process();

    switch(ESTADO_ATUAL){
        case ESTADO_AGUARDANDO:
          //aguardar();
          irFrente();
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


