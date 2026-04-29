/**********************************************************************
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Sebastián Wahler  23/03/2016  Primer release - Interrupciones
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES: 
*
**********************************************************************/
#include "p33FJ256GP710.h"
#include "config.h"

//Configuracion de valores para el timer //
#define FCY 40000000  //el projecto ya viene configurado a 40mhz//
#define PR1_BS = 749;  //150 us serian con esta escala 
#define PR1_MAX (PR1 * 6); // 4499 en el PR1 serian 900us

extern char caracteres[];
//El apuntador apunta a la ultima posición del arreglo donde puso un caracter
extern int apuntador;
extern int length;
volatile int aux;
/*
 * Rutina de Atención de la interrupción externa INT0
 */
void __attribute__((interrupt, auto_psv)) _INT1Interrupt( void )
{
	/* reset INT1 interrupt flag */
 	IFS1bits.INT1IF = 0;
    //El caracter que entra por PORTB
    char c = PORTB;
    
    //Verificar si esta en el rango de caracteres posibles.
    if ((c >= '0' && c <= '9') ||
    c == '+' || c == '-' || c == '*' || c == '/'){
        //Verifica donde esta el apuntador, en el caso de estar al final pone el 
        //caracter al inicio o lo pone en la posicion siguiente
        if(apuntador > length){
            apuntador = 0;
        } else {
            apuntador++;
        }
        caracteres[apuntador] = c;
        
    }
    //borrar al final.    
    //counterINT0++;
}

}
/*
 * Rutina de Atención de la interrupción del Timer1
 */
void __attribute__((interrupt, auto_psv)) _T1Interrupt( void )
//no me acuerdo bien si eran ni bien salte la rutina de atencion o si era dps asi que lo deje aca
IFS0bits.T1IF = 0;

{
	int puerto = PORTB;

        if (aux != puerto){  //si el valor es distinto..
            aux = puerto;
            PR1 = PR1_BS;
}
        else {

    if (PR1 < PR1_MAX){ // si PR1 es menor a 900us,aumenta de 150us en 150us con tope de 900us
	
        PR1+=PR1_BS; // 150us,300us,450...900us//
   }
     else{
		 
        PR1 = PR1_MAX; //tope maximo de 900

   }
 }
}

/*---------------------------------------------------------------------
  Function Name: Init_Timer1
  Description:   Initialize Timer1
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Init_Timer1( void )
{
//Configurar Timer1
T1CONbits.TON = 0;
T1CONbits.TCS = 0;
T1CONbits.TCKPS = 1;  // (00) 1:1 (01) 1:8 (10) 1:64 (11) 1:256 // si mal no me equivoco,usaron 1:256 y por eso daba 23 en PR1,lo cambie de rompehuevos nomas
PR1 = 749;
//Configurar Interrupción.
IPC0bits.T1IP = 1; // Prioridad 1
IFS0bits.T1IF = 0;
IEC0bits.T1IE = 1;
T1CONbits.TON = 1; //start 
}

void Init_INT1( void )
{
    /* reset INT1 interrupt flag */
    IFS1bits.INT1IF = 0;

    /* set INT1 interrupt priority level */
    IPC5bits.INT1IP = 5;

    /* enable INT1 interrupt */
    IEC1bits.INT1IE = 1;
}
void config( void )
{
	TRISB = 0xFFFF; //Todo como entrada
    aux = PORTB;
    /* Inicializar Interrupción Externa INT1 */
    Init_INT1();
	/* Inicializar Timers necesarios */
	Init_Timer1();
}

