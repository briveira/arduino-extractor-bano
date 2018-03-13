#include <dht.h>

/*=================================
 Bernardo Riveira
 briveira@gmail.com
 2018/03/10
 https://github.com/briveira/arduino-extractor-bano

This arduino project is being used to control a bathroom extractor. 

It uses two sensors (DHT11 for humidity and the tipycal PIR for movement, 
and also a relay to activate the extractor motor) which must be connected 
to the NO (Normally Open) connector.

I use it with an arduino nano (ATmega328P).

It will simply start the motor if...

* the extractor has been off for 45 minutes or more
* detects movement
* detects humidity above 90%

Of course, you can modify those default values.
*/

//=================================
// CONFIGURACION GLOBAL

#define PORCENTAJE_HUMEDAD_ALTA 90

// minutos que estara encendido tras detectar humedad alta, 
// o movimiento

#define MINIMO_ENCENDIDO_MILISEGUNDOS (15 * 60000L)

// minutos que tarda en encender desde el último apagado

#define MAXIMO_TIEMPO_APAGADO_MILISEGUNDOS (45 * 60000L)

// enviar info a la consola

#define DEBUG

//=================================
// conexiones de los sensores

// sensor pinout

#define dht_apin A1 // analog pin para el detector de humedad
#define pir_dpin 4 // digital pin para el PIR

// actuator pinout

#define relay_dpin 5 // digital pin para el rele


//=================================
// setup del rele

void setupRelay(){
  pinMode(relay_dpin, OUTPUT);    
}

// setup del detector de movimiento

void setupPIR() {
  pinMode(pir_dpin, INPUT);    
}

// setup del sensor de humedad

void setupSensorHumedad() {  
}

// setup del puerto serie para depuracion

void setupSerialConsole() {
  Serial.begin(9600);
  delay(500);
  Serial.println("Controlador de ventilador de baño\n\n");
}

// setup general al iniciar el dispositivo

void setup(){

  setupSerialConsole();
  setupPIR();
  setupSensorHumedad();
  setupRelay();

  desactivarVentilador();

}

//=================================
// activar/desactivar ventilador
// el rele se usa en la conexion "Normally open"
// es decir, el estado HIGH provoca el cierre del circuito

bool estadoVentilador = false;

// instante en que se encendio/apago por ultima vez

unsigned long int msInstanteUltimoEncendido = 0;
unsigned long int msInstanteUltimoApagado = 0;

void activarVentilador() {
  if (!estadoVentilador) {
    estadoVentilador = true;
    digitalWrite(relay_dpin, HIGH);
    msInstanteUltimoEncendido = millis();
  }
}

void desactivarVentilador() {
  if (estadoVentilador) {
    estadoVentilador = false;
    digitalWrite(relay_dpin, LOW);
    msInstanteUltimoApagado = millis();
  }
}

//=================================
// gestion de movimiento

// instante en que se detectó movimiento 
// por ultima vez

unsigned long msInstanteUltimoMovimiento = 0;

// ultimo estado detectado

bool ultimoEstadoMovimiento = false;

// deteccion de movimiento, actualiza instante ultimo
// devuelve y mantiene variable global con estado actual

bool deteccionMovimiento() {
  ultimoEstadoMovimiento = digitalRead(pir_dpin)==HIGH;
  if (ultimoEstadoMovimiento) {
    msInstanteUltimoMovimiento = millis();
  }
  return ultimoEstadoMovimiento;
}

//=================================
// gestion de humedad del aire

// libreria de gestion del sensor

dht DHT;

// instante en que se detectó humedad alta por ultima vez

unsigned long msInstanteUltimaHumedadAlta = 0;

// ultimo % de humedad detectado

int ultimoPorcentajeHumedad = 0;

// ultimo estado de humedad alta

bool ultimoEstadoHumedadAlta = false;

// calcula el estado de humedad, detecta si hubo 
// humedad alta

bool deteccionHumedad() {

  DHT.read11(dht_apin); 
  ultimoPorcentajeHumedad = DHT.humidity;
  ultimoEstadoHumedadAlta = (ultimoPorcentajeHumedad >= PORCENTAJE_HUMEDAD_ALTA);
  if (ultimoEstadoHumedadAlta) {
    msInstanteUltimaHumedadAlta = millis();    
  }  
}

//=================================

void debug() {
  Serial.println("==========================================");
  Serial.print("instante actual = "); Serial.println(millis());
  
  Serial.print("ventilador activo = "); Serial.println(estadoVentilador);
  Serial.print("ventilador ultima activacion = "); Serial.println(msInstanteUltimoEncendido);
  Serial.print("ventilador ultima desactivacion = "); Serial.println(msInstanteUltimoApagado);

  Serial.print("movimiento detectado = "); Serial.println(ultimoEstadoMovimiento);
  Serial.print("movimiento ultima deteccion = "); Serial.println(msInstanteUltimoMovimiento);

  Serial.print("humedad actual (%) = "); Serial.println(ultimoPorcentajeHumedad);
  Serial.print("estado humedad ALTA = "); Serial.println(ultimoEstadoHumedadAlta);
  Serial.print("ms ultima deteccion humedad ALTA = "); Serial.println(msInstanteUltimaHumedadAlta);
}

//=================================
// sistema de gestion del ventilador
// decide si debe encenderlo o no en función del estado de los sensores
// y el tiempo que lleva encendido

void gestionVentilador() {

  unsigned long int instante = millis(); // instante actual

  if (!estadoVentilador) { // si está apagado...

    // ¿hay humedad o movimiento o ha estado apagado más de lo permitido?
    
    if (ultimoEstadoMovimiento || ultimoEstadoHumedadAlta 
        || (instante - msInstanteUltimoApagado) > MAXIMO_TIEMPO_APAGADO_MILISEGUNDOS) {
      // debemos encenderlo
      activarVentilador(); 
    }
    
  } else {

    // si está encendido y...
    //  - hace tiempo del ultimo movimiento y
    //  - hace tiempo de la ultima deteccion de humedad alta y
    //  - hace tiempo de la ultima vez que lo pusimos en marcha

    if ( (instante - msInstanteUltimoMovimiento) > MINIMO_ENCENDIDO_MILISEGUNDOS 
     && (instante - msInstanteUltimaHumedadAlta) > MINIMO_ENCENDIDO_MILISEGUNDOS
     && (instante - msInstanteUltimoEncendido) > MINIMO_ENCENDIDO_MILISEGUNDOS) {
        // lo apagamos
        desactivarVentilador();
      }
  }
    
}

//=================================

void loop(){

    delay(2000);

    deteccionMovimiento();
    deteccionHumedad();

    gestionVentilador();
    
#ifdef DEBUG
    debug();
#endif
     
}
