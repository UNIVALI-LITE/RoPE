#include <avr/power.h>
#include <avr/sleep.h>

#define PINO_LED_LOOP 11
#define PINO_LED_ERRO 12
#define PINO_BOTOES A0
#define PINO_INTERRUPCAO 2

#define LED_LOOP 0
#define LED_ERRO 1

volatile bool finalizou_interrupcao = false;
volatile uint8_t estados_leds[2] = { LOW , LOW };
volatile uint32_t total_interrupcoes = 0;

uint32_t total_erros = 0;
uint32_t total_pressionamentos = 0;

uint32_t pressionamentos[4];

#ifdef DEBUG
uint32_t count = 0;
#endif // DEBUG


void baixarEstadoPinosDesnecessarios()
{
	for (uint32_t pin = A1; pin <= A5; pin++)
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}

	for (uint32_t pin = 0; pin <= 13; pin++)
	{
		if (pin != PINO_INTERRUPCAO && pin != PINO_LED_ERRO && pin != PINO_LED_LOOP)
		{
			pinMode(pin, OUTPUT);
			digitalWrite(pin, LOW);
		}
	}
}

void setup()
{
	cli();
	attachInterrupt(digitalPinToInterrupt(PINO_INTERRUPCAO), interrupcao_detectada, FALLING);

	power_spi_disable();	
	power_timer1_disable();
	power_timer2_disable();
	power_twi_disable();	

#ifndef DEBUG
	// Necessário para a comunicação via serial, não pode ser desativado se 
	// estiver em modo DEBUG, pois este modo utiliza a serial
	power_usart0_disable(); 
#endif // !DEBUG


#ifdef DEBUG

	Serial.begin(9600);

#endif // DEBUG

	pinMode(PINO_LED_LOOP, OUTPUT);
	pinMode(PINO_LED_ERRO, OUTPUT);

	pinMode(PINO_BOTOES, INPUT_PULLUP);
	pinMode(PINO_INTERRUPCAO, INPUT_PULLUP);

	digitalWrite(PINO_LED_ERRO, LOW);

	//baixarEstadoPinosDesnecessarios(); Aparentemente aumenta o consumo ao invés de diminuir

	for (uint8_t i = 0; i < 4; i++)
	{
		pressionamentos[i] = 0;
	}
}

void interrupcao_detectada()
{
	total_interrupcoes++;

	uint32_t intervalo = 5000 * 1000;

	delayMicroseconds(intervalo);
	finalizou_interrupcao = true;
}

void aguardar_interrupcao()
{
#ifdef DEBUG

	count++;
	Serial.print("Entrando em stand by. Iteracao ");
	Serial.print(count);
	Serial.println("...");
	Serial.flush();

#endif // DEBUG

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	power_adc_disable();
	sleep_bod_disable();
	power_timer0_disable();

	sei();
	sleep_mode();
}

void sair_da_interrupcao()
{
	cli();
	sleep_disable();
	power_adc_enable();
	power_timer0_enable();	

	while (!finalizou_interrupcao) {}

	finalizou_interrupcao = false;

#ifdef DEBUG

	Serial.print("Saindo do standby. Iteracao ");
	Serial.print(count);
	Serial.println("...");
	Serial.flush();

#endif // DEBUG
}

void processar_instrucoes()
{
	delay(200);

	estados_leds[LED_LOOP] = (estados_leds[LED_LOOP] == HIGH) ? LOW : HIGH;
	digitalWrite(PINO_LED_LOOP, estados_leds[LED_LOOP]);

	uint8_t estado_do_pino_da_interrupcao = digitalRead(PINO_INTERRUPCAO);
	uint16_t valor_atual_da_entrada = analogRead(PINO_BOTOES);

	digitalWrite(PINO_LED_ERRO, LOW);
	total_pressionamentos += 1;

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
	else
	{
#ifdef DEBUG
		Serial.println("ERRO: nao foi possivel detectar o botao pressionado!");
		total_erros++;
#endif // DEBUG

		digitalWrite(PINO_LED_ERRO, HIGH);
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
	Serial.print("Numero de interrupcoes ");
	Serial.print(total_interrupcoes);
	Serial.print(". O valor da entrada foi: ");
	Serial.println(valor_atual_da_entrada);
	Serial.print("Erros ocorridos ate o momento: ");
	Serial.print(total_erros);
	Serial.print(". Percentual de erros: ");
	Serial.println(round(((float)total_erros / total_pressionamentos) * 1000.0f) / 10.0f);
	Serial.println();
	Serial.flush();

#endif // DEBUG

	while (digitalRead(PINO_INTERRUPCAO) == estado_do_pino_da_interrupcao)
	{
		//delay(20);
	}
}

void loop()
{
	aguardar_interrupcao();
	sair_da_interrupcao();
	processar_instrucoes();
}