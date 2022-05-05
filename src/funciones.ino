#include <EEPROM.h>



int intentos = 0;       //  variable que guarda los diferentes intentos al inicar modulo


int _timeout;
String _buffer;




int eeAddress = 0;
int aux = 0; // variable auxiliar
unsigned int aux2 = 0;

String telefono1 = "";
String telefono2 = "";

const int numCaptacionRuido = 40 ; // numero de veces que copiara el valor dell ruido                  SEGUN CALCULOS EN 1S HAY 30 CAPTACIONES POR ENCIMA DE CONSOGNA
const int numRuidoActivar  = 7;  // poner a 5 numero de veces que el ruido tiene que ser superior a consigna
const int diferenciaRuido = 110;  // medida de mas tomada por el sensor ruido en ruido
const int ajustarAlaBajaSensor = 30; // para que este un poco por debajo de la media



void iniciaModulo() {

#ifdef DEBUG
  Serial.begin(9600);
  Serial.println (F("inicio programa"));
#endif
#ifdef COMANDOSATSIM || ESP01
  Serial.begin(9600);
#endif
  SIM.begin(9600);
  _buffer.reserve(255); //reserve memory to prevent intern fragmention
  // borrarSMS();            // borrar la memoria de los sms
}





void inciaPerifericos () {
  pinMode (LED_ESTADO, OUTPUT);
  pinMode (LED_VERDE, OUTPUT);
  pinMode (LED_ROJO, OUTPUT);
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, HIGH);
  pinMode (RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);
  pinMode (SWICH_PROGRAMACION, INPUT);
  pinMode (PULSA_PROGRAMACION, INPUT);
  pinMode (PULSA_CAMBIO_ESTADO, INPUT);
  pinMode (POTENCIO, INPUT);
  pinMode (RELE, OUTPUT);
  digitalWrite (RELE, LOW);
#ifdef OPTOPC817
  pinMode (PC817, INPUT);
#else
  pinMode (SENSOR_RUIDO, INPUT);
#endif
}



bool comprobarComunicacion() {
  int a = -1;
#ifdef DEBUG
  Serial.println (F("esperando respuesta modulo"));
#endif
  SIM.println("AT");   //Configuramos el módulo para trabajar con SMS
  if (leerSim().indexOf("OK") != -1 ) {
    intentos = 0;
#ifdef DEBUG
    Serial.println (F("modulo responde ok!"));
#endif
    segundosInicio = 1;                 //iniciamos la configurancion secuencial del modulo
    return true;
  }
  else
    intentos++;
#ifdef DEBUG
  Serial.println (F("fallo al responder modulo"));
#endif
  delay(200);
  if (intentos == 10) {
#ifdef DEBUG
    Serial.println (F("10 intentos comunicacion fallidos"));
#endif
    intentos = 0;
  }
  return false;
}


void configuraModulo() {
#ifdef DEBUG
  Serial.println (F("\ncomandos que configuran el modulo"));
#endif
  SIM.println("AT+CMGF=1");   //Configuramos el módulo para trabajar con SMS
#ifdef DEBUG
  Serial.println(leerSim());
#endif
  SIM.println("AT+CNMI=2,2,0,0,0");  // configura el modulo para que muestre por el serial los mensajes recibidos
#ifdef DEBUG
  Serial.println(leerSim());
#endif
  SIM.println("AT+CLIP=1");   //Configuramos para identificar la llamada
#ifdef DEBUG
  Serial.println(leerSim());
#endif
  SIM.println("AT+CALS=5,0");   // desactivar tono llamada     5,1 TONO ACTIVO    5,0 TONO OFF
#ifdef DEBUG
  Serial.println(leerSim());
#endif
  SIM.println("AT+CRSL=0");   // BAJA EL TONO DEL RING A MINIMO
#ifdef DEBUG
  Serial.println(leerSim());
#endif
}

/*
  void ajustarAudicion() {
  #ifdef DEBUG
  Serial.println(F("\n mic y altavoz"));
  #endif
  // recuperaNivelMic();
  // recuperarNivelAltavoz();
  //  SIM.println("AT+CMIC=0,01");// ajustar nivel de microfono  AJUSTAR DE 0-15
  //#ifdef DEBUG
  //  Serial.println(leerSim());
  //#endif
  //  SIM.println("AT+CLVL=40");   // ajustar volumen altavoz   AJUSTAR 0-100
  //#ifdef DEBUG
  //  Serial.println(leerSim());
  //#endif
  //  SIM.println("AT+CALS=5,0");   // desactivar tono llamada     5,1 TONO ACTIVO    5,0 TONO OFF
  //#ifdef DEBUG
  //  Serial.println(leerSim());
  //#endif
  }
*/
bool comprobarCobertura () {
#ifdef DEBUG
  Serial.println(F("\ncomprobar cobertura"));
#endif
  SIM.println("AT+CREG?");
  if (leerSim().indexOf("+CREG: 0,1") != -1 ) {  // cambiar a !

#ifdef DEBUG
    Serial.println (F("cobertura correcta!"));
#endif
    intentos = 0;
    digitalWrite (LED_ESTADO, HIGH);
    return true;
  }
  else {
#ifdef DEBUG
    Serial.println (F("fallo de cobertura"));
#endif
    delay(5000);             // esperar segundos 5 para que coja cobertura
    intentos ++;
#ifdef DEBUG
    Serial.print (F("intento ")); Serial.println (intentos);
#endif
    if (intentos == 5) {
#ifdef DEBUG
      Serial.println (F("5 intentos cobertura fallidos"));
#endif
      intentos = 0;
      resetSIM();
    }
    // mensaje = "";
    return false;
  }
  /*

    Possible values of registration status are,
    0 not registered, MT is not currently searching a new operator to register to
    1 registered, home network
    2 not registered, but MT is currently searching a new operator to register to
    3 registration denied
    4 unknown (e.g. out of GERAN/UTRAN/E-UTRAN coverage)
    5 registered, roaming
    6 registered for "SMS only", home network (applicable only when indicates E-UTRAN)
    7 registered for "SMS only", roaming (applicable only when indicates E-UTRAN)
    8 attached for emergency bearer services only (see NOTE 2) (not applicable)
    9 registered for "CSFB not preferred", home network (applicable only when indicates E-UTRAN)
    10 registered for "CSFB not preferred", roaming (applicable only when indicates E-UTRAN)

  */
}

