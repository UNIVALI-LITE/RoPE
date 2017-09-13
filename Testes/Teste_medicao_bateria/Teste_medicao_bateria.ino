#define TENSAO_MAXIMA_ARDUINO 5
#define TENSAO_BATERIA 5.6

int carga_maxima_bateria = (1023 * (TENSAO_BATERIA / 2) / TENSAO_MAXIMA_ARDUINO);

void setup() {
	
  Serial.begin(9600);
  
}
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
void loop() { 
  
  Serial.print(lerTensaoBateria (A5) );
  Serial.print(" ");
  Serial.println((int) (lerPercentualBateria (A5) * 100) );
  
}