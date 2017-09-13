#include <Wire.h>

// Para realizar este teste é necessário descompactar a biblioteca que está na pasta "TestLibs"
// para dentro da pasta "libraries" do Arduino
#include <LiquidCrystal_I2C.h>


#define TENSAO_MAXIMA_ARDUINO 5
#define TENSAO_BATERIA 5.6
#define PINO_MEDIDOR_BATERIA A3

int carga_maxima_bateria = (1023 * (TENSAO_BATERIA / 2) / TENSAO_MAXIMA_ARDUINO);

// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

float lerPercentualBateria(uint8_t pino)
{
  uint16_t valorPino = analogRead(pino);
  
  return valorPino / (carga_maxima_bateria * 1.0f);
}

float lerTensaoBateria(uint8_t pino)
{
  uint16_t valorPino = analogRead(pino);
    
  return 2 * valorPino * ( TENSAO_MAXIMA_ARDUINO / 1023.0f );
}

void setup()
{
  pinMode(PINO_MEDIDOR_BATERIA, INPUT);
  
  lcd.begin (16,2);
  
  Serial.begin(9600);
  lcd.setBacklight(HIGH);
}


void loop()
{    
  lcd.setCursor(0,0);
  lcd.print("Tensao: ");
  lcd.print(lerTensaoBateria(PINO_MEDIDOR_BATERIA));
  lcd.print("   ");
  
 
  lcd.setCursor(0,1);
  lcd.print("Percentual: ");
  lcd.print((int) (lerPercentualBateria(PINO_MEDIDOR_BATERIA) * 100));
  lcd.print("   ");
  
  //delay(1000);  
}
