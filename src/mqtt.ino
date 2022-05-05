
void compruebaMqtt (String x) {
  String pOn = "portero encendido\n";
  String pOff = "portero apagado\n";
 

  if (pOn.equalsIgnoreCase(x)) {
    ledParpadea(2);
    digitalWrite (LED_VERDE, LOW);
    digitalWrite (LED_ROJO, HIGH);
    estado = false;
  }
  else if (pOff.equalsIgnoreCase(x)) {
    ledParpadea(4);
    digitalWrite (LED_VERDE, HIGH);
    digitalWrite (LED_ROJO, LOW);
    estado = true;
  }
  else{
     ledRojoParpadea(5);
  }
}


void enviaMqtt (bool estado) {
  if (estado) {
    Serial.print ("activaPorteroMqtt\n");
  }
  else {
    Serial.print ("desactivaPorteroMqtt\n");
  }
}
