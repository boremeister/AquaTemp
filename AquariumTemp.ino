
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
* a) try with 3,3V
* b) minimize power consumption -> sleep (mogoèe bi bilo najboljš, da je cel èas v sleep, ko pritisneš gumb se zbudi, prižge screen, zmeri temp in jo pokaže)
*/

// LIBRARIES 
#include <elapsedMillis.h>	// http://playground.arduino.cc/Code/ElapsedMillis
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

// PINS AND DEFINITIONS
#define TEMP_SENS_PIN 6		// temperature sensor
#define BKG_LIGHT 13		// controls backlight
#define BUTTON 8			// button for turning LED backlight ON
#define DEBUG_MODE 1		// DEBUG_MODE mode (1 - on, 0 - off)
OneWire oneWire(TEMP_SENS_PIN);			// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);	// Pass our oneWire reference to Dallas Temperature. 
DeviceAddress Probe01 = { 0x28, 0xEE, 0x80, 0x97, 0x06, 0x00, 0x00, 0x59 };

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);	// LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

// GLOBAL VARIABLES
float temp;
elapsedMillis tempTimer;				// timer for temperature measurement
long int sensorInterval = 5000;			// interval for measuring temperature [ms]
const byte msToSec = 1000;				// conversion ms -> s
int buttonState = 0;					// state of button
elapsedMillis lightTimer;				// timer for temperature measurement
long int lightInterval = 7000;			// interval for measuring temperature [ms]
bool ledBacklightState = false;			// current state of led backlight

void setup()
{

#if defined (DEBUG_MODE)
	Serial.begin(9600);
	sensors.begin();
	Serial.println("Prepare ready!");
	Serial.println("Settings:");
	Serial.println("Sensor timer: " + String(sensorInterval / msToSec) + "s");
	Serial.println("Reading temperature ...");
#endif

	pinMode(BUTTON, INPUT);
	pinMode(BKG_LIGHT, OUTPUT);

	digitalWrite(BKG_LIGHT, HIGH);
	ledBacklightState = true;	// set backlight status variable to on
	lcd.begin(16, 2);
	lcd.clear();
	// title
	lcd.setCursor(0, 0);
	lcd.write("LCD screen ...");
	// content
	lcd.setCursor(0, 1);
	lcd.print("... initialized!");

	// default start delay
	delay(1000);
}

void loop()
{
	
	/*
	* measuer and display temeprature
	*/
	if (tempTimer > sensorInterval){

		tempTimer -= sensorInterval;	// reset timer
		
		// read temperature
		sensors.requestTemperatures(); // Send the command to get temperatures
		// You can have more than one IC on the same bus. 
		// 0 refers to the first IC on the wire
		temp = sensors.getTempCByIndex(0);	// returns 4 decimal places?

		#if defined(DEBUG_MODE)
			Serial.println(temp);
		#endif
	
		/*
		* display data on led
		*/
		lcd.clear();
		// title
		lcd.setCursor(0, 0);
		lcd.print("Temperature:");
		// content
		lcd.setCursor(5, 1);
		lcd.print(temp, 4);
		lcd.setCursor(10, 1);
		lcd.print(" ");
		lcd.print((char)223);	// symbol for degree
		lcd.print("C");
	}

	/* 
	* control LED backlight
	*/
	buttonState = digitalRead(BUTTON);

	if (buttonState == HIGH){
		if (!ledBacklightState){		
			digitalWrite(BKG_LIGHT, HIGH);
			ledBacklightState = true;	// set backlight status variable to on
			#if defined(DEBUG_MODE)
				Serial.println("Turning LED backlight ON!");
			#endif
			lightTimer = 0;
		}
	} else if (lightTimer > lightInterval){
		// turn led backlight off if light interval has passed
		lightTimer -= lightInterval;	// reset timer
		// check if backlight is on
		if (ledBacklightState){
			digitalWrite(BKG_LIGHT, LOW);
			ledBacklightState = false;
			#if defined(DEBUG_MODE)
				Serial.println("Turning LED backlight OFF!");
			#endif
		}
	}
}
