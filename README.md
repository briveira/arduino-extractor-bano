# arduino-extractor-bano
Arduino-based bathroom extractor controller, activated every hour, any time there is movement or when detecting high humidity

This arduino project is being used to control a bathroom extractor. It uses two sensors (DHT11 for humidity and the tipycal PIR for movement, and also a relay to activate the extractor motor) I use it with an arduino nano (ATmega328P, the cheap one). 

## default behaviour

As it does not use an RTC, and the arduino has none, there is no actual way of program a daily schedule, so it will simply start the motor if...

* the extractor has been off for 45 minutes or more
* detects movement
* detects humidity above 90%

While there is high humidity or movement it will stay on and it will last for at least 15 minutes.

## customize configuration

To modify this default values: percentage of humidity >=90, minimum minutes on (15), maximum time off (45), just modify this values before uploading the code to your arduino :

#define PORCENTAJE_HUMEDAD_ALTA 90
#define MINIMO_ENCENDIDO_MILISEGUNDOS (15 * 60 * 1000L)
#define MAXIMO_TIEMPO_APAGADO_MILISEGUNDOS (45 * 60 * 60000L)

It expects the sensors (DHT11, PIR) and relay to use this pins; change them if you need to:

#define dht_apin A1 // analog pin para el detector de humedad
#define pir_dpin 4 // digital pin para el PIR

#define relay_dpin 5 // digital pin para el rele

## libraries

It needs the "DHT_Library.zip" installed on your arduino IDE (there's a copy in the repository)