void calidadSenal() {
  /*Response
    +CSQ: <rssi>,<ber>Parameters
    <rssi>
    0 -115 dBm or less
    1 -111 dBm
    2...30 -110... -54 dBm
    31 -52 dBm or greater
    99 not known or not detectable
    <ber> (in percent):
    0...7 As RXQUAL values in the table in GSM 05.08 [20]
    subclause 7.2.4
    99 Not known or not detectable
  */
#ifdef DEBUG
  Serial.println (F("calidad señal"));
  SIM.println ("AT+CSQ");
  Serial.println(leerSim());
#endif
}

void resetSIM() {
#ifdef DEBUG
  Serial.println (F("reset modulo"));
#endif
  digitalWrite (RESET_PIN, LOW);
  delay(1000);
  digitalWrite (RESET_PIN, HIGH);
  comprobarCobertura();
  segundosInicio =  10;
}

bool comprobarError () {
  if ((segundosCorrecionError > 0) || (segundosLlamadaEntrante > 0)) {
#ifdef DEBUG
    Serial.println(F("corregimos loop pillado"));
#endif
    return true;
  }
  else
    return false;
}

void mostrarVariables() {
#ifdef DEBUG
  Serial.print(F("timeout = ")); Serial.print(_timeout);
  Serial.print(F("\testado = ")); Serial.print(estado);
  Serial.print(F("\tsegundosInicio = ")); Serial.print(segundosInicio);
  Serial.print(F("\tsegundos = ")); Serial.print(segundos);
  Serial.print(F("\tsegundosCorrecionError = ")); Serial.print(segundosCorrecionError);
  Serial.print(F("\tllamandaPerdida = ")); Serial.print(llamandaPerdida);
  Serial.print(F("\tsegundosLlamada = ")); Serial.print(segundosLlamada);
  Serial.print(F("\tintentosLlamadas = ")); Serial.print(intentosLlamadas);
  Serial.print(F("\texcesoTiempoLlamada = ")); Serial.print(excesoTiempoLlamada);
  Serial.print(F("\tsegundosLlamadaEntrante = ")); Serial.print(segundosLlamadaEntrante);
  Serial.print(F("\tvAux = ")); Serial.print(vAux);
  Serial.print(F("\tentrandoLlamada = ")); Serial.print(entrandoLlamada);
  Serial.print(F("\tsegundosLlamada = ")); Serial.print(segundosLlamada);
  Serial.print(F("\tcontadorTiempoRespiro = ")); Serial.print(contadorTiempoRespiro);
  Serial.print(F("\ttiempoRespiro = ")); Serial.print(tiempoRespiro);
  Serial.print(F("\tsegundoPrueba = ")); Serial.println(segundoPrueba);
#endif
}

String leerSim() {
  _timeout = 0;
  while  (!SIM.available() && _timeout < 12000 )  //12000
  {
    delay(13);
    _timeout++;
    if (_timeout == 11000) {
#ifdef DEBUG
      Serial.println(F("timeout a 11000"));
#endif
      mostrarVariables();
    }
    if (_timeout == 11999) {
#ifdef DEBUG
      Serial.println(F("timeout a 11999 resetamos variables"));
#endif
      ledParpadea(7);
      digitalWrite (LED_ROJO, HIGH);
      _timeout = 0;
      colgar();
      mostrarVariables();
      //   segundosInicio = 1; // inciamos programa
    }
#ifdef DEBUG
    //  Serial.println(F("pillado loop"));
#endif
    if (leerPulsadorProgramacion() == 0) {
      pasosProgramacion++;
      //  Serial.print(".");
      return "PULSADOR PASA PROGRAMA";
    }

  }
  if (SIM.available()) {
    return SIM.readString();
  }

}


void leerSerial() {
  String entrada = "";
  if (Serial.available() > 0) {
    entrada = (Serial.readString());
    if ( entrada.indexOf("AT") != -1) {               // leer en el serial un comando at y por tanto lo envia al sim
#ifdef COMANDOSATSIM
      SIM.println(entrada);
#endif
#ifdef DEBUG
      Serial.println(leerSim());
#endif
    }
    else {
#ifdef ESP01
      compruebaMqtt (entrada);
#endif
    }
  }
}




//////////////////////////////////////////////////////////////////////////////////PULSADORES Y SWICHT
int leerOpto () {                       // tiene pull down
  int a = 0;
  a = digitalRead (PC817);
  if (a == 1) {
#ifdef DEBUG
    Serial.println(F("optoacoplador activa modulo"));
#endif
    delay(100);
  }
  return a;
}

int leerRuido () {
  int sensor = analogRead(SENSOR_RUIDO);
  return sensor;
}

int leerPotenciometro () {
  int poten = analogRead(POTENCIO);
  poten = map(poten, 0, 1023, 0, 10);
  return poten;
}

int leerEstado () {
  int a = 1;
  a = digitalRead (PULSA_CAMBIO_ESTADO);
  if (a == 0) {
#ifdef DEBUG
    Serial.println(F("pulsador cambia estado"));
#endif
    delay(2000);
  }
  return a;
}

void cambioEstado () {
  if (estado == true) {                              // estado activado paso a desactivar
    digitalWrite (LED_VERDE, LOW);
    digitalWrite (LED_ROJO, HIGH);
    estado = false;
#ifdef ESP01
    enviaMqtt (true);
#endif
#ifdef DEBUG
    Serial.println(F("cambio estado a activado"));
#endif
  }
  else {                                       // estado desactivado paso a activar
    digitalWrite (LED_VERDE, HIGH);
    digitalWrite (LED_ROJO, LOW);
    estado = true;
#ifdef ESP01
    enviaMqtt (false);
#endif
#ifdef DEBUG
    Serial.println(F("cambio estado a desactivado"));
#endif
  }
}

int leerSwichProgramacion () {
  int a = 1;
  a = digitalRead (SWICH_PROGRAMACION);
  if (a == 0) {
    // delay(500);
  }
  else {
    pasosProgramacion = 0;
  }
  return a;
}
///////////////////////////////////////////////////////////////////////////////////////////

int leerPulsadorProgramacion () {
  int a = 1;
  a = digitalRead (PULSA_PROGRAMACION);
  if (a == 0) {
    delay(1500);
#ifdef DEBUG
    Serial.println(F("añado un paso a la programacion"));
#endif
  }
  return a;
}

