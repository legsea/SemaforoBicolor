/*

	12/03/2019 8:51:13
	
	- Inicio del Programa
	- PIC16F886 @4Mhz Reloj Interno

	19/03/2019 13:01:52
	
	- Puesta en marcha de la placa. 
	
	R00: Contiene el programa base previo a todas las pruebas que se haran despues.

	20/03/2019 14:39:56
	
	R01: Hasta aca funciona barrido, deteccion de remoto, muestra en el display, etc.
				Proximo: Hacer las funciones del remoto Master.

******************** OBSOLETO ****************************************************
	R02: Funciones del Remoto OK. 
				- Boton A: Incrementa de a 1 hasta que se pulsa A o B
				- Boton B: Decrementa de a 1 hasta que se pulsa A o B
				- Boton C: Pone a Cero
				- Boton D: Incrementa 1 unidad y para.	
******************** OBSOLETO ****************************************************

06/06/2019 11:00:35

	R03: Se modificaron las funciones del remoto master, ahora:
	
				- Boton A: Incrementa de a 1 
				- Boton B: Decrementa de a 1 
				- Boton C: Pone a Cero
				- Boton D: Incrementa 10 unidades y para.	


	R04: Se crea un buffer de puestos de llamada. Si un puesto llama a un numero y se demora
			 y otro puesto llama, se puede rellamar desde el puesto anterior y mostrara el numero
			 al que se llamo.	-> 15/08/2019 8:24:43 : No se urara hasta nuevo release.


15/08/2019 8:28:53

	R05: Vamos a modificar la rutina de procesar remoto. El turnero presenta algunos fallos
			 que no deberia presentarse.


17/10/2019 9:35:03

	R06: Se corrige problema con el modo de stand by. Sigue en prueba-
	
	
	
30/10/2019 13:12:52

	R07: Se mejora la respuesta del remoto de Servicio. El problema era que tardaba
			 mucho en responder. 
			 Ahora funciona mucho mejor.
			 	
	
	

*/


// *********************************************************************************************
//	Estructura de los Datos
// *********************************************************************************************

/*
	DATA_0 : Cabecera 
	DATA_1 : Boton Presionado
	DATA_2 : Puesto invertivo o cero si es master	 	
	DATA_3:  Puesto o Funcion si es master
	
*/

#define Cabecera 0x03		
#define TAPAGADO 3600	//equivale a 1 hora


#include <htc.h>
#include "16F886.h"

// Constantes de uso general----------

unsigned char PDel1;         //Para el delay
unsigned char uni,dec,cen;
unsigned char uni_p,dec_p;
unsigned char t_display;
unsigned char pd;
unsigned char mx;		//Variable de Multiplexado


unsigned char puesto=0;
unsigned char i=0;

unsigned int contador=0;
unsigned char contador_h,contador_l;



unsigned char bajos=0;
unsigned char dur0=0;
unsigned char count_bits=0;                     // Cuenta los bits que entran    
unsigned char DATA_0=0;
unsigned char DATA_1=0;
unsigned char DATA_2=0;
unsigned char DATA_3=0;   //

unsigned char Buffer_0,Buffer_1=0;                  // Auxiliares para comparar lo que entra
unsigned char Buffer_2,Buffer_3=0;
unsigned char temp1,temp2,temp3,temp4=0;  //Registros Temporales proposito general


unsigned char boton=0;	// La tecla del remoto que se presionó

unsigned char t_1seg;
unsigned char t_500mseg;
unsigned char t_buzzer=0;
unsigned char t_100mseg=0;
unsigned char t_freeze=0;
unsigned char t_timeout=0;
unsigned char c_beeps=0;
unsigned char c_veces=0;
unsigned char t_tempbuzzer=0;
unsigned char t_pausa=0;
unsigned char t_ledrx;
unsigned char t_test1;
unsigned char t_test2;
unsigned int  t_apagado;

bank1 unsigned int lastcall[10];
bank1 unsigned char lastbox[10];
bank1 unsigned char backup_contador;


bit flag_pd=0;
bit flag_repeticion=0;
bit flag_blank=0;
bit flag_leerbajo=0;
bit flag_bajo20ms=0;
bit flag_newflanco=0;
bit flag_procpaq=0;                     // Indica que hay que procesar los bits que entraron
bit flag_bitOK=0;                        // Indica que entro un codigo correcto
          
