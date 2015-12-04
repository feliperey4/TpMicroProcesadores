
/*==================[inclusions]=============================================*/

#include "main.h"

/*==================[macros and definitions]=================================*/

#define SEG_VALVULA 5
#define VALVULA_PIN 1

#define F_CPU 204000000UL  /* Freq CPU = 204 MHz */
#define TICKRATE_HZ (100) /* 100 ticks per second */

#define BOTON_PULSO   0

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

uint16_t adcVal_1=0;
uint16_t adcVal_2=0;
uint32_t contEncoder=0;
uint32_t segValvula=0;
uint32_t torOpt=10000;
uint8_t estadoPulso=0;
uint16_t ticksTx=90;

char  contAux=0;
uint32_t  pulsosVuelta=1;
char modoTor=0;
char flagRecibido=0;
char stateValvula=0;

// Bandera de tareas
char flagRx=0;
char flagTx=0;
char flagCelda=0;
char flagValvula=0;
char flagEnco=0;

//Contadores de proceso
uint16_t contRx=0;
uint16_t contTx=0;
uint16_t contCelda=0;
uint16_t contValvula=0;
uint16_t contEnco=0;

struct BufferIn bufferIn;

struct EstadosSistema{
   enum EstadoModo{
         INICIO,
         CALIBRAR,
         OPERAR
      }estadoModo;
   enum EstadoOperar{
         INICIO_OPERAR,
         TOR_OPT,
         MODO_TOR,
         CONTROLAR,
         ACTIVAR_VALVULA
      } estadoOperar;
   enum EstadoCalibrar{
         INICIO_CALIBRAR,
         TOR_MODO_1,
         TOR_MODO_2,
         ENCODER,
         VALVULA
      } estadoCalibrar;
}estadoSistema={INICIO,INICIO_OPERAR,INICIO_CALIBRAR};

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void setupHardware(void) {
#if defined (__USE_LPCOPEN)
#if !defined(NO_BOARD_LIB)
   // Read clock settings and update SystemCoreClock variable
   SystemCoreClockUpdate();

   // Inicia board led, usb y botones
   Board_Init();

   // Inicia la UART
   NewMiuart();

   // Apaga la valvula
   Board_LED_Set(VALVULA_PIN, false);

   // Inicia ADC
   NewMiadc();
#endif
#endif
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void){
   if (contRx++ == 3) {
      flagRx = 1; //Tarea programada cada 30 ms
      contRx = 0;
   }
   if (contTx++ >= ticksTx) {
      flagTx = 1; //Tarea programada cada 500 ms
      contTx = 0;
   }
   if (contCelda++ == 2) {
      flagCelda = 1; //Tarea programada cada 20 ms
      contCelda = 0;
   }
   if (contValvula++ == 101) {
      flagValvula = 1; //Tarea programada cada 1001 ms
      contValvula = 0;
   }
   if (contEnco++ == 1) {
      flagEnco = 1; //Tarea programada cada 10 ms
      contEnco = 0;
   }
}


int main(void)
{
   setupHardware();

   SysTick_Config( F_CPU / TICKRATE_HZ);

   bufferIn.len=0;

   miuartPrintLn("Iniciando programa ...");

   while (1) {
      if (flagRx) {
         rx();
         flagRx = 0;
      }
      if (flagCelda) {
         celda();
         verificaOperacion();
         flagCelda = 0;
      }
      if (flagTx) {
         tx();
         flagTx = 0;
      }
      if (flagValvula) {
         valvula();
         flagValvula = 0;
      }
      if (flagEnco) {
         encoder();
         flagEnco = 0;
      }
   }
   return 0;
}

//----- Tareas ----
/**
 * Funcion para ejecutar la tarea de recepcion.
 */
void rx() {
   if (miuartRxLn(&bufferIn)) {
      char cmd[bufferIn.len+1];
      BufferToStr(&bufferIn,&cmd);

      switch (estadoSistema.estadoModo) {
      case INICIO:
         if (strEqual(cmd,"calibrar")) {
            estadoSistema.estadoModo = CALIBRAR;
            estadoSistema.estadoCalibrar = INICIO_CALIBRAR;
         } else if (strEqual(cmd,"operar")) {
            estadoSistema.estadoModo = OPERAR;
            estadoSistema.estadoOperar = INICIO_OPERAR;
         }
         break;
      case OPERAR:
         switch (estadoSistema.estadoOperar) {
         case TOR_OPT:
            torOpt = strToInt(cmd);
            flagRecibido = 1;
            break;
         case MODO_TOR:
            modoTor = strToInt(cmd);
            flagRecibido = 1;
            break;
         case CONTROLAR:
            if (strEqual(cmd,"detener")) {
               flagRecibido = 1;
            }
            break;
         default:
            break;
         }
         break;
      default:
         break;
      }
      bufferIn.len=0;
   }
}

/**
 * Funcion para ejecutar la tarea de transmicion.
 */