void programarAudio() {
  if (pasosProgramacionAudio == 1) {
    borrarSMS();
#ifdef DEBUG
    Serial.println(F("iniciamos la programacion del mic\nBorramos memoria de sms en la sim para no acomular"));
#endif
    aux = 0;
    ledParpadea(2);
    digitalWrite (LED_ROJO, LOW);
    digitalWrite (LED_VERDE, HIGH);
    pasosProgramacionAudio = 2;
  }

  else if ((pasosProgramacionAudio == 2) && (contadorTiempoRespiroAudio == 0)) {
    aux =  leerPotenciometro();
    contadorTiempoRespiroAudio = 1;
    pasosProgramacionAudio = 3;
  }

  else if ((pasosProgramacionAudio == 3) && (contadorTiempoRespiroAudio == 0)) {
    if (aux != leerPotenciometro()) {
      aux = leerPotenciometro();
      int x = (aux * 15) / 10;

#ifdef DEBUG
      Serial.print(F("valor del potenciometro ")); Serial.println (aux);
      Serial.print(F("valor a ajustar mic ")); Serial.println (x);
#endif
      ledRojoParpadea (aux);
      if (x < 10) { // condicidion que si no se cumple pone un 0 delante de int
        SIM.print("AT+CMIC=0,0"); SIM.println(x); // ajustar nivel de microfono  AJUSTAR DE 0-15
      }
      else {
        SIM.print("AT+CMIC=0,"); SIM.println(x); // ajustar nivel de microfono  AJUSTAR DE 0-15
      }
#ifdef DEBUG
      Serial.println(leerSim());
#endif
      grabarNivelMicAltavoz (x, 0);
      contadorTiempoRespiroAudio = 1;
      pasosProgramacionAudio = 2;
    }
  }

  else if ((pasosProgramacionAudio == 4) && (contadorTiempoRespiroAudio == 0)) {
#ifdef DEBUG
    Serial.println(F("iniciamos la programacion del altavoz"));
#endif
    ledParpadea(2);
    digitalWrite (LED_ROJO, HIGH);
    digitalWrite (LED_VERDE, LOW);
    aux =  leerPotenciometro();
    contadorTiempoRespiroAudio = 1;
    pasosProgramacionAudio = 5;
  }

  else if ((pasosProgramacionAudio == 5) && (contadorTiempoRespiroAudio == 0)) {
    if (aux != leerPotenciometro()) {
      aux = leerPotenciometro();
      int y = aux * 10; // multiplica por 10 para adecuar el comando de altavoz
#ifdef DEBUG
      Serial.print(F("valor del potenciometro ")); Serial.println (aux);
      Serial.print(F("valor a ajustar altavoz ")); Serial.println (y);
#endif
      ledVerdeParpadea (aux);
      SIM.print("AT+CLVL="); SIM.println(y); // ajustar volumen altavoz   AJUSTAR 0-100
#ifdef DEBUG
      Serial.println(leerSim());
#endif
      grabarNivelMicAltavoz (y, 1);
      contadorTiempoRespiroAudio = 1;
      pasosProgramacionAudio = 4;
    }
  }

  else if (pasosProgramacionAudio == 6) {
#ifdef DEBUG
    Serial.println(F("Finalizamos la programacion de altravoz y mic"));
#endif
    ledParpadea(4);
    digitalWrite (LED_ROJO, HIGH);
    aux = 0;
    pasosProgramacionAudio = 0; // fuera del
  }

}