bit flag_bufferfull=0;               //Indica que el buffer de primera pasada esta lleno y listo para comparar   
bit flag_remotoOK=0;                  // Encontro un remoto y lo puede procesar   
bit flag_mismatch=0;                  // No son iguales los codigos.
bit flag_cabecera=0;


bit flag_lastA=0;
bit flag_lastB=0;
bit flag_lastC=0;
bit flag_lastD=0;
bit flag_freeze=0;
bit flag_beeps=0;
bit flag_precargabuzzer=0;
bit flag_pausa=0;
bit flag_timers=0;
bit flag_master;					//Usado para que no muestre puesto
bit flag_nobeep;					//flag para que no haga los beep, si es el master por ejemplo

bit flag_guardar;
bit flag_descartar;
bit flag_apagado;
bit flag_noshowpuesto;



void device_init (void)
{ 

// Configuracion interrupciones globales (Todas!!!)
    GIE = 1;	// Interrupciones globales
    PEIE = 1;	// Interrupciones de perifericos    
    T0IE = 0;	// Interrupciones por Timer0
    INTE = 1;	// Interrupciones por flanco
    RBIE = 0;	// Interrupcion por RB
    T0IF = 0;
    INTF = 0;
    RBIF = 0; 	
     
		INTEDG=1;
		
		
// Configuracion de CCP
    CCP1IE = 0;	// Interrupciones por CCP (desactivado)
		CCP2IE = 0;
		CCP1CON=0x00;
		CCP2CON=0x00;


// Configuracion de Timer
// Timer0
		T0CS = 0;	// Contador en el pin T0CKI = 1; Reloj interno = 0;
		T0SE = 1;	// Transicion de bajo a alto = 0;
		PSA = 0;	// Pre escaler asignado a: 0=TMR0/1=WDT;
		PS2 = 0;
		PS1 = 0;	// Pre escaler 1:4
		PS0 = 1;

// Timer1
		T1CKPS1 = 0;	// Pre escaler 1:4
		T1CKPS0 = 0;	// Pre escaler 1:4
		T1OSCEN = 0;	// 
		nT1SYNC = 1;		// 
		TMR1CS = 0;		// Timer1 como reloj interno
    TMR1IE = 1;		// Interrupciones por Timer1
    TMR1ON = 1;		// Timer 1 encendido
 		TMR1H=0xD8;
		TMR1L=0xF0;
		

// Timer2 
    TMR2IE = 1;		// Interrupciones por Timer2
    TOUTPS3 = 0;
    TOUTPS2 = 0;
    TOUTPS1 = 0;	//Bits de Post escaler
    TOUTPS0 = 0;
    TMR2ON = 0;		//Enciendo el timer2
    T2CKPS1 = 1;
    T2CKPS0 = 1;	// Pre escaler 1:4
    TMR2   = 1;
		PR2=110;		//Para que interrumpa cada 1mseg aproximadamente
	
// Comparadores			
  
  	CM1CON0=0b00000000;
  	CM2CON0=0x00;
  
  
// Configuracion puertos (1=entrada y 0=salida)
// Puerto A
    
    PORTA = 0x00;
    TRISA = 0b00000000;

		ANSEL = 0b00000000;				//  Analog Inputs "1" Set

// Puerto B
  
    nRBPU = 0;							//Pull ups enable
    PORTB = 0x00;
    TRISB = 0b00000001;
    
    ANSELH=0x00;
    
// Puerto C
    
    PORTC = 0x00;
    TRISC = 0b00000000;
    
;


// Modulo PWM

    CCP1CON=0x00;

	//OSCCON = 0b01110000;			//Pic trabajando con Osc. Interno a 8 Mhz

}

void Delay_50us(void)
{

   #asm
   PLoop12: movlw     0x07      ; 1 set numero de repeticion  A
           movwf     _PDel1     ; 1 |
   PLoop22: clrwdt              ; 1 clear watchdog
           clrwdt              ; 1 ciclo delay
           decfsz    _PDel1,f  ; 1 + (1) es el tiempo 0  ? (A)
           goto      PLoop22    ; 2 no, loop
   PDelL12: goto PDelL22         ; 2 ciclos delay
   PDelL22: clrwdt
   #endasm

}


