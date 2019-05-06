# gsm-y-portero-autom-tico-golmar
portero automático llama por gsm (sim800l) a teléfono mediante un arduino nano

Este proyecto ha sido posible a otros proyectos que he encontrado por las webs, a todos ellos muchas gracias por compartir

Se trata de un circuito que integra un sim800l y un nano conectado al portero (en mi caso un Golmar t-700). Este dispositivo lo que hace es que si estoy fuera de casa y pican al telefonillo me hace una llamada a mi número y desde mi teléfono puedo hablar. ( la intención era que si picaba algún "caco" para saber si hay alguien dentro y poder simular que sí).


El dispositivo se puede activar (variable fuera de casa on para que me llame) por recepción de rf433, pulsador o modbus (este último no está probado todavía). Además combinándolo con volumétricos rf433 sirve de alarma GSM.

El impreso tiene un en la parte del modulo sim800l y es que el hace contacto las diferentes conexiones debido a la aureola que hay fuera de ellos, esta pendiente de modificación pero solo limando un poco las pistas para que no hagan contacto es suficiente


Los materiales necesarios por que he usado son:

1 arduino nano.
1 sim 800l.
1 receptor RF de 43 .
3 leds.
3 mosfet bs170 .
1 rele SRA-05VDC.
2 diodo 1N4001.
1 fusible.
1 swich.
1 lm317.
1 lm350.
1 7805.
1 optoacoplacodor pc817.
1 zeners 5,1v.
bornas.

condesadores.

1- 1mf.
4- 100nf.
2- uf.
2- pf.
1- 0.1mf.

resistencias.

1- 33k.
1- 1k.
2- 450.
3- 330.
1- 200.
2- 10.
7 - 10k.


Si álguien más lo prueba en diferentes telefonillos que lo reporte. 

Gracias!!!!!!
