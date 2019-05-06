/*                                           
 
 
                                                NOTAS
                    SIM 800L LLAMA A TELEFONO

                    
      BUSY                               QUIERE DECIR QUE HE COLGADO LA LLAMADA
      NO CARRER                         QUIERE DECIR QUE NO HE COGIDO LLAMADA



                  TELEFONO LLAMA A SIM 800L

      NO CARRER                        QUIERE DECIR LLAMAR Y COLGAR
      RING                              MIENTRAS SE REALIZA LA LLAMADA
      NO CARRER                        DESPUES DE TODOS LOS RINGS SE CUELGA POR TIEMPO

      ATH                              CUELGA LA LLAMADA
      ATA                               RESPONDE LA LLAMADA
*/
////////////////////////////rf433
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

/////////////////////////////

#include <SoftwareSerial.h>
SoftwareSerial mySerial(7,8 );




//////////////////////// funcion tiempo

unsigned long ExTimer;

byte CountSg=0;//Cuenta los segundos de 1 - 30
byte CountMsg=0;
byte segundos=0;




//////////////////////// PARA LA FUNCION DE IDENTIFICADOR DE LLAMADA
char incoming_char = 0; //Variable que guarda los caracteres que envia el sim800l
int estado = 0;

bool llamando=false;
boolean alarma_on=false;
bool alarma_off=false;
bool preparado_llamar=false;


const int led_verde =A4;    // led enciende alarma off           
const int led_rojo = A5;    // les enciende alarma on             
//const int pulsador = 10;    // pin  para activar la variable alarma on con pulsador    TIENE RESISTENCIA PULL DONW
//int val_pulsador = 0;  

const int opto_sim800l=A0;     //optoacoplador que activa la entrada altavoz y mic del portero a la sim

const int llamada_portero = 4;    // pin 4 para activar la variable de la llamada posteriormente sera el timbre    TIENE RESISTENCIA PULL DONW
int val_llamada_portero = 0;      //estado del pulsador tiene resistencia pull donw


int llamada_nocarrier=0;
int llamada_busy=0;
int llamada_ring=0;
String mensaje = "";      //variable que almacena el estado del NO CARRIER cuando la llamada no es contestada
//char dato ;
//int fin_de_llamada = 0;  // almacena el estado si la llamada no es contestada 
char incoming = 0;      //Variable que guarda los caracteres que envia el SIM800L

///////////variable para almacenar el pir que salt

byte sensor_alarma=0;