void Delayms(unsigned int t)
{
	unsigned char x;
	
	while(t)
	{
		x=20;
		while(x)
		{
			Delay_50us();
			x--;
			
		}	

		t--;
		
	}


}

void beep(void)
{
   Buzzer=1;
   Delayms(100);
   Buzzer=0;
}

void eeprom_init(void)
{
	unsigned char x;
	
	x=EEPROM_READ(0x7F); 
	
	if(x!=0x33)
	{
		contador=0;
		
		contador_l=contador&0xFF;
		contador_h=contador>>8;
		
		EEPROM_WRITE(0x7F,0x33);
		
		EEPROM_WRITE(0x05,contador_h);
		EEPROM_WRITE(0x06,contador_l);
		
		//EEPROM_WRITE(0x01,decenas);
		
		
	}
}


void var2num(unsigned char temp)
{
	switch(temp)
	{
		//    				 fpgabcde		
		case 0:PORTA=0b10011111;		//Muestra un Cero
			break;
		case 1:PORTA=0b00001100;		//Muestra un Uno
			break;
		case 2:PORTA=0b00111011;
			break;
		case 3:PORTA=0b00111110;
			break;
		case 4:PORTA=0b10101100;
			break;
		case 5:PORTA=0b10110110;
			break;
		//    				 fpgabcde
		case 6:PORTA=0b10110111;
			break;
		case 7:PORTA=0b00011100;
			break;
		case 8:PORTA=0b10111111;
			break;
		case 9:PORTA=0b10111100;
			break;
		//						  fpgabcde	
		case 10:PORTA=0b10111101;		//A
			break;
		case 11:PORTA=0b10100111;		//b
			break;
		case 12:PORTA=0b10110011;		//C
			break;
		case 13:PORTA=0b00101111;		//d
			break;
		case 14:PORTA=0b10110011;		//E
			break;
		case 15:PORTA=0b10110001;		//F
			break;
		case 16:PORTA=0b00100000;		// - (Guion)
			break;			
		case 20:PORTA=0b00000000;	//Nada
			break;	
			
	}
	
	if(flag_blank)
		PORTA=0b00000000;
	
	if(flag_pd)
	{
		PORTA|=0b01000000;
		
		
	}	
		

}

void reset_registros(void)
{
  count_bits=32;
  flag_bajo20ms=0;
  flag_leerbajo=1;
  flag_bitOK=0;
  flag_procpaq=0;
  DATA_0=0;
  DATA_1=0;
  DATA_2=0;
  DATA_3=0;
  Led_Rx=0;
  Test2=0;
  flag_freeze=0;
  flag_master=0;
  flag_descartar=0;
}

void Display(void) 
{
	 cen=contador/100;							//Saca Centena
	 dec=(contador-cen*100)/10;			//Saca decena
	 uni=(contador-cen*100-dec*10);	//Saca unidades

//Apaga los digitos si estan en cero
	 if(!cen)
			cen=20;	// Si es cero, estan apagados 
	 if(!dec && cen==20)
	 		dec=20; // Si es cero, estan apagados			

		if(flag_descartar)
		{
			uni_p=20;
			dec_p=20;
			
		}	



}

