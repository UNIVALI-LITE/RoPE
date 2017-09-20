#define PINO_LED_LOOP 11
#define PINO_LED_ERRO 12
#define PINO_BOTOES A0
#define PINO_INTERRUPCAO 2

#define LED_LOOP 0
#define LED_ERRO 1

volatile uint8_t estados_leds[2] = { LOW , LOW };

uint32_t total_erros = 0;
uint32_t total_pressionamentos = 0;

uint32_t pressionamentos[4];

#ifdef DEBUG
uint32_t count = 0;
#endif // DEBUG



void setup()
{
#ifdef DEBUG

	Serial.begin(9600);

#endif // DEBUG

	pinMode(PINO_LED_LOOP, OUTPUT);
	pinMode(PINO_LED_ERRO, OUTPUT);

	pinMode(PINO_BOTOES, INPUT_PULLUP);
	pinMode(PINO_INTERRUPCAO, INPUT_PULLUP);

	digitalWrite(PINO_LED_ERRO, LOW);

	for (uint8_t i = 0; i < 4; i++)
	{
		pressionamentos[i] = 0;
	}

	delay(100);
}

void loop()
{
	uint8_t estado_do_pino_da_interrupcao = digitalRead(PINO_INTERRUPCAO);	

	digitalWrite(PINO_LED_ERRO, LOW);

	if (estado_do_pino_da_interrupcao == LOW)
	{
		//delay(10);

		uint16_t valor_atual_da_entrada = analogRead(PINO_BOTOES);

		total_pressionamentos += 1;

		if (valor_atual_da_entrada > 350)
		{
#ifdef DEBUG
			Serial.println("ERRO: nao foi possivel detectar o botao pressionado!");
			total_erros++;
#endif // DEBUG

			digitalWrite(PINO_LED_ERRO, HIGH);
		}
		else
		{
			estados_leds[LED_LOOP] = (estados_leds[LED_LOOP] == HIGH) ? LOW : HIGH;
			digitalWrite(PINO_LED_LOOP, estados_leds[LED_LOOP]);

			if (valor_atual_da_entrada >= 1 && valor_atual_da_entrada <= 50)
			{
				pressionamentos[0] += 1;
			}
			else if (valor_atual_da_entrada >= 51 && valor_atual_da_entrada <= 150)
			{
				pressionamentos[1] += 1;
			}
			else if (valor_atual_da_entrada >= 151 && valor_atual_da_entrada <= 250)
			{
				pressionamentos[2] += 1;
			}
			else if (valor_atual_da_entrada >= 251 && valor_atual_da_entrada <= 350)
			{
				pressionamentos[3] += 1;
			}
		}

#ifdef DEBUG

		Serial.print("Pressionamentos: ");

		for (uint8_t i = 0; i < 4; i++)
		{
			Serial.print("Botao ");
			Serial.print(i + 1);
			Serial.print(" = ");
			Serial.print(pressionamentos[i]);

			if (i < 3)
			{
				Serial.print(", ");
			}
		}

		Serial.println();
		Serial.print("O valor da entrada foi: ");
		Serial.println(valor_atual_da_entrada);
		Serial.print("Erros ocorridos ate o momento: ");
		Serial.print(total_erros);
		Serial.print(". Percentual de erros: ");
		Serial.println(round(((float)total_erros / total_pressionamentos) * 1000.0f) / 10.0f);
		Serial.println();
		Serial.flush();

#endif // DEBUG

		while (digitalRead(PINO_INTERRUPCAO) == LOW)
		{
			//delay(20);
}
	}

}