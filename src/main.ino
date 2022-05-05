/*
          _____________________________
         |  ATMEGA >>>   SIM800L  |
          -----------------------------
              GND      >>>   GND
          RX  8       >>>   TX
          TX  7       >>>   RX
         RESET 6       >>>   RST
         LED ESTADO 3
         LED ROJO   11
         LED VERDE  10
         SWICH PROGRAMACION 13
         PULSA PROGRAMACION 12
         PULSA CAMBIO ESTADO 2
         SENSOR RUIDO A3
         EPROM SCL A5
         EPROM SDA A4
         RELE 4


                                        ////////////////////////METODOS
  void begin();
  void reset();

  // Methods for calling || Funciones de llamadas.
  bool answerCall();
  void callNumber(char* number);
  bool hangoffCall();
  uint8_t getCallStatus();
  //Methods for sms || Funciones de SMS.
  bool sendSms(char* number,char* text);
  String readSms(uint8_t index); //return all the content of sms
  String getNumberSms(uint8_t index); //return the number of the sms..
  bool delAllSms();     // return :  OK or ERROR ..

  void signalQuality();
  void setPhoneFunctionality();
  void activateBearerProfile();
  void deactivateBearerProfile();



*/
//#define DEBUG             // activa debut
#define OPTOPC817         // se activa por opto si se activa sino por ruido
#define ESP01             // comunciacion por el serial con el esp01
//#define COMANDOSATSIM     // envia al sim los comandos introducidos por el serial

#include "Arduino.h"
#include "definicionesSim.h"
#include <SoftwareSerial.h> // para comunicarse con el modulo



SoftwareSerial SIM (RX_PIN, TX_PIN);

bool estado = true;
/////////////// funciones tiempo
unsigned long ExTimer;
byte CountSg = 0;
byte CountMsg = 0;
byte segundosInicio = 1;
byte segundos = 0;
byte segundosCorrecionError = 0; // corregir la variable que se queda pillada al hacer trasicion de cambio estado;
/////////////////////////////////

////////////////// telefonos y llamadas
bool llamandaPerdida = false;
int segundosLlamada = 0;
bool llamando = false;
byte intentosLlamadas = 0;  // para establecer las llamadas hechas a los diferentes tels
const byte excesoTiempoLlamada = 180; // tiempo maximo llamada
byte segundosLlamadaEntrante = 0;
byte vAux = 0;
bool entrandoLlamada = false;


/////////////////// sensor ruido
unsigned int consginaReposo = 0;// establece el sensor de ruido en silencio
unsigned int consignaRuido = 0;        // establece sensor ruido con llamada
int segundosRuido = 0;                // para controlar que el sensor de ruido capte ruido al menos 2 segundos y que no se active por qualquier otro ruido
byte contadorRuido = 0;
int saltosPorEncima = 0;

//////////////////// programacion modulo
byte contadorTiempoRespiro = 0;
const byte tiempoRespiro = 7;          // variables para que las comunicaciones entra el atmega y sim vayan descansando y no se acomulen
byte pasosProgramacion = 0;
byte pasosProgramacionAudio = 0;
byte contadorTiempoRespiroAudio = 0;
const byte tiempoRespiroAudio = 3;
byte segundoPrueba = 0;

void setup() {

  ExTimer = millis();   ///////////funcion tiempo
  iniciaModulo();
  while (comprobarComunicacion() == false);

#ifdef esp01
  iniciaComunicacion();
#endif

}

void loop() {
  funcionTiempo();


#ifdef COMANDOSATSIM || ESP01
  leerSerial ();
#endif



  /////////////////////////////////////////////////////////////////// // el programa detecta el swich d programar se inicia el la programacion
  if (leerSwichProgramacion() == 0) {
    programarModulo();
  }



  else {
    ///////////////////////////////////////////////////////////////////////// si no se programa se ejecuta el programa de forma normal
    if (leerEstado() == 0) {  // cambio estado por pulsador
      cambioEstado();
    }

#ifdef OPTOPC817
    if (estado == false) {       // estado activado
      if (((llamandaPerdida == false) && (llamando == false) && (entrandoLlamada == false))) {
        if (leerOpto () == 1) { //  funcion que recibe 1 cuando se activa
          llamar(1);
        }
      }
    }
#else

    if (estado == false) {       // estado activado
      if (((llamandaPerdida == false) && (llamando == false) && (entrandoLlamada == false))) {
        if (ruidoOn () == true) { //  funcion que mira si el sensor de ruido recibe ruido durante dos segundos
          llamar(1);
        }
      }
    }
#endif
    //////////////////////////////////////////////// comprobar cuantas lecturas hace por segundo
    //    int sensorValue = leerRuido ();
    //    if (sensorValue > consignaRuido) {
    //      Serial.println(sensorValue);
    //      saltosPorEncima++;
    //      Serial.print(F("saltos por encima en 1 s ")); Serial.println (saltosPorEncima);
    //    }

    ///////////////////////////////////////////////////////////////////////////////////

  }


  if (llamando) {                           /////////////////////////// para ajustar nivel de mic y altavos durante la llamada
    if (leerPulsadorProgramacion() == 0) {
      pasosProgramacionAudio++;
    }
    if (pasosProgramacionAudio > 0) {
      programarAudio() ;
    }
  }



}