void check_master(void)
{
	flag_descartar=1;


	if(boton==0xDD && !flag_lastD)//  && puesto==0xDD)
	{
		flag_apagado=0;
		flag_blank=0;
		contador+=10;
		if(contador>999)
			contador=0;
		flag_nobeep=1;
		flag_master=1;
		flag_guardar=1;
		t_freeze=5;
		puesto=0;
		Display();
		flag_lastD=1;
		flag_descartar=0;

		return;	
	}

		
	if(boton==0xAA && !flag_lastA)// && puesto==0xAA)
	{
		flag_apagado=0;
		flag_blank=0;
		contador++;
		if(contador>999)
			contador=0;
		flag_nobeep=1;
		flag_master=1;
		flag_guardar=1;
		t_freeze=5;
		puesto=0;
		Display();
		flag_lastA=1;
		flag_descartar=0;

		return;
			
	}	

	if(boton==0xBB && !flag_lastB)// && puesto==0xBB)
	{
		flag_apagado=0;
		flag_blank=0;
		if(!contador)
			contador=999;
		else
			contador--;

		flag_nobeep=1;
		flag_master=1;
		flag_guardar=1;
		t_freeze=5;
		puesto=0;
		Display();
		flag_lastB=1;
		flag_descartar=0;

		return;

	}	
	
	if(boton==0xCC && !flag_lastC)//  && puesto==0xCC)
	{
		flag_apagado=0;
		flag_blank=0;
		flag_lastC=1;
		contador=0;
		puesto=0;
		Display();
		flag_nobeep=1;
		flag_master=1;
		flag_guardar=1;
		t_freeze=5;	
		flag_descartar=0;

		return;			
	}	


	

}

void llamada(void)
{

	flag_nobeep=0;			//Vino bien el puesto, es valido el codigo
	flag_master=0;			//no es master
	flag_descartar=1;		// No descartar
	
/*	
	//Hacemos una copia de la matrix de llamadas
	for(i=0;i<9;i++)	
	{
		lastbox[i]=lastbox[i+1];
		lastcall[i]=lastcall[i+1];
	}
*/
		contador_h=EEPROM_READ(0x05);
		contador_l=EEPROM_READ(0x06);
	
		contador=contador_h*256+contador_l;	
		
	if(boton==0xAA && !flag_lastA)// && puesto!=0xAA && !flag_master)
	{

		contador++;
		if(contador>999)
			contador=0;
		
		//lastcall[9]=contador;
		//lastbox[9]=puesto;
		
		t_freeze=40;
		flag_freeze=1;
		flag_lastA=1;
		flag_guardar=1;
		puesto=DATA_3;				// Aca esta el puesto
		Display();
		flag_descartar=0;
		return;
			
	}




	
}

void search(void)
{
	i=9;
	while(i)
	{
		if(lastbox[i]==puesto)
		{
			contador=lastcall[i];
			break;
		}	
		i--;	
	} 

}


void procesar_remoto(void)
{
	unsigned char aux_puesto;
	
	boton=DATA_1&0xFF;		//Aca esta el boton presionado


	flag_nobeep=0;			//Vino bien el puesto, es valido el codigo
	flag_master=0;			//no es master
	flag_descartar=1;		// No descartar
	flag_apagado=0;
	flag_blank=0;	
/*	
	//Hacemos una copia de la matrix de llamadas
	for(i=0;i<9;i++)	
	{
		lastbox[i]=lastbox[i+1];
		lastcall[i]=lastcall[i+1];
	}
*/
	//	contador_h=EEPROM_READ(0x05);
	//	contador_l=EEPROM_READ(0x06);
	
	//	contador=contador_h*256+contador_l;	
		
		//if(!DATA_2)
			//flag_master=1;
			
		aux_puesto=DATA_2;				//chequea el puesto
		asm("swapf _DATA_2,f");		//viene invertido para seguridad
	
		aux_puesto=DATA_2-DATA_3;	// si viene invertido, da cero y esta OK el puesto			
		
		
		if(aux_puesto)
			flag_master=1;
		
		
	if(boton==0xAA && !flag_lastA)// && puesto!=0xAA && !flag_master)
	{

		contador++;
		if(contador>999)
			contador=0;
		
		//lastcall[9]=contador;
		//lastbox[9]=puesto;
		
		t_freeze=40;
		flag_freeze=1;
		flag_lastA=1;
		flag_guardar=1;
		puesto=DATA_3;				// Aca esta el puesto
		
		if(flag_master)
		{
			flag_nobeep=1;
			puesto=0;
			t_freeze=20;
			flag_guardar=0;

		}	
		
		Display();
		flag_descartar=0;
		return;
			
	}

	if(boton==0xBB && !flag_lastB)// && puesto!=0xBB && !flag_master)
	{
		//contador=lastcall[9];

		//lastcall[9]=contador;
		//lastbox[9]=puesto;
		t_freeze=40;
		flag_freeze=1;
		flag_lastB=1;
		flag_guardar=1;
		puesto=DATA_3;				// Aca esta el puesto

		if(flag_master)
		{
			flag_nobeep=1;
			puesto=0;
			t_freeze=20;
			if(!contador)
				contador=999;
			else
			contador--;
			flag_guardar=0;
		}	
		else
		{
			contador++;
			if(contador>999)
				contador=0;
			
		}
	

	
		Display();
		flag_descartar=0;
		return;
	}	


	if(boton==0xCC && !flag_lastC)// && puesto!=0xCC && !flag_master)
	{
		//search();
		t_freeze=40;
		flag_freeze=1;
		flag_lastC=1;
		puesto=DATA_3;				// Aca esta el puesto
		if(flag_master)
		{
			flag_nobeep=1;
			puesto=0;
			//t_freeze=20;
			contador=0;
			//flag_guardar=1;
		}	
		Display();
		flag_descartar=0;
		return;
	}	

	if(boton==0xDD && !flag_lastD)// && puesto!=0xDD && !flag_master)
	{
		//search();
		t_freeze=40;
		flag_freeze=1;
		flag_lastD=1;
		puesto=DATA_3;				// Aca esta el puesto
		if(flag_master)
		{
			flag_nobeep=1;
			puesto=0;
			t_freeze=25;
			contador+=10;
			if(contador>999)
				contador=0;
		//	flag_guardar=1;
		}	
		Display();
		flag_descartar=0;
		return;
	}				






/*


		
	t_apagado=TAPAGADO;		//Reset del tiempo de standby
	flag_apagado=0;


	
	if(flag_master)
		return;
		
		


	if(!aux_puesto)
		llamada();
	
	//if(aux_puesto && !DATA_2)
		//check_master();	
*/		
}


