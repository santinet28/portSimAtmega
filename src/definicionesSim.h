/*                                              NOTAS
                    SIM 800L LLAMA A TELEFONO

                    
      BUSY                               QUIERE DECIR QUE HE COLGADO LA LLAMADA
      NO CARRER                         QUIERE DECIR QUE NO HE COGIDO LLAMADA
      NO DIALTONE


                  TELEFONO LLAMA A SIM 800L

      NO CARRER                        QUIERE DECIR LLAMAR Y COLGAR
      RING                              MIENTRAS SE REALIZA LA LLAMADA
      NO CARRER                        DESPUES DE TODOS LOS RINGS SE CUELGA POR TIEMPO

      ATH                              CUELGA LA LLAMADA
      ATA                               RESPONDE LA LLAMADA
*/


#define RX_PIN 7
#define TX_PIN 8
#define RESET_PIN 6   // reset modulo sim 800l
#define LED_ESTADO 9 //les indica estado modulo 
#define LED_VERDE 10
#define LED_ROJO  11
#define SWICH_PROGRAMACION 13
#define PULSA_PROGRAMACION 12
#define PULSA_CAMBIO_ESTADO 2
#define SENSOR_RUIDO A3
#define PC817 17
#define POTENCIO A1
#define  RELE 4
