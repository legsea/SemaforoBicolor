//__CONFIG(0x23A4);
//__CONFIG(0x2BE4);

//PortA

//Salidas a display

// PortB

#define Data_In RB0
#define digito3 RB1
#define pinDecenas RB2
#define NU RB3
#define releColor RB4
#define pinUnidades RB5

// PortC

#define Ding_Dong RC0
#define NUC1 RC1
#define Buzzer RC2
#define Contando RC3
#define Led_Rx RC5
#define Test1 RC6
#define Test2 RC7

// Macros

#define testbit(var, bit) ((var) & (1 << (bit)))
#define setbit(var, bit) ((var) |= (1 << (bit)))
#define clrbit(var, bit) ((var) &= ~(1 << bit))