void setup () {

  //////////////////////////////rf433

     mySwitch.enableReceive(1); // pin 1 interrupcion que corresponde al pin 3 del nano

  
  /////////////////// configuracion sim800l
   Serial.begin(9600);
   mySerial.begin(9600);
   Serial.println ("espera un poco hasta que encuentre cobertura");
   delay (1000); //tiempo para que coja cbertura 9 segundos
   mySerial.println("AT");
   delay(1000);
   mySerial.println("AT+CMGF=1");   //Configuramos el módulo para trabajar con SMS
   delay(1000);
   mySerial.println("AT+CLIP=1");   //Configuramos para identificar la llamada
   delay(1000);
     // mySerial.println("AT\r\n");
   Serial.println ("funciono");

ExTimer=millis();     ///////////funcion tiempo
  

  pinMode (led_verde, OUTPUT);
  pinMode (led_rojo, OUTPUT);

  
  pinMode(llamada_portero,INPUT);//
  val_llamada_portero =0;    //se ativa con high



  pinMode (opto_sim800l, OUTPUT);
  
 alarma_on=true;   
// llamar ();
}
void loop () {

//////////////////////// funcion tiempo

 unsigned long TimNow=millis();   
   if(TimNow < ExTimer ) {ExTimer=TimNow;}
   if((TimNow - ExTimer) >= 100) {ExTimer=TimNow;ExcTimer();}




///////////////////////////////////rf433

 if (mySwitch.available()) {
    
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    
  
  if (mySwitch.getReceivedValue()==*******){       //////////poner codigo en ******************
  alarma_off=true;
  delay (1000);                                     //// tiempo para que solo reciba un codigo
  Serial.println ("recibido codigo rf433mhz alarma off");
}

  else if (mySwitch.getReceivedValue()==**********){       
  alarma_on=true;
  delay (1000);
  Serial.println ("recibido codigo rf433mhz alarma on");
}

else if ((preparado_llamar==true)&&(mySwitch.getReceivedValue()== ********))  { // codigo rf
 Serial.println ("salto pir cuarto frio");
 sensor_alarma=1;           // para definir el sensor a la hora de enviar el sms
 mensaje_alarma ();
 }


 else if ((preparado_llamar==true)&&(mySwitch.getReceivedValue()== ********))  { // rf pir entrada garaje
 Serial.println ("salto pir entrada garaje");
 sensor_alarma=2;           // para definir el sensor a la hora de enviar el sms
 mensaje_alarma ();
 }


 else if ((preparado_llamar==true)&&(mySwitch.getReceivedValue()== ***********))  { // rf contactor entrada
 Serial.println ("salto pcontactor entrada");
 sensor_alarma=3;           // para definir el sensor a la hora de enviar el sms
 mensaje_alarma ();
 }

else if ((preparado_llamar==true)&&(mySwitch.getReceivedValue()== *********))  { // rf comedot
   Serial.println ("salto comedor");
 sensor_alarma=4;           // para definir el sensor a la hora de enviar el sms
 mensaje_alarma ();
 }

else if ((preparado_llamar==true)&&(mySwitch.getReceivedValue()== ***********))  { // rf pasillo
   Serial.println ("salto pasillo");
 sensor_alarma=5;           // para definir el sensor a la hora de enviar el sms
 mensaje_alarma ();
 }

 else if ((preparado_llamar==true)&&(mySwitch.getReceivedValue()== ********** ))  { // rf comedot
   Serial.println ("salto  puerta cocina");
 sensor_alarma=6;           // para definir el sensor a la hora de enviar el sms
 mensaje_alarma ();
 }

  else if ((preparado_llamar==true)&&(mySwitch.getReceivedValue()== ******** ))  { // rf golfas poner codigo
   Serial.println ("salto  golfas");
 sensor_alarma=7;           // para definir el sensor a la hora de enviar el sms
 mensaje_alarma ();
 }
 
   else if ((preparado_llamar==true)&&(mySwitch.getReceivedValue()== ********** ))  { // rf caseta madera ponner codigo
   Serial.println ("salto  caseta madera");
 sensor_alarma=8;           // para definir el sensor a la hora de enviar el sms
 mensaje_alarma ();
 }

  mySwitch.resetAvailable();
  }
  /////////////////////////////////////////
  
 
 val_llamada_portero=digitalRead(llamada_portero);  // comprobar estado pulsador y luego sera del timbre


                ////////////////////DEFINIMOS LOS ESTADOS DE LA ALARMA , LOS LEDS Y VARIABLES
 if (alarma_on== true){
    digitalWrite(led_verde, LOW);
    digitalWrite(led_rojo, HIGH);
    alarma_off=false;
    perdida_on();          /////////////saber si que ha activado
    Serial.println("bucle alarma on");
    alarma_on=false;
    preparado_llamar=true;
 }

 else if (alarma_off==true){
        alarma_on=false;
        perdida_off ();           /////////// saber que se ha desactivado
    digitalWrite(led_verde, HIGH);
    digitalWrite(led_rojo, LOW);
      Serial.println("bucle alarma off");
  alarma_off=false;
  preparado_llamar=false;
  }


if (Serial.available() > 0){
 mySerial.write(Serial.read());
   }
 if (mySerial.available() > 0)
  {
    incoming = (mySerial.read()); //Guardamos el carácter del GPRS
     mensaje = mensaje + incoming ; // Añadimos el carácter leído al mensaje
     Serial.print(incoming); //Mostramos el carácter en el monitor seri
    
    }


  llamada_nocarrier = mensaje.indexOf("NO CARRIER");
    if (llamada_nocarrier >= 0){
      Serial.print("a ver ahora");
      mensaje="";
      cuelga();}
      
    
  
  llamada_busy = mensaje.indexOf("BUSY");
  if (llamada_busy >= 0){
      Serial.print("a ver ahora busy");
      mensaje="";
      cuelga();}


if  ((val_llamada_portero>0)&&(preparado_llamar==true)){
  
  Serial.println ("llaman al portero y alarma activada");
  delay (500);  // tiempo para que el pulsador no de muchos pulsos seguidos
   llamar();
}

 
 }

void ExcTimer(){//Esta funcion se ejecuta cada decima de segundo
  CountMsg++;
  if (CountMsg>=10){CountMsg=0;CountSg++;LoopNewSecond();}
  
}


void perdida_on(){              // llamada perdida al activar la variable alarma on
   Serial.println("Realizando llamada perdida alarma on ");
 mySerial.println("AT+CLIP=1");
 delay(1000);
    mySerial.println("ATD***********;");  //Comando AT para realizar una llamada                   cambiar ***************** por tel
    delay(800);  // Espera 2 min mientras realiza la llamada
    mySerial.println("ATH");  // Cuelga la llamada
    delay(1000);
    Serial.println("Llamada perdida finalizada");
  }

void perdida_off(){              // llamada perdida al activar la variable alarma on
   Serial.println("Realizando llamada perdida alarma  off");
 mySerial.println("AT+CLIP=1");
 delay(1000);
    mySerial.println("ATD***************;");  //Comando AT para realizar una llamada
    delay(8000);  // Espera 2 min mientras realiza la llamada
    mySerial.println("ATH");  // Cuelga la llamada
    delay(1000);
    Serial.println("Llamada perdida finalizada");
  }