void procesar_paquete(void)
{

	flag_procpaq=0;
	INTE=0;


   if(!flag_bufferfull)
   {
      Buffer_0=DATA_0;
      Buffer_1=DATA_1;
      Buffer_2=DATA_2;
      Buffer_3=DATA_3;
      flag_bufferfull=1;
      reset_registros();
      INTE=1;
      return;
   }

   flag_mismatch=0;
   
   temp1=Buffer_0^DATA_0;   
   
   if(temp1)
      flag_mismatch=1;
      
   temp2=Buffer_1^DATA_1;
      
   if(temp2 && !flag_mismatch)   
      flag_mismatch=1;
   
   temp3=Buffer_2^DATA_2;
      
   if(temp3 && !flag_mismatch)   
      flag_mismatch=1;

   temp4=Buffer_3^DATA_3;
   
   if(temp4 && !flag_mismatch)   
      flag_mismatch=1;
      
   if(flag_mismatch)
   {
      Buffer_0=DATA_0;
      Buffer_1=DATA_1;
      Buffer_2=DATA_2;
      Buffer_3=DATA_3;      
      reset_registros();
      INTE=1;
      flag_bufferfull=0;
      return;         
   }      


	if(DATA_0==Cabecera)
	{
		flag_remotoOK=1;
	  flag_bufferfull=0;
		t_timeout=50;
		Test1=1;
		//t_test1=10;	
		//t_ledrx=10;
		Led_Rx=1;
		t_ledrx=20;
		flag_apagado=0;
		t_apagado=TAPAGADO;
	}
	else
	{
		reset_registros();
		INTE=1;
		//t_buzzer=1;
		//Buzzer=1;
		Test1=0;
	}		
	



}


void graba(void)
{
			//Test2=!Test2;
      #asm
            RRF _DATA_0,f
            RRF _DATA_1,f
            RRF _DATA_2,f
            RRF _DATA_3,f

      #endasm

      flag_bitOK=1;         

}


void procesar_flanco(void)
{
   flag_newflanco=0;
   
   if(dur0>25 && dur0<90)		//40-90
   {
      CARRY=1;
     	//Led_Rx=0;
     	Test2=0;
      graba();

   }

   if(dur0>90 && dur0<150)	//90-150
   {
      CARRY=0;
     	//Led_Rx=1;
     	Test2=1;
      graba();
   }   
   
   if(dur0<25 || dur0>150)//40-150
   {
      reset_registros();         
    	flag_bufferfull=0;
    	//Buzzer=0;
      Buffer_0=0;
      Buffer_1=0;
      Buffer_2=0;
      Buffer_3=0;
      flag_lastA=0;
      flag_lastB=0;
      flag_lastC=0;
      flag_lastD=0;
    
   }         

   if(flag_bitOK)
   {
      count_bits--;         

      if(!count_bits)   
      {
         flag_procpaq=1;
         return;
      }
   }

}