void LoopNewSecond() {
  ////////////////////////////INICIO SECUENCIAL DEL PROGRAMA
  if ((segundosInicio > 0) && (segundosInicio < 25)) {
    segundosInicio++;
    if (segundosInicio == 2) {
      inciaPerifericos();
    }
    else if (segundosInicio == 5) {
      configuraModulo();
      rele(true);
    }

    else if (segundosInicio == 7) {
      recuperaNivelMic();
      recuperarNivelAltavoz();
      rele(false);
    }

    else if (segundosInicio == 9) {
      recuperaSensorRuido();
    }

    if (segundosInicio > 12) {
      if (comprobarCobertura() == false) { // hace 10 intentos cada 10 segundos para comprobar cobertura y en caso de fallo reinicia el modulo
        segundosInicio++;
      }
      else {                // cobertura ok
        segundosInicio = 0;
        calidadSenal();
        cambioEstado ();          //el dispositivo arranca en modo llamada si detecta ruido

      }
    }
  }


  /////////////////////////////////////////////////////////////////// GESTION LLAMADAS PERDIDAS
  if ((segundosLlamada > 0) && (llamandaPerdida == true)) {
    segundosLlamada++;
#ifdef DEBUG
    Serial.print (F("dentro newSecond llamadas perdidas, segundos = ")); Serial.println (segundosLlamada);
#endif
    if (segundosLlamada == 10) {
      colgar();
      contadorTiempoRespiro = 1; //dar tiempo para que pase la siguiente paso
      segundosLlamada = 0 ;
      segundosLlamadaEntrante = 0;
    }
  }

  if (segundosInicio == 0) {
#ifdef DEBUG
    //        Serial.print (F("estado llamada ")); Serial.println (estadoLlamada());
#endif
  }



  ///////////////////////////////////////////////////////////////GESTION LLAMADAS        // el sim espera 12s para que se descuelgue o corta llamada
  if ((segundosLlamada > 0) && (llamando == true)) {
    //   Serial.print (" segundos Llamada "); Serial.println (segundosLlamada);
    segundosLlamada ++;

    if (segundosLlamada > 4) {
#ifdef DEBUG
      //  Serial.print (F("estado llamada ")); Serial.println (estadoLlamada());
#endif
      if (intentosLlamadas == 1) {                 // se esta llamando al 1r tel
        // Serial.print (F("intentos llamada ")); Serial.println (intentosLlamadas);
        if (( segundosLlamada < 26) && (estadoLlamada() == 0)) {     ////// llamada se ha colgado porque el sim solo intenta llamar 12s
          colgar();
          segundosLlamada = 1;
          llamar(2);
        }
        else  if (( segundosLlamada > 25) && (estadoLlamada() == 0)) {       ////// se ha establecido llamada y no llamos al tel 2
#ifdef DEBUG
          Serial.println (F("llamada al tel 1 finalizada"));
#endif
          colgar();
        }
      }

      else if (intentosLlamadas == 2) {       ///////// sse esta llamando al 2 tel
        if (( segundosLlamada < 26) && (estadoLlamada() == 0)) {         ////// llamada se ha colgado

#ifdef DEBUG
          Serial.println (F("no ha habido conexion a ningun tel"));
#endif
          colgar();
        }
        else  if (( segundosLlamada > 25) && (estadoLlamada() == 0)) {       ////// se ha establecido llamada al segundo tel
#ifdef DEBUG
          Serial.println (F("llamada al tel 2 finalizada"));
#endif
          colgar();
        }
      }


      if (segundosLlamada == 7) {        // 7 segundos activo rele
        rele (true);
      }

      if (segundosLlamada == excesoTiempoLlamada) {
#ifdef DEBUG
        Serial.println (F("llamada exece tiempo maximo"));
#endif
        colgar();
      }
    }
  }

  /////////////////////////////////////////////////// LLAMADA ENTRANTE Y CAMBIO DE ESTADO
  //
  // #ifdef DEBUG
  //    Serial.print(F("segundos prueba ")); Serial.println(segundoPrueba);
  //  #endif
  //    segundoPrueba++;
  //    if (segundoPrueba >30){
  /////////////////////// corregir error variable entrada llamada pillada
#ifdef DEBUG
  // Serial.print(F("segundos llamada correccion errors = ")); Serial.println(segundosLlamadaEntrante);
#endif

/*
  if (segundosCorrecionError > 0) {
    segundosCorrecionError++;
  }
    if ((segundosCorrecionError == 20) && (segundosLlamadaEntrante > 0)) {
#ifdef DEBUG
      Serial.print(F("funcion que corrige error de variable pillada, segundosCorrecionError =  ")); Serial.println(segundosCorrecionError);
#endif
      colgar();
      segundosCorrecionError = 0;
    }
    else if ((segundosCorrecionError == 20) && (segundosLlamadaEntrante == 0)) {
      segundosCorrecionError = 0;
#ifdef DEBUG
      Serial.print(F("funcion que corrige error de variable pillada no ha hecho falta, segundosCorrecionError =  ")); Serial.println(segundosCorrecionError);
#endif
    }
 */


  if (segundosInicio == 0) {  // para que se ejecute una vez el programa haya arrancado
    if ((llamandaPerdida == false) && (llamando == false)) {
      if ((pasosProgramacion == 10) || (pasosProgramacion == 0)) {   //mientras se programa el modulo no funciona
        if ((llamadaEntrante() == 3 ) && (entrandoLlamada == false))  {
#ifdef DEBUG
          Serial.println(F("llamada entrante, apago led y estado llamada es = 3"));// Serial.println(segundosLlamadaEntrante);
#endif
 /*         segundosCorrecionError = 1 ;
#ifdef DEBUG
          Serial.print(F("funcion que corrige error de variable pillada, segundosCorrecionError =  ")); Serial.println(segundosCorrecionError);
#endif*/
          digitalWrite (LED_ESTADO, LOW); // apagmos el led de estado
          segundosLlamadaEntrante = 1;
          entrandoLlamada = true;              // para que no vuleva a entrar si detecta otro ring
        }



        if (segundosLlamadaEntrante > 0) {
        /*  segundosCorrecionError = 1 ;
#ifdef DEBUG
     //     Serial.print(F("funcion que corrige error de variable pillada, segundosCorrecionError =  ")); Serial.println(segundosCorrecionError);
#endif*/
/*#ifdef DEBUG
          Serial.print(F("segundos llamada entrante segundos llamada > 0 variable  =  ")); Serial.println(segundosLlamadaEntrante);
#endif*/
          segundosLlamadaEntrante ++;

          if (segundosLlamadaEntrante == 2) {
            String x = comprobarNumeroEntrante();
            vAux = buscarCoincidenciaNumero (x); // guarda en variable auxiliar el valor del reconocimineto de llamada para su posterior envio a metodo
            //#ifdef DEBUG
            //        Serial.print(F("llamada entrante ")); Serial.println(x);
            //#endif
          }
          if (segundosLlamadaEntrante > 2) {
            int x = pruebaEntrante();
#ifdef DEBUG
    //        Serial.print (F("segundos llamada entrante > 2 , estado llamada = ")); Serial.println (x);
#endif
            if (x == 0 ) {
#ifdef DEBUG
              Serial.print(F("hemos tenido llamada entrante y enviamos datos a metodo, segundos llamada ")); Serial.println(segundosLlamadaEntrante);
              Serial.print(F(" valor de coincidencia ")); Serial.println(vAux);
#endif
              cambioEstadoPorLlamada (segundosLlamadaEntrante, vAux);
              segundosLlamadaEntrante = 0;
              digitalWrite (LED_ESTADO, HIGH); // encendemos el led de estado
              vAux = 0;
              entrandoLlamada = false;
            }
          }
        }
      }
    }
  }
  //    }


  /////////////////////////////////////////////////PORGRAMACION MODULO Y AUDIO
  if (contadorTiempoRespiro > 0) {                 //////////   metodo que hace las comunicaciones entre el sim vayan respondiendo
    contadorTiempoRespiro ++;
    //  Serial.print ("tiempo espiro ");Serial.println(contadorTiempoRespiro);
    if (contadorTiempoRespiro == tiempoRespiro) {
      contadorTiempoRespiro = 0;
    }
  }

  if (contadorTiempoRespiroAudio > 0) {                 //////////   metodo que hace las comunicaciones entre el sim vayan respondiendo cuando se ajusta el auido y mic
    contadorTiempoRespiroAudio ++;
    //  Serial.print ("tiempo espiro ");Serial.println(contadorTiempoRespiro);
    if (contadorTiempoRespiroAudio == tiempoRespiroAudio) {
      contadorTiempoRespiroAudio = 0;
    }
  }



}