void llamar()             //llamada para hablar por el telefonillo
  {
    Serial.println("Realizando llamada...");
     mySerial.println("ATD******************;");  //Comando AT para realizar una llamada
      delay (10000);                                                          ///// TIEMPO DE RETARDO PARA QUE DE TIEMPO A LLAMAR Y DESCOLGAR SIN QUE EN EL TELEFONILLO SUENEN LOS TONOS
       segundos=1;
   digitalWrite (opto_sim800l, HIGH);
    Serial.println("activo rele");
      Serial.println("llamada de 1 minuto");
   }



void cuelga(){
  Serial.println("funcion cuelga");
    mySerial.println("ATH");  // Cuelga la llamada
    delay(500);
    Serial.println("Llamada finalizada");
     digitalWrite (opto_sim800l, LOW);
     segundos=0;
   }
  

void LoopNewSecond(){

  if (segundos >=1){

   segundos++;
Serial.println (segundos);

    if (segundos==90){            ///     1 minuto y medio de llamada     tiempo que dura la llamada

    cuelga();
  }}
  }
  
  




  // Funcion para el envio de un SMS
void mensaje_alarma(){ 

 if (sensor_alarma==1){                         // sensor cuarto frio 
 mySerial.println("AT+CMGF=1");                 // Activamos la funcion de envio de SMS
 delay(100);                                    // Pequeña pausa
 mySerial.println("AT+CMGS=\"+**************\"");  // Definimos el numero del destinatario en formato internacional      CAMBIAR ******* POR TEL
 delay(100);                                    // Pequeña pausa
 mySerial.print("salto volumetrico cuarto frio");                 // Definimos el cuerpo del mensaje
 delay(500);                                    // Pequeña pausa
 mySerial.print(char(26));                      // Enviamos el equivalente a Control+Z 
 delay(100);                                    // Pequeña pausa
 mySerial.println("");                          // Enviamos un fin de linea
 delay(100); 
 sensor_alarma=0;}                               

else if (sensor_alarma==2){                    // sensor entrada garaje
mySerial.println("AT+CMGF=1");                
 delay(100);                                   
 mySerial.println("AT+CMGS=\"+34************\"");  
 delay(100);                                    
 mySerial.print("salto volumetrico entrada garaje");                
 delay(500);                                   
 mySerial.print(char(26));                      
 delay(100);                                   
 mySerial.println("");                         
 delay(100); 
 sensor_alarma=0;}

              
else if (sensor_alarma==3){                    // contactor entrada principal
mySerial.println("AT+CMGF=1");                
 delay(100);                                   
 mySerial.println("AT+CMGS=\"+34*****************\"");  
 delay(100);                                    
 mySerial.print("salto entrada principal");                
 delay(500);                                   
 mySerial.print(char(26));                      
 delay(100);                                   
 mySerial.println("");                         
 delay(100); 
 sensor_alarma=0;}

 else if (sensor_alarma==4){                    // sensor comedot
mySerial.println("AT+CMGF=1");                
 delay(100);                                   
 mySerial.println("AT+CMGS=\"+34***************\"");  
 delay(100);                                    
 mySerial.print("salto volumetrico comedor");                
 delay(500);                                   
 mySerial.print(char(26));                      
 delay(100);                                   
 mySerial.println("");                         
 delay(100); 
 sensor_alarma=0;}

 else if (sensor_alarma==5){                    // sensor pasillo
 delay(100);                                   
 mySerial.println("AT+CMGS=\"+34******************\"");  
 delay(100);                                    
 mySerial.print("salto volumetrico pasillo");                
 delay(500);                                   
 mySerial.print(char(26));                      
 delay(100);                                   
 mySerial.println("");                         
 delay(100); 
 sensor_alarma=0;}


else if (sensor_alarma==6){                    // puerta cocina
mySerial.println("AT+CMGF=1");                
 delay(100);                                   
 mySerial.println("AT+CMGS=\"+34*****************\"");  
 delay(100);                                    
 mySerial.print("salto volumetrico golfas");                
 delay(500);                                   
 mySerial.print(char(26));                      
 delay(100);                                   
 mySerial.println("");                         
 delay(100); 
 sensor_alarma=0;}


 else if (sensor_alarma==7){                    // golfas
mySerial.println("AT+CMGF=1");                
 delay(100);                                   
 mySerial.println("AT+CMGS=\"+34****************\"");  
 delay(100);                                    
 mySerial.print("salto volumetrico golfas");                
 delay(500);                                   
 mySerial.print(char(26));                      
 delay(100);                                   
 mySerial.println("");                         
 delay(100); 
 sensor_alarma=0;}

 else if (sensor_alarma==8){                    // sensor caseta madera
mySerial.println("AT+CMGF=1");                
 delay(100);                                   
 mySerial.println("AT+CMGS=\"+34*****************\"");  
 delay(100);                                    
 mySerial.print("salto volumetrico caseta madera");                
 delay(500);                                   
 mySerial.print(char(26));                      
 delay(100);                                   
 mySerial.println("");                         
 delay(100); 
 sensor_alarma=0;}
}