void leer_bajo(void)
{
   if(!Data_In && !flag_bajo20ms)   
   {
      bajos++;
      Delay_50us();
      
      if(bajos>80)
      {
         bajos=0;
         flag_bajo20ms=1;
         flag_leerbajo=0;
         return;
      }   
      
   }


   if(Data_In && !flag_bajo20ms)
   {
      bajos=0;
      flag_leerbajo=1;
      flag_bajo20ms=0;
	 }   

}


void leer_remoto(void)
{
   
   if(flag_leerbajo)
      leer_bajo();

   if(flag_bajo20ms)
   {
      if(flag_newflanco)
         procesar_flanco();

      if(flag_procpaq)
      {
         procesar_paquete();
         
      }   
  
   }

}

unsigned int bin2bcd(unsigned int input)
{
	
	unsigned int output;
	signed char a;
	
	for(a = 13; a >= 0; a--)
	{
    if((output & 0xF) >= 5)
        output += 3;
    if(((output & 0xF0) >> 4) >= 5)
        output += (3 << 4);
    if(((output & 0xF00) >> 8) >= 5)
        output += (3 << 8);
    output = (output << 1) | ((input >> a) & 1);
  }
	
	return output;	
	
	
}

void Timers(void)
{
	flag_timers=0;
	
	if(flag_beeps)			//Rutina de control de beeps
	{

		if(flag_precargabuzzer)
		{
			t_buzzer=15;
			flag_precargabuzzer=0;

		}	

		if(!flag_pausa)
		{
			Buzzer=1;
			t_buzzer--;
			if(!t_buzzer)
			{
				Buzzer=0;
				flag_pausa=1;
				t_pausa=20;	
				flag_blank=1;
			}		
		}
		else 
		{
			Buzzer=0;
			t_pausa--;
			
			if(!t_pausa)
			{
				flag_pausa=0;
				c_beeps--;
				
				flag_blank=0;
							
				if(!c_beeps)
				{
					c_veces--;
					
					if(!c_veces)
					{
						flag_beeps=0;
						//INTE=1;						
					}
					else 
					{
						flag_precargabuzzer=1;
						c_beeps=5;
						flag_pausa=1;
						t_pausa=80;
						//INTE=0;
					}	
				}	
				else 
				{
					flag_precargabuzzer=1;
				}						
			}
		}
	}
		

	t_100mseg++;
	
	if(t_100mseg==10)			// Cosas para hacer cada 100 mseg
	{
		t_100mseg=0;

		
		if(t_buzzer && !flag_beeps)
		{
			t_buzzer--;
			if(!t_buzzer)
				Buzzer=0;
			
		}				


		if(t_ledrx)
		{
			t_ledrx--;
			
			if(!t_ledrx)
				Led_Rx=0;

		}	

		if(t_test2)
		{
			t_test2--;
			
			if(!t_test2)
				Test2=0;
			
		}	

		if(t_test1)
		{
			t_test1--;
			
			if(!t_test1)
				Test1=0;
			
		}	

		if(t_freeze)
		{
			t_freeze--;
			if(!t_freeze)
			{
				flag_freeze=0;
				INTE=1;	
			}
			
		}				

			if(t_timeout && !flag_freeze)
			{
				t_timeout--;
				if(!t_timeout)
				{
					reset_registros();
					INTE=1;
				}	

			}	


		t_500mseg++;
		
		
		if(t_500mseg==5)	// Pasaron 500mseg
		{
			t_500mseg=0;



		}
		
		t_1seg++;
		if(t_1seg==10)		// Paso 1 segundo
		{
			t_1seg=0;

			if(t_apagado)
			{
				t_apagado--;
				if(!t_apagado)
				{
					flag_apagado=1;
				}		
			
			}		
			
			if(flag_apagado)
			{
				flag_blank=!flag_blank;
			}	
			
		}

	}				

}


