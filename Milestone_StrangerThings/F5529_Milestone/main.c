#include <msp430f5529.h>


#define RLED BIT2;
#define GLED BIT3;
#define BLED BIT4;
#define active_LED BIT7;

volatile unsigned int byte = 0;
volatile unsigned int total_bytes = 0;


void initializeUART(void)       // from Lab 1 example code
{
    P4SEL |= BIT4;          // UART TX
    P4SEL |= BIT5;          // UART RX
    UCA1CTL1 |= UCSWRST;    // Resets state machine
    UCA1CTL1 |= UCSSEL_2;   // SMCLK
    UCA1BR0 = 6;            // 9600 Baud Rate
    UCA1BR1 = 0;            // 9600 Baud Rate
    UCA1MCTL |= UCBRS_0;    // Modulation
    UCA1MCTL |= UCBRF_13;   // Modulation
    UCA1MCTL |= UCOS16;     // Modulation
    UCA1CTL1 &= ~UCSWRST;   // Initializes the state machine
    UCA1IE |= UCRXIE;       // Enables USCI_A0 RX Interrupt

}

void Set_LED(void){
    P4DIR |= active_LED; //status of LEDs, will show on LED 2.
    P4OUT &=~ active_LED;
    P1DIR |= RLED;
    P1SEL |= RLED;
    P1DIR |= GLED;
    P1SEL |= GLED;
    P1DIR |= BLED;
    P1SEL |= BLED;
}

void Set_PWM(void){
    TA0CCR0 |= 225;
    TA0CCR1 |= 0;
    TA0CCR2 |= 0;
    TA0CCR3 |= 0;
    TA0CCTL1 |= OUTMOD_7;
    TA0CCTL2 |= OUTMOD_7;
    TA0CCTL3 |= OUTMOD_7;
    TA0CTL |= MC_1 + TACLR + TASSEL_2;
}

void main(void){
 WDTCTL = WDTPW + WDTHOLD; //stop watchdog timer
 initializeUART();
 Set_LED();
 Set_PWM();
 __bis_SR_register(LPM0_bits + GIE);        // Set low power mode
 __no_operation();


}




#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    P4OUT |= active_LED;
    switch(__even_in_range(UCA1IV,USCI_UCTXIFG)){
    case 0: break; //if vector is zero, there is no interrupt
    case 2:
        switch(byte){
        case 0: //while(!(UCA1IFG & UCTXIFG));

                total_bytes = UCA1RXBUF;
                byte++;


      //UCA1TXBUF = (int)UCA1RXBUF - 3;
                break;

        case 1:
            TA0CCR1 = (UCA1RXBUF);

            byte++;
            break;
        case 2:
            TA0CCR2 = (UCA1RXBUF);
            byte++;
            break;
        case 3:
            TA0CCR3 = (UCA1RXBUF);
            UCA1TXBUF = total_bytes - 3;
            byte++;
            break;
        default:

            UCA1TXBUF = UCA1RXBUF;
            byte++;
            if (byte == total_bytes)
                byte = 0;

            break;
                   //while(!(UCA1IFG & UCTXIFG));


        }

        break;
/*
        if (UCA1RXBUF == 0x0D)
        {
            byte = 0;
            P4OUT &=~ active_LED;
        }
        break;
*/



    }

}