void tx(void){
   switch (estadoSistema.estadoModo) {
   case INICIO:
      enviarDatos();
      break;
   case CALIBRAR:
      switch (estadoSistema.estadoCalibrar) {
      case INICIO_CALIBRAR:
         miuartPrintLn("=========== Inicia calibracion ==============");
         miuartPrintLn("****** TEST Torque 1 *********");
         estadoSistema.estadoCalibrar = TOR_MODO_1;
         contAux = 0;
         modoTor=0;
         break;
      case TOR_MODO_1:
         miuartPrint("Valor de torque 1: ");
         char auxValAdc[10];
         intToStr(auxValAdc, adcVal_1);
         miuartPrintLn(auxValAdc);
         if (contAux++ == 10) {
            miuartPrintLn("****** TEST Torque 2 *********");
            estadoSistema.estadoCalibrar = TOR_MODO_2;
            contAux = 0;
            modoTor=1;
         }
         break;
      case TOR_MODO_2:
         miuartPrint("Valor de torque 2: ");
         char auxValAdc2[10];
         intToStr(auxValAdc2, adcVal_2);
         miuartPrintLn(auxValAdc2);
         if (contAux++ == 10) {
            miuartPrintLn("****** TEST ENCODER *********");
            estadoSistema.estadoCalibrar = ENCODER;
            contEncoder = 1;
            contAux = 0;
            modoTor=0;
         }
         break;
      case ENCODER:
         miuartPrint("Valor de Encoder: ");
         char auxValEncoder[10];
         intToStr(auxValEncoder, contEncoder);
         miuartPrintLn(auxValEncoder);
         if (contAux++ == 10) {
            miuartPrintLn("****** TEST Valvula *********");
            //pulsosVuelta = contEncoder;
            estadoSistema.estadoCalibrar = VALVULA;
            contAux = 0;
            segValvula = 0;
            miuartPrintLn("Valvula activada !!!");
         }
         break;
      case VALVULA:
         if (segValvula >= (SEG_VALVULA)) {
            estadoSistema.estadoModo = INICIO;
            estadoSistema.estadoCalibrar = INICIO_CALIBRAR;
            miuartPrintLn("Valvula desactivada !!!");
            miuartPrintLn("=========== Fin de la calibracion ==============");
         }
         break;
      default:
         break;
      }
      break;
   case OPERAR:
      switch (estadoSistema.estadoOperar) {
      case INICIO_OPERAR:
         miuartPrintLn("=========== Inicia operar ==============");
         miuartPrint("Ingrese torque optimo: ");
         estadoSistema.estadoOperar = TOR_OPT;
         flagRecibido = 0;
         ticksTx=9;
         break;
      case TOR_OPT:
         if (flagRecibido == 1) {
            estadoSistema.estadoOperar = MODO_TOR;
            flagRecibido = 0;
            miuartPrint("Torque optimo =  ");
            char auxValTor[10];
            intToStr(auxValTor, torOpt);
            miuartPrintLn(auxValTor);
            miuartPrintLn("Ingrese modo: ");
         }
         break;
      case MODO_TOR:
         if (flagRecibido == 1) {
            estadoSistema.estadoOperar = CONTROLAR;
            flagRecibido = 0;
            contEncoder = 0;
            miuartPrint("Modo =  ");
            char auxValTor[10];
            intToStr(auxValTor, modoTor);
            miuartPrintLn(auxValTor);
            miuartPrintLn("Controlando...");
         }
         break;
      case CONTROLAR:
         enviarDatos();
         if (flagRecibido == 1) {
            estadoSistema.estadoModo = INICIO;
            estadoSistema.estadoOperar = INICIO_OPERAR;
            miuartPrintLn("=========== Fin de operar ==============");
            ticksTx=90;
         }
         break;
      case ACTIVAR_VALVULA:
         enviarDatos();
         if (segValvula >= (SEG_VALVULA)) {
            estadoSistema.estadoModo = INICIO;
            estadoSistema.estadoOperar = INICIO_OPERAR;
            miuartPrintLn("Valvula desactivada !!!");
            miuartPrintLn("=========== Fin de operar ==============");
            ticksTx=90;
         }
         break;
      default:
         break;
      }
      break;
   default:

      break;
   }
}

/**
 * Funcion para ejecutar la tarea para leer las celdas.
 */
void celda(void){
   if(modoTor==0){
      adcVal_1=leerMiadc(modoTor);
   }else{
      adcVal_2=leerMiadc(modoTor);
   }
}

/**
 * Funcion para ejecutar la tarea de verificacion del modo de OPERAR.
 */
void verificaOperacion(void) {
   switch (estadoSistema.estadoModo) {
   case OPERAR:
      switch (estadoSistema.estadoOperar) {
      case CONTROLAR:
         if ((modoTor == 0 ? adcVal_1 : adcVal_2) >= torOpt) {
            segValvula = 0;
            estadoSistema.estadoOperar = ACTIVAR_VALVULA;
         }
         break;
      default:
         break;
      }
      break;
   default:
      break;
   }
}

/**
 * Funcion para ejecutar la tarea de la valvula.
 */
void valvula(void) {
   if (segValvula < (SEG_VALVULA)) {
      Board_LED_Set(VALVULA_PIN, true);
      segValvula++;
      stateValvula = 1;
   } else {
      Board_LED_Set(VALVULA_PIN, false);
      stateValvula = 0;
   }
}

/**
 * Funcion para ejecutar la tarea para leer los pulsos del encoder.
 */
void encoder(void){
   uint32_t valor=Buttons_GetStatus(BOTON_PULSO);
   if(valor==1 && estadoPulso==0){
      contEncoder++;
      estadoPulso=1;
   }
   else if(valor==0 && estadoPulso==1){
      estadoPulso=0;
   }
}

//-------------- Otros
/**
 * Funcion para enviar un paquete de datos: Celda, Encoder y estado de valvula.
 */
void enviarDatos(void) {
   miuartPrint("Valor ADC ");
   miuartPrint(modoTor == 0 ? "1: " : "2: ");
   char auxValTor[10];
   intToStr(auxValTor, modoTor == 0 ? adcVal_1 : adcVal_2);
   miuartPrintLn(auxValTor);
   miuartPrint("Valor Encoder: ");
   char auxValEnc[10];
   intToStr(auxValEnc, contEncoder);
   miuartPrintLn(auxValEnc);
   miuartPrint("Estado de la valvula: ");
   miuartPrintLn(stateValvula == 1 ? "Activada" : "Desactivada");
}

/*==================[end of file]============================================*/
