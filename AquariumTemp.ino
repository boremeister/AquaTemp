
/*

AquariumTemp ver 1
Author: boremeister

*/

/*

* one-wire protocol - http://playground.arduino.cc/Learning/OneWire
* find device address - http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
*		mine is device address (temperature sensor DS18B20) - 0x28, 0xEE, 0x80, 0x97, 0x06, 0x00, 0x00, 0x59
* connecting LCD 1602 - http://www.dreamdealer.nl/tutorials/connecting_a_1602a_lcd_display_and_a_light_sensor_to_arduino_uno.html
*/

/*
* TODO
* a) try controling LED power with one of atmega's digital pin & transistor
* b) try 8 MHz again
*/

// LIBRARIES 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <avr/sleep.h>

// PINS AND DEFINITIONS
#define TEMP_SENS_PIN 3		// temperature sensor
#define BKG_LIGHT 13		// controls backlight
#define LED 8				// blinking LED
#define DEBUG_MODE 1		// DEBUG_MODE mode (1 - on, 0 - off)
#define WAKE_PIN 2			// pin used for waking up (interrupt port)
#define LCD_ON_TIME 7000	// amount of time LCD background light is ON
OneWire oneWire(TEMP_SENS_PIN);			// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);	// Pass our oneWire reference to Dallas Temperature. 
DeviceAddress Probe01 = { 0x28, 0xEE, 0x80, 0x97, 0x06, 0x00, 0x00, 0x59 };

LiquidCrystal lcd(12, 11, 10, 9, 5, 6);	// LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

// GLOBAL VARIABLES
float temp;
long int lightInterval = 7000;			// interval for measuring temperature [ms]
bool ledBacklightState = false;			// current state of led backlight

void setup()
{

#if defined (DEBUG_MODE)
	Serial.begin(9600);
	sensors.begin();
	Serial.println("Prepare ready!");
	Serial.println("Reading temperature ...");
#endif

	// prepare PINs
	pinMode(WAKE_PIN, INPUT);
	pinMode(BKG_LIGHT, OUTPUT);
	pinMode(LED, OUTPUT);

	lcd.begin(16, 2);
	//lcd.clear();
	//digitalWrite(BKG_LIGHT, HIGH);
	//// title
	//lcd.setCursor(0, 0);
	//lcd.write("LCD screen ...");
	//// content
	//lcd.setCursor(0, 1);
	//lcd.print("... initialized!");

	// default start delay
	delay(1000);
}

void loop()
{

	/*
	* display data on led
	*/
	lcd.clear();
	// title
	lcd.setCursor(0, 0);
	lcd.print("Measuring ...");
	lcd.setCursor(1, 1);
	lcd.print("... temperature");

	digitalWrite(BKG_LIGHT, HIGH);

	// read temperature
	sensors.requestTemperatures(); // Send the command to get temperatures
	// You can have more than one IC on the same bus. 
	// 0 refers to the first IC on the wire
	temp = sensors.getTempCByIndex(0);	// returns 4 decimal places?

#if defined(DEBUG_MODE)
	Serial.println(temp);
#endif

	//digitalWrite(LED, HIGH);
	flashLED();
	
	/*
	* display data on led
	*/
	lcd.clear();
	// title
	lcd.setCursor(0, 0);
	lcd.print("Temperature is");
	// content
	lcd.setCursor(5, 1);
	lcd.print(temp, 4);
	lcd.setCursor(10, 1);
	lcd.print(" ");
	lcd.print((char)223);	// symbol for degree
	lcd.print("C");
	
	// show data on LCD for some time
	delay(LCD_ON_TIME);

	// show 'slepping' on LCD and turn backlight off
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Sleeping ...");
	digitalWrite(BKG_LIGHT, LOW);

	// go to sleep
	sleepNow();
}

/*
*	puts arduino/atmega to sleep
*/
void sleepNow()
{

	// disable ADC
	ADCSRA = 0;

	// use interrupt 0 (pin 2) and run function wakeUpNow when pin 2 gets LOW
	attachInterrupt(0, wakeUpNow, LOW);

	delay(100);

	// set sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	noInterrupts();

	// enable the sleep bit in the mcucr register so sleep is possible. just a safety pin
	sleep_enable();

	// turn off brown-out enable in software
	MCUCR = bit(BODS) | bit(BODSE);  // turn on brown-out enable select
	MCUCR = bit(BODS);        // this must be done within 4 clock cycles of above
	interrupts();             // guarantees next instruction executed

	// put devide to sleep
	sleep_mode();
	
	/*
	* THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
	*/
	
	// first thing after waking from sleep disable sleep
	sleep_disable();
}

/*
*	handle the interrupt after wakeup
*/
void wakeUpNow()
{
	// execute code here after wake-up before returning to the loop() function
	// timers and code using timers (serial.print and more...) will not work here.

	// disables interrupt 0 on pin 2 so the wakeUpNow code will not be executed during normal running time.
	detachInterrupt(0);
}

void flashLED()
{
	for (byte i = 0; i < 3; i++)
	{
		digitalWrite(LED, HIGH);
		delay(50);
		digitalWrite(LED, LOW);
		delay(200);
	}
}