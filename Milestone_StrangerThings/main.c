#include <msp430f5529.h>


#define RLED BIT2;
#define GLED BIT3;
#define BLED BIT4;
#define active_LED BIT7;

unsigned int byte = 0;
unsigned int total_bytes = 0;


void initializeUART(void)       // from Lab 1 example code
{
    P3SEL |= BIT3;          // UART TX
    P3SEL |= BIT4;          // UART RX
    UCA0CTL1 |= UCSWRST;    // Resets state machine
    UCA0CTL1 |= UCSSEL_2;   // SMCLK
    UCA0BR0 = 6;            // 9600 Baud Rate
    UCA0BR1 = 0;            // 9600 Baud Rate
    UCA0MCTL |= UCBRS_0;    // Modulation
    UCA0MCTL |= UCBRF_13;   // Modulation
    UCA0MCTL |= UCOS16;     // Modulation
    UCA0CTL1 &= ~UCSWRST;   // Initializes the state machine
    UCA0IE |= UCRXIE;       // Enables USCI_A0 RX Interrupt
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

 __bis_SR_register(LPM0_bits + GIE);        // Set low power mode

 initializeUART();
 Set_LED();
 Set_PWM();
}




#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    P4OUT |= active_LED;
    switch(__even_in_range(UCA0IV,USCI_UCTXIFG)){
    case 0: break; //if vector is zero, there is no interrupt
    case 1:
        switch(byte){
        case 0:// while(!(UCA0IFG & UCTXIFG));

                total_bytes = UCA0RXBUF;
                byte++;


      //UCA0TXBUF = (int)UCA0RXBUF - 3;
        break;

        case 1:
            TA0CCR1 = (UCA0RXBUF);
            break;
        case 2:
            TA0CCR2 = (UCA0RXBUF);
            break;
        case 3:
            TA0CCR3 = (UCA0RXBUF);
            break;
        default: UCA0TXBUF = UCA0RXBUF;
                        break;
                   // while(!(UCA0IFG & UCTXIFG));


        }

        if(UCA0RXBUF != 0x0D)
            byte++;
        else
        {
            byte = 0;
            P4OUT &=~ active_LED;
        }
        break;




    }

}