void programarModulo() {

  /////////////////////////////////////////////////////////////////////////////////////////////////////INICIA PROGRAMACION
  if (pasosProgramacion == 0) {////////////////////////////////////////////////////////////////////// 2 LEDS APAGADOS
#ifdef DEBUG
    //  Serial.println("funcion leer swicht programacion");
    Serial.println(F("paso 1\nesperando sms o pulsador"));
#endif
    digitalWrite (LED_ROJO, LOW);
    digitalWrite (LED_VERDE, LOW);
    pasosProgramacion = 1;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////PASO  1 ESPERANDO SMS PARA GRABAR O BORRAR TEL EN LA SIM
  else if ((pasosProgramacion == 1) && (contadorTiempoRespiro == 0)) { //////////////////////////////// SI SE PULSA PULSADOR SE PASA AL SIGUIENTE PASO y SALTA PERDIDAS

    String  recibido = smsRecibido();
    if (recibido.indexOf("borrar") != -1) {
#ifdef DEBUG
      Serial.println (F("borra tel sim"));
#endif
      ledParpadea(4);
      grabarSim("borrar", 3);
      pasosProgramacion = 0;
    }

    else if (recibido.indexOf("tel1") != -1) {
      int x = recibido.indexOf("tel1");        //buscamos posicion de tel1
      int i = recibido.length();                // buscamos tamaño total de la string
      telefono1 = recibido.substring(x + 4, x + 15);        // guardamos en string del tel el telefono
#ifdef DEBUG
      Serial.println (F("recibido tel 1"));
      Serial.print (F("tel memorizado es   ")); Serial.println(telefono1);
#endif
      telefono2 = recibido.substring(x + 19 , x + 30);    // guardamos en string del tel el telefono
#ifdef DEBUG
      Serial.println (F("recibido tel 2"));
      Serial.print (F("tel memorizado es   ")); Serial.println(telefono2);
#endif
      digitalWrite (LED_ROJO, HIGH);
      digitalWrite (LED_VERDE, LOW);
      grabarSim (telefono1, 1);            //funcion que graba los tel en la sim
      contadorTiempoRespiro = 1;                   // para dar un tiempo con las gravaciones en la sim
      pasosProgramacion = 44;               // solo este paso grava el segundo  tel en la sim

    }
    else if (recibido.indexOf("PULSADOR PASA PROGRAMA") != -1) {
      pasosProgramacion = 7;/////////////////////////////////////////////////////////////////////////////PASAMOS A PROGRAMAR SENSOR RUIDO
    }
    else if (recibido.length() > 2) { //se recibe un sms pero no es correcto
#ifdef DEBUG
      Serial.println (F("sms erronero"));
#endif
      pasosProgramacion = 7;/////////////////////////////////////////////////////////////////////////////PASAMOS A PROGRAMAR SENSOR RUIDO
      ledParpadea(6);
    }
    //  borrarSMS();
  }

  else if ((pasosProgramacion == 44) && (contadorTiempoRespiro == 0)) {                    //paso que solo graba el segundo tel en la sim y da tiempo a no colapsar
    if (telefono2.length() == 11) {             // si hay segundo telefono guardado y por tanto si grabamos
      grabarSim (telefono2, 2);
      digitalWrite (LED_ROJO, LOW);
      digitalWrite (LED_VERDE, HIGH);
    }
    contadorTiempoRespiro = 1;
    pasosProgramacion = 4;
  }

  else if ((pasosProgramacion == 4 ) && (contadorTiempoRespiro == 0)) {
#ifdef DEBUG
    Serial.println(F("paso 4\nPerdidas a tels"));
#endif
    contadorTiempoRespiro = 1;
    pasosProgramacion = 5;

  }
  else if ((pasosProgramacion == 5) && (contadorTiempoRespiro == 0)) {

    digitalWrite (LED_VERDE, HIGH);
    digitalWrite (LED_ROJO, HIGH);
    perdidaTel(1);
    // Serial.println("perdida tel 1");
    contadorTiempoRespiro = 1;
    pasosProgramacion = 6;
  }

  else if ((pasosProgramacion == 6 ) && (contadorTiempoRespiro == 0)) {
    ///////////////////////////////////////////////////////////////////////////////////  //////////// paso que espera que las perdidas
    if (segundosLlamada == 0) {
      if (llamandaPerdida == false) {                              // la llamada anterior ha acabado
        if (telefono2.length() == 11) { // hay telefono y por tanto hacemos perdida
          perdidaTel(2);
        }
        pasosProgramacion = 7;
        contadorTiempoRespiro = 1;
      }
    }
    else {
      contadorTiempoRespiro = 1;
    }
  }



  else if (((pasosProgramacion == 7 ) && (contadorTiempoRespiro == 0) && (segundosLlamada == 0))) { ////////////////////////////////////////PROGRAMAMOS SENSOR RUIDO
#ifdef OPTOPC817
    pasosProgramacion = 10;
#ifdef DEBUG
    Serial.println(F("pasamos a no programar sensor audio"));
#endif DEBUG
    //#endif OPTOPC817
    if (leerPulsadorProgramacion() == 0) { /////////////////////////////////////////////////////////////////////SI PULSA BOTON PASAMOS DE PROGRAMAR EL SENSOR RUIDO
      pasosProgramacion = 10;
    }
#else OPTOPC817
#ifdef DEBUG
    Serial.println(F("programar sensor ruido"));
#endif
    //   }
    digitalWrite (LED_VERDE, LOW);
    digitalWrite (LED_ROJO, LOW);
    contadorTiempoRespiro = 1;
    pasosProgramacion = 8;
#endif OPTOPC817
  }

  else if ((pasosProgramacion == 8 ) && (contadorTiempoRespiro == 0)) {
    int numCaptacion = 20;
    unsigned int p = 0;
    for (int x = 0; x < numCaptacion; x++) {         ////// bucle que tomara 200 medidas del sensor de ruido para hacer la media
      p += leerRuido ();
    }
    consginaReposo =  p / numCaptacion;


#ifdef DEBUG
    Serial.print(F("ruido reposo = ")); Serial.println(consginaReposo);
    Serial.println (F("esperando ruido"));
#endif
    digitalWrite (LED_VERDE, HIGH);
    digitalWrite (LED_ROJO, LOW);
    pasosProgramacion = 9;
    contadorTiempoRespiro = 1;
  }

  else if ((pasosProgramacion == 9 ) && (contadorTiempoRespiro == 0)) {
#ifdef DEBUG
    Serial.print(".");
#endif
    int numCaptacion = 10;
    int j = leerRuido();
    if (((j - diferenciaRuido > consginaReposo) && (aux < numCaptacion) && ( j > consginaReposo))) {      // ruido detectado mas de 300 de alto que en reposos, numero de veces y que sea mayor para evitar desbirdamiento de variable
      aux2 += j;
      aux ++;
#ifdef DEBUG
      Serial.print(F("valor  ruido ")); Serial.println(j);
      Serial.print(F("valor  aux 2 ")); Serial.println(aux2);
#endif
    }

    if (aux == numCaptacion) {
      consignaRuido =  (aux2 / numCaptacion) - ajustarAlaBajaSensor; // menos 80 para que este un poco por debajo de la media
#ifdef DEBUG
      Serial.print(F("ruido activo = ")); Serial.println(consignaRuido);
#endif
      ledParpadea(6);
      grabarSensorRuido (consginaReposo, consignaRuido);
      pasosProgramacion = 10;
      aux = 0;
      aux2 = 0;
    }
  }

  else if (pasosProgramacion == 10 ) {

#ifdef DEBUG
    Serial.println(F("programacion finalizada"));
#endif
    digitalWrite (LED_VERDE, LOW);
    digitalWrite (LED_ROJO, HIGH);
    estado = false;
#ifdef DEBUG
    Serial.println(F("cambio estado a activado"));
#endif
    pasosProgramacion = 11;

  }
}




bool ruidoOn() {                          //
  int i = 0;
  int conteo = 0;
  int captacion [numCaptacionRuido];
  if (((llamandaPerdida == false) && (llamando == false) && (entrandoLlamada == false))) {
    int p = leerRuido();
    if (consignaRuido <= p) {
      for (int x = 0; x < numCaptacionRuido; x++) {
        captacion[x] = leerRuido();
        //#ifdef DEBUG
        //        Serial.print(F("array ruido valor  ")); Serial.println (captacion[x]);
        //#endif
      }
      for (int x = 0; x < numCaptacionRuido; x++) {
        if (captacion[x] >= consignaRuido) {                ///
          i++;
          conteo ++;
#ifdef DEBUG
          Serial.print(F("array ruido por encima valor  ")); Serial.println (captacion[x] );
          Serial.print(F("conteo que devulve la array con valores superiores  ")); Serial.println (conteo);
#endif
        }
        if (i >= numRuidoActivar) {    // veces que tiene ser detectado sonido para activarse
#ifdef DEBUG
          Serial.println(F("activo funcion ruido"));
#endif
          return true;
        }
      }
    }
  }
  return false;
}


String comprobarNumeroEntrante () {
  String x = leerSim();
  int pos = 0;
#ifdef DEBUG
  Serial.println (F("llamada entrante comprobamos numero "));
#endif
  if (x.indexOf("RING") != -1) {
    pos = x.indexOf("RING");
    x = x.substring(pos + 16, pos + 25);
#ifdef DEBUG
    Serial.print(F("numero entrante es ")); Serial.println (x);
#endif
  }
  return x;
}

int buscarCoincidenciaNumero (String num) {
  /*
     AT+CPBR                 leer todas las entradas
     at+cpbr=1
  */
  SIM.println  ("at+cpbr=1");
  int pos = 0;
  String x = leerSim();
  if (x.indexOf("+CPBR:") != -1) {          // hay numero guardado en la posicion 1 de la sim
    pos = x.indexOf("+CPBR:");
    x = x.substring(pos + 13, pos + 22);
#ifdef DEBUG
    Serial.print(F("1 intento tel recuperado de la sim ")); Serial.println (x);
#endif
    if (x.equals(num)) {
#ifdef DEBUG
      Serial.println(F("numero coincide con el 1 de la sim"));
#endif
      return 1; //coincide num1 de la sim
    }
    else {
      SIM.println  ("at+cpbr=2");
      x = leerSim();
      if (x.indexOf("+CPBR:") != -1) {          // hay numero guardado en la posicion 1 de la sim
        pos = x.indexOf("+CPBR:");
        x = x.substring(pos + 13, pos + 22);
#ifdef DEBUG
        Serial.print(F("2 intento tel recuperado de la sim ")); Serial.println (x);
#endif
        if (x.equals(num)) {
#ifdef DEBUG
          Serial.println(F("numero coincide con el 2 de la sim"));
#endif
          return 2; // coincide el num 2 de la sim
        }
      }
    }
  }
  else
    return 3; // 3 para que ningun num coincida
}

void cambioEstadoPorLlamada( byte segundos, byte num) {
  if (num < 3) {            // numero corresponde a alguno de la sim
    if (segundos < 12) {   // perida corta activa disposito
#ifdef DEBUG
      Serial.println(F("perdida al tel para activar"));
#endif
      SIM.print ("ATD>");
      SIM.print (num); //   SIM.print (telefono1);  // llama al tel 1 de la memoria sim
      SIM.print (";\n");
      digitalWrite (LED_VERDE, LOW);
      digitalWrite (LED_ROJO, HIGH);
      estado = false;
      llamandaPerdida = true;
      segundosLlamada = 1;
#ifdef ESP01
      enviaMqtt (true);
#endif
#ifdef DEBUG
      Serial.println(F("cambio estado a activado"));
#endif

    }
    else if (segundos > 11) {
      digitalWrite (LED_VERDE, HIGH);
      digitalWrite (LED_ROJO, LOW);
      estado = true;
#ifdef ESP01
      enviaMqtt (false);
#endif
#ifdef DEBUG
      Serial.println(F("cambio estado a desactivado"));
#endif
    }
  }
  else {
#ifdef DEBUG
    Serial.println(F("numero no corresponde a ninguno de la sim"));
#endif
  }
}


void perdidaTel (int numero) {
  llamandaPerdida = true;
  segundosLlamada = 1;
  if (numero == 1) {
#ifdef DEBUG
    Serial.println(F("perdida al 1 tel"));
#endif
    SIM.print ("ATD>");
    SIM.print (numero); //   SIM.print (telefono1);  // llama al tel 1 de la memoria sim
    SIM.print (";\n");
  }

  else  if (numero == 2) {
#ifdef DEBUG
    Serial.println(F("perdida al 2 tel"));
#endif
    delay (2000);
    SIM.print ("ATD>");
    SIM.print (numero);
    SIM.print (";\n");

  }
}

void  llamar (int numero) {
  llamando = true;             //variable para cada segundo vaya comprobando el estado de la llamada
  digitalWrite (LED_ESTADO, LOW);
  segundosLlamada = 1;
  if (numero == 1) {
    intentosLlamadas = 1;     // se ha llamada al tel 1
#ifdef DEBUG
    Serial.println(F("llamo al 1 numero de telefono "));
#endif
    SIM.print ("ATD>");
    SIM.print (numero);
    SIM.println (";");
    delay(500);
    //#ifdef DEBUG
    //    Serial.println(leerSim());
    //#endif
  }
  else if (numero == 2) {
    intentosLlamadas = 2;
#ifdef DEBUG
    Serial.println(F("llamo al 2 numero de telefono "));
#endif
    delay(2000);               //para dar tiempo al llamar al sguiente num
    SIM.print ("ATD>");
    SIM.print (numero);
    SIM.print (";\n");
    delay(500);
  }
}

//
//bool llamadaColgada() {
//  if ((leerSim().indexOf("NO CARRIER") == -1 ) || (leerSim().indexOf("BUSY") == -1 )) { // han colgado la llamada
//#ifdef DEBUG
//    Serial.println (F("llamada colgada"));
//#endif
//    return true;
//  }
//  else {
//    return false;
//  }
//}

void colgar () {
  intentos = 0;
  segundosLlamada = 0;
  segundosLlamadaEntrante = 0;
  llamando = false;
  llamandaPerdida = false;
  entrandoLlamada = false;
  SIM.println ("ATH");
  digitalWrite (LED_ESTADO, HIGH);
  pasosProgramacionAudio = 0; // por si se esta programando el audio se finaliza
  rele (false);
  delay(50);
}

/*
   AT+CNUM
  +CNUM: “Line 1?,”+1858#######?,145
  OK

  Call the above phone number using another landline phone or mobile phone.
  The hyperterminal should indicate the incoming call through the ‘RING’ alert.
  Answer the phone call by issuing the ATA command in the hyperterminal.
*/

int pruebaEntrante () {
  SIM.println ("AT+CPAS");
  _buffer = leerSim();
  return _buffer.substring(_buffer.indexOf("+CPAS: ") + 7, _buffer.indexOf("+CPAS: ") + 9).toInt();
}/*
  String x;
  SIM.println ("AT+CPAS");
  if (SIM.available()) {
    x = SIM.readString();
    if (x.indexOf("+CPAS: ") != -1) {
      int i = x.substring(x.indexOf("+CPAS: ") + 7, x.indexOf("+CPAS: ") + 9).toInt();
  #ifdef DEBUG
      Serial.print (F("estado llamada = ")); Serial.println (i);
  #endif
      return i;
    }
    else {
  #ifdef DEBUG
      Serial.print (F("estado llamada no leido variable corresponde = ")); Serial.println (x);
  #endif
      return 5;
    }
  }
  #ifdef DEBUG
  Serial.println (F("estado llamada sin nada en buffer"));
  #endif
  return 5;
  }
*/
int llamadaEntrante () {
  String x;
  if (SIM.available()) {
    x = SIM.readString();
    if (x.indexOf("RING") != -1) {
#ifdef DEBUG
      Serial.println (F("llamada entrante"));
#endif
      return 3;
    }
  }
  else return 0;
  /*
    SIM.println ("AT+CPAS");
    delay(10);
    _buffer = leerSim();
    return _buffer.substring(_buffer.indexOf("+CPAS: ") + 7, _buffer.indexOf("+CPAS: ") + 9).toInt();*/
}

int estadoLlamada() { //uint8_t
  String x;
  if (SIM.available()) {
    x = SIM.readString();
    if (x.indexOf("BUSY") != -1) {
#ifdef DEBUG
      Serial.println (F("llamada colgada"));
#endif
      return 0;
    }
    else if (x.indexOf("NO CARRIER") != -1) {
#ifdef DEBUG
      Serial.println (F("no contesta o llamada finalizada"));
#endif
      return 0 ;
    }
  }
  else
    // Serial.println ("error");
    return 5;
}

/*
  values of return:

  0 Ready (MT allows commands from TA/TE)
  2 Unknown (MT is not guaranteed to respond to tructions)
  3 Ringing (MT is ready for commands from TA/TE, but the ringer is active) "llamada entrante"
  4 Call in progress

  if (SIM.available() > 0)
  {

  incoming = (SIM.read()); //Guardamos el carácter del GPRS
  mensaje = mensaje + incoming ; // Añadimos el carácter leído al mensaje
  Serial.print(incoming); //Mostramos el carácter en el monitor seri
  }
*/

//  SIM.flush();
// SIM.println ("AT+CPAS");
//  delay(10);
//  //int p = 0;
//    while  (!SIM.available() )  //12000
//  {
//  //   if  (!SIM.available()) {
//  //  SIM.println ("AT+CPAS");
//    return "pepe";
//   }
// delay(5);
//    Serial.print ("error "); Serial.println (p);
//    p++;
// return 5;
// }

// x = x.substring(x.indexOf("+CPAS: ") + 7, x.indexOf("+CPAS: ") + 9);
// if (x.indexOf ("+CPAS: ") != -1) {
// Serial.println (x);


//  else {
//  //  SIM.println ("AT+CPAS");
//    return "pepe";
//  }
//.substring(x.indexOf("+CPAS: ") + 7, x.indexOf("+CPAS: ") + 9).toInt();
// }
//  if  (!SIM.available()) {
//    return 0;
//    delay (10);
//  }
//  else return 0;


void rele (bool estado) {
  if (estado) {
    digitalWrite (RELE, HIGH);
#ifdef DEBUG
    Serial.println(F("rele on"));
#endif
  }
  else {
    digitalWrite (RELE, LOW);
#ifdef DEBUG
    Serial.println(F("rele off"));
#endif
  }
}
/*
  String Sim800l::getNumberSms(uint8_t index){
  _buffer=readSms(index);
  Serial.println(_buffer.length());
  if (_buffer.length() > 10) //avoid empty sms
  {
    uint8_t _idx1=_buffer.indexOf("+CMGR:");
    _idx1=_buffer.indexOf("\",\"",_idx1+1);
    return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+4));
  }else{
    return "";
  }
  }
*/

String smsRecibido() {
  /*
    SIM.print (F("AT+CMGF=1\r"));
    #ifdef DEBUG
    Serial.println (leerSim());
    #endif
    if (( leerSim().indexOf("OK"))!= -1) {
      SIM.print (F("AT+CMGR="));
      SIM.print (1);
      SIM.print("\r");
      _buffer = leerSim();
      if (_buffer.indexOf("CMGR:") != -1) {
        return _buffer;
      }
      else return "ERROR LECTURA";
    }
    else
      return "ERROR COMANDO";
    }
  */

  String x = leerSim();
#ifdef DEBUG
  Serial.println(x);
#endif
  return x;



}
void borrarSMS() {
  SIM.print("at+cmgda=\"del all\"\n\r");
#ifdef DEBUG
  Serial.println (leerSim());
#endif

}

//////////////////////////////////////////////////////////////////  grabar tel en sim
/*
   AT+CPBR                 leer todas las entradas
   at+cpbr=1

   AT+CPBF                 encontrar una entrada
   at+cpBf="santi"

   AT+CPBW                 almacenar una entrada
   AT+CPBW=1,"676000000",145,"santi"

   AT+CPBS                 buscar una entrada
*/

void grabarSim (String comando, int pos) {
  Serial.print(F("gestionando la sim grabando ")); Serial.println(comando);
  // String tel = comando;
  if (pos == 3) {

    SIM.println("AT+CPBW= 1,\"000000000\", 145, \"borrado1\"");
#ifdef DEBUT
    Serial.println(leerSim());
#endif
    SIM.println("AT+CPBW= 2,\"000000000\", 145, \"borrado2\"");
#ifdef DEBUT
    Serial.println(leerSim());
#endif
  }

  if (pos == 1) {
    SIM.print("AT+CPBW=1,\""); SIM.print(comando); SIM.print("\""); SIM.print(",145,\""); SIM.println("santi\"");
#ifdef DEBUT
    Serial.println(leerSim());
#endif

  }
  if (pos == 2) {
    SIM.print("AT+CPBW=2,\""); SIM.print(comando); SIM.print("\""); SIM.print(",145,\""); SIM.println("berta\"");
#ifdef DEBUT
    Serial.println(leerSim());
#endif
  }


}



/////////////////////////////////////////////////////////////////// EEPROM

void grabarEeprom() {}
/*
  int eeAddress = 0;
  String x = "";

  x =  EEPROM.get( eeAddress, telefono1);
  #ifdef DEBUG
  Serial.print ("primera variable vale "); Serial.println (x.length());
  #endif
  if (x.length() < 6) {
  #ifdef DEBUG
    Serial.println ("primer espacio de la memoria libre, grabo aqui");
    Serial.println ("tel 1 grabado");
  #endif
    EEPROM.put( eeAddress, telSms );

  }

  else {
    eeAddress += sizeof(telefono1);
    x =  EEPROM.get( eeAddress, telefono2);
  #ifdef DEBUG
    Serial.print ("segunda variable vale "); Serial.println (x.length());
  #endif
    if (x.length() < 6) {
      EEPROM.put( eeAddress, telSms );
  #ifdef DEBUG
      Serial.println ("segundo espacio de la memoria libre, grabo aqui");
      Serial.println ("tel 2 grabado");
  #endif
    }
    else {
  #ifdef DEBUG
      Serial.println ("dos telefonos grabados");
  #endif
    }
  }
  }

*/
void leerEeprom() {
  /*
    eeAddress = 0;
      Serial.print( "coloco el puntero en posicion: " );Serial.println (eeAddress);
    EEPROM.put( eeAddress, tel1 );
    Serial.println ("tel 1 grabado");
    Serial.print ("tamaño que ocupa el tel en la eeprom ") ; Serial.println (sizeof(tel1));
    eeAddress += sizeof(tel1);
    Serial.print( "coloco el puntero en posicion: " );Serial.println (eeAddress);
    EEPROM.put( eeAddress, tel2 );
    Serial.println ("tel 2 grabado");
  */

  /*
    eeAddress = 0;
    Serial.print( "coloco el puntero en posicion: " ); Serial.println (eeAddress);
    Serial.print( "telefono 1 de la eeprom: " );
    String p = EEPROM.get( eeAddress, telefono1 );
    Serial.println(telefono1);
    eeAddress += sizeof(telefono1);
    Serial.print( "coloco el puntero en posicion: " ); Serial.println (eeAddress);
    Serial.print( "telefono 2 de la eeprom: " );
    EEPROM.get( eeAddress, telefono2);
    Serial.println( telefono2 );
    /*

      String i = "";
      int eeAddress = 0;

      if (telefono1.length() < 6) { // telefono 1 en variable volatil vacia
      eeAddress = 0;
      i = EEPROM.get( eeAddress, telefono1 );
      #ifdef DEBUG
      Serial.print(F( "telefono 1 de la eeprom: " ));
      Serial.println(i);
      #endif

      if (i.length() < 6) { // 1 tel de la eeprom vacio
      #ifdef DEBUG
        Serial.println(F( "telefono 1 de la eeprom vacio, nada para recuperar " ));
      #endif
      } else {
        telefono1 = i;
      #ifdef DEBUG
        Serial.println(F( "tel eeprom copi" )); Serial.print (F("tel es : ")); Serial.println (telefono1);
        Serial.println(F("busca 2 tel"));
      #endif
        eeAddress += sizeof (i);
      #ifdef DEBUG
        Serial.print(F( "puntero en posicion " )); Serial.println(eeAddress);
      #endif
        i = EEPROM.get( eeAddress, telefono2 );
        if (i.length() < 6) {      // ningun telefono 2 en la eeprom
      #ifdef DEBUG
          Serial.println(F( "no tel 2 en eeprom " ));
      #endif
        } else {
          telefono2 = i;
      #ifdef DEBUG
          Serial.print(F( "si tel 2 en eeprom, es: " ));
          Serial.println(telefono2);
      #endif
        }
      }

      } else if ((telefono1.length() > 6) && ( telefono2.length() < 6)) { // telefono 1 en volatil hay dos no, recuperamos dos si hay en eeprom
      i = EEPROM.get( eeAddress, telefono1 );  // buscamos tamaño variable
      eeAddress += sizeof (i);  // puntero en posicion del bit corespondiente a la segunda variable
      #ifdef DEBUG
      Serial.print(F( "1 tel si que en volatil pero no 2, buscamos...\nPuntero pos " ));
      Serial.println(eeAddress);
      #endif
      i = EEPROM.get( eeAddress, telefono2 );  // buscamos el 2 tel en la eeprom
      if (i.length() < 6) {
      #ifdef DEBUG
        Serial.println(F( "no tel 2 eeprom" ));
      #endif
      }
      else {
        telefono2 = i;
      #ifdef DEBUG
        Serial.print(F( "si tel2 en eeprom, grabo\n'v' tiene valor : " )); Serial.println(telefono2);
      #endif

      }
      }
      else {
      #ifdef DEBUG
      Serial.println(F( "volatil tiene 2 tel " ));
      Serial.print(telefono1);    Serial.print(F("        "));    Serial.println(telefono2);
      #endif
      }
  */
}


void grabarSensorRuido(int sensorLow, int sensorHigh) {

  byte eeAddressLow = 50;                // posicion 50 de la eeprom para el valor low
  byte eeAddressHigh = 60;                // posicion 60 de la eeprom para el valor high

  EEPROM.put( eeAddressLow, sensorLow );
#ifdef DEBUG
  Serial.print (F("grabo en la posicion de la eeprom ")); Serial.print (eeAddressLow); Serial.print (F(" el valor del sensor low ")); Serial.println (sensorLow);
#endif
  EEPROM.put( eeAddressHigh, sensorHigh );
#ifdef DEBUG
  Serial.print (F("grabo en la posicion de la eeprom ")); Serial.print (eeAddressHigh); Serial.print (F(" el valor del sensor high ")); Serial.println (sensorHigh);
#endif
}

void recuperaSensorRuido () {
  byte eeAddressLow = 50;                // posicion 50 de la eeprom para el valor low
  byte eeAddressHigh = 60;
  int valor = 0;
  int x = EEPROM.get (eeAddressLow, valor);
  if (x == 0) {
#ifdef DEBUG
    Serial.print (F("recuperando sensor ruido low, posicion  ")); Serial.print (eeAddressLow); Serial.print (F(" valor  ")); Serial.print (x); Serial.println (F(" no hay valor"));
#endif
  }
  else {
    consginaReposo = x;
#ifdef DEBUG
    Serial.print (F("recuperando el valor sensor ruido low,posicion  ")); Serial.print (eeAddressLow); Serial.print (F(" valor  ")); Serial.print (consginaReposo); Serial.println (F(" grabo variable"));
#endif
  }
  x = EEPROM.get (eeAddressHigh, valor);
  if (x == 0) {
#ifdef DEBUG
    Serial.print (F("recuperando el valor sensor ruido high,posicion  ")); Serial.print (eeAddressHigh); Serial.print (F(" valor  ")); Serial.print (x); Serial.println (F(" no hay valor"));
#endif
  }
  else {
    consignaRuido = x;
#ifdef DEBUG
    Serial.print (F("recuperando el valor sensor ruido high,posicion  ")); Serial.print (eeAddressHigh); Serial.print (F(" valor  ")); Serial.print (consignaRuido); Serial.println (F(" grabo variable"));
#endif
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////GRABAMOS EEPROM NIVEL AUDIO
void grabarivelAudio(int sensorLow, int sensorHigh) {

  byte eeAddressLow = 50;                // posicion 50 de la eeprom para el valor low
  byte eeAddressHigh = 60;                // posicion 60 de la eeprom para el valor high

  EEPROM.put( eeAddressLow, sensorLow );
#ifdef DEBUG
  Serial.print (F("grabo en la posicion de la eeprom ")); Serial.print (eeAddressLow); Serial.print (F(" el valor del sensor low ")); Serial.println (sensorLow);
#endif
  EEPROM.put( eeAddressHigh, sensorHigh );
#ifdef DEBUG
  Serial.print (F("grabo en la posicion de la eeprom ")); Serial.print (eeAddressHigh); Serial.print (F(" el valor del sensor high ")); Serial.println (sensorHigh);
#endif
}

void grabarNivelMicAltavoz(int nivel, byte dispositivo) { //0 para el mic   1 para el altavoz{

  byte eeAddressMic = 150;
  byte eeAddressAltavoz = 160;
  if (dispositivo == 0) {
    EEPROM.put( eeAddressMic, nivel );
#ifdef DEBUG
    Serial.print (F("grabo en la posicion de la eeprom ")); Serial.print (eeAddressMic); Serial.print (F(" el valor  ")); Serial.println (nivel);
#endif
  }

  else if (dispositivo == 1) {
    EEPROM.put( eeAddressAltavoz, nivel );
#ifdef DEBUG
    Serial.print (F("grabo en la posicion de la eeprom ")); Serial.print (eeAddressAltavoz); Serial.print (F(" el valor  ")); Serial.println (nivel);
#endif
  }
}

void recuperaNivelMic () {

  byte eeAddressMic = 150;
  int valor = 0;
  int i = EEPROM.get (eeAddressMic, valor);

  if (( i > -1 ) && ( i < 16)) { // hay un valor en la eeprom
#ifdef DEBUG
    Serial.print (F("busco en la posicion de la eeprom ")); Serial.print (eeAddressMic); Serial.print (F(" el valor del mic  ")); Serial.println (i);
    Serial.println (F("valor del mic en la eeprom correcto y lo envio al sim800l "));
#endif

    if (i < 10) {
      SIM.print("AT+CMIC=0,0"); SIM.println(i); // ajustar nivel de microfono  AJUSTAR DE 0-15
    }

    else {
      SIM.print("AT+CMIC=0,"); SIM.println(i); // ajustar nivel de microfono  AJUSTAR DE 0-15
    }
#ifdef DEBUG
    Serial.println(leerSim());
#endif
  }
  else {
#ifdef DEBUG
    //  Serial.print (F("busco en la posicion de la eeprom ")); Serial.print (eeAddressMic); Serial.print (F(" el valor del mic  "));Serial.print (i);
    Serial.println (F("no hay valor en la eeprom del mic y envio valor predeterminado"));
#endif
    SIM.println("AT+CMIC=0,01");
#ifdef DEBUG
    Serial.println(leerSim());
#endif
  }
}

void recuperarNivelAltavoz() {
  byte eeAddressAltavoz = 160;
  int valor1 = 0;
  int x = EEPROM.get (eeAddressAltavoz, valor1);
  if (( x > -1 ) && ( x < 101)) { // hay un valor en la eeprom del altavoz
#ifdef DEBUG
    Serial.print (F("busco en la posicion de la eeprom ")); Serial.print (eeAddressAltavoz); Serial.print (F(" el valor del altavoz  ")); Serial.println (x);
    Serial.println (F("valor del altavoz en la eeprom correcto y lo envio al sim800l "));
#endif
    SIM.print("AT+CLVL="); SIM.println(x);   // ajustar volumen altavoz   AJUSTAR 0-100
#ifdef DEBUG
    Serial.println(leerSim());
#endif
  }
  else {
#ifdef DEBUG
    //  Serial.print (F("busco en la posicion de la eeprom ")); Serial.print (eeAddressAltavoz); Serial.print (F(" el valor del altavoz  "));Serial.println (x);
    Serial.println (F("no hay valor en la eeprom del altavoz y envio valor predeterminado"));
#endif
    SIM.println("AT+CLVL=40");
#ifdef DEBUG
    Serial.println(leerSim());
#endif
  }
}


//////////////////////////////////////////////////////////////////////////////FINAL EEPROM


void ledParpadea (byte veces) {
  for (int x = 0; x < veces; x++) {
    digitalWrite (LED_VERDE, HIGH);
    digitalWrite (LED_ROJO, HIGH);
    delay (500);
    digitalWrite (LED_VERDE, LOW);
    digitalWrite (LED_ROJO, LOW);
    delay (500);
  }
}

void ledRojoParpadea (byte veces) {
  for (int x = 0; x < veces; x++) {
    digitalWrite (LED_ROJO, HIGH);
    delay (500);
    digitalWrite (LED_ROJO, LOW);
    delay (500);
  }
}
void ledVerdeParpadea (byte veces) {
  for (int x = 0; x < veces; x++) {
    digitalWrite (LED_VERDE, HIGH);
    delay (500);
    digitalWrite (LED_VERDE, LOW);
    delay (500);
  }
}


void funcionTiempo () {
  unsigned long TimNow = millis();
  if (TimNow < ExTimer ) {
    ExTimer = TimNow;
  }
  if ((TimNow - ExTimer) >= 100) {
    ExTimer = TimNow;
    ExcTimer();
  }
}

void ExcTimer() { //Esta funcion se ejecuta cada decima de segundo
  CountMsg++;
  if (CountMsg >= 10) {
    CountMsg = 0;
    CountSg++;
    LoopNewSecond();
  }

}