void interrupt interrupciones(void)   // Interrupciones
{

	if(INTE && INTF)
	{
	   
	   if(flag_bajo20ms)
	   {
	      if(Data_In)
	      {
	         TMR0=0;
	         INTEDG=0;
	      }   
	      else
	      {
	         dur0=TMR0;
	         flag_newflanco=1;
	         INTEDG=1; 
	      }            
	   }   
	      
	   INTF=0;
	}   
	
	if(TMR1IE && TMR1IF)	//Preparado para interrumpir cada 10mseg
	{
		
		TMR1IF=0;
		
		flag_timers=1;	


		TMR1H=0xD8;
		TMR1L=0xF0;

	}

	if(TMR2IE && TMR2IF)	// Interrupciones para display
	{
		
		mx++;
		if(mx==12)
			mx=1;

		if(mx<7)
		{
			
					
		//	Test1=!Test1;	
			 if(flag_apagado && !flag_remotoOK)
			 {
			 	uni_p=20;
			 	dec_p=20;
			 	uni=16;
			 	dec=cen=20;
			 }
			 else	
			 if(!flag_master && !flag_descartar)
			 {
				 
				 //flag_blank=0;
				 dec_p=puesto&0xF0;
				 dec_p>>=4;
			
				 uni_p=puesto&0x0F;
				 if(!uni_p && !dec_p)
				 	uni_p=20;
				 
				 if(!dec_p)
				 	dec_p=20;			
			 }	


			 if(flag_cabecera)
			 {
			 	uni=Cabecera&0x0F;
			 	dec=Cabecera&0xF0;
			 	dec>>=4;
			 }	
			 
			 mx_1=0;
			 mx_2=0;
			 mx_3=0;
			 mx_4=0;
			 mx_5=0;
			 
			
			
			switch(mx)
			{
			  case 1:   var2num(uni);
			            mx_1=1;
			            break;

			  case 2:   var2num(dec);
			            if(!flag_apagado)
			            	mx_2=1;
			            break;

			  case 3:   var2num(cen);
			            if(!flag_cabecera && !flag_apagado)
			            	mx_3=1;   
			            break; 

				case 4:	var2num(uni_p);
								if(!flag_cabecera && !flag_apagado)
									mx_4=1;
								break;
								
				case 5: var2num(dec_p);
								if(!flag_cabecera && !flag_apagado)
									mx_5=1;
								break;				


			}						
		}
		TMR2IF=0;
		
		
	}


}



void main(void)
{

	device_init();

	TMR2ON = 1;		

	Buzzer=1;
	t_buzzer=10;

	flag_cabecera=1;
	Delayms(1000);
	flag_cabecera=0;
	flag_pd=0;

	eeprom_init();
	
	flag_blank=0;		

	
	contador_h=EEPROM_READ(0x05);
	contador_l=EEPROM_READ(0x06);
	
	contador=contador_h*256+contador_l;
	Display();

	count_bits=32;

	flag_leerbajo=1;
	t_apagado=TAPAGADO;

	while(1)
	{
		if(!flag_freeze)
			leer_remoto();

		if(flag_remotoOK)
		{
			flag_remotoOK=0;
			
			procesar_remoto();			//Chequea si es master y sino que puesto y tecla se presiono

			
			if(!flag_descartar)
			{
				Buzzer=1;
				t_buzzer=2;
				
				flag_apagado=0; 
  			t_apagado=TAPAGADO;
				if(!flag_nobeep)
				{
					flag_beeps=1;								//Habilita los beeps
					c_beeps=4;									// Van a ser 4 beeps
					flag_precargabuzzer=1;
					c_veces=2;									// en 2 tandas
				
				}

				if(flag_guardar)
				{
					flag_guardar=0;
					contador_l=contador&0xFF;
					contador_h=contador>>8;
					
					EEPROM_WRITE(0x05,contador_h);
					EEPROM_WRITE(0x06,contador_l);	
			
				}												
				
			}

			if(flag_master)
			{
				flag_master=0;
				INTE=1;
			}	


			reset_registros();

		}	

		if(flag_timers)
			Timers();	




	}

}


