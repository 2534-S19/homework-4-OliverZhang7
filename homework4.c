#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"
typedef enum {idle, S2, S25, S253, S2534} states_fsm;
int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.
    bool finished = false;
    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.
    eUSCI_UART_Config uartConfig = {
             EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // SMCLK Clock Source = 3MHz
             19,                                           // UCBR = 19
             8,                                            // UCBRF = 8
             0x55,                                         // UCBRS = 0xD6
             EUSCI_A_UART_NO_PARITY,                       // No Parity
             EUSCI_A_UART_LSB_FIRST,                       // LSB First
             EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
             EUSCI_A_UART_MODE,                            // UART mode
             EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling
        };

    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    // TODO: Initialize EUSCI_A0
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    // TODO: Enable EUSCI_A0
    UART_enableModule(EUSCI_A0_BASE);

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
        if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
        {
            rChar = UART_receiveData(EUSCI_A0_BASE);
        }
        else
        {
            rChar = 0xFF;
        }

        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.
        if (rChar != 0xFF)
        {
            if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
                 UART_transmitData(EUSCI_A0_BASE, rChar);

            if (charFSM(rChar) == true)
                finished=true;
        }


        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
        if (finished==true)
        {
            int i;
            for(i=0; i<48; i++)//there are total 48 chars in the string
            {
            if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
            {
                  int k;
                  for(k=0; k<48; k++)
                  {
                      UART_transmitData(EUSCI_A0_BASE, response[k]);
                  }
                  //UART_transmitData(EUSCI_A0_BASE, '\n');
                  //UART_transmitData(EUSCI_A0_BASE, '\n');
                  //UART_transmitData(EUSCI_A0_BASE, '\r');
                  //UART_transmitData(EUSCI_A0_BASE, response[3]);
            }

            }

            finished=false;
        }

    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
bool charFSM(char rChar)
{
    static states_fsm currentState = idle;
    bool finished = false;
    switch (currentState)
    {

        case idle:
            if (rChar == 0x32)
                currentState = S2;
            else
                currentState = idle;
            break;

        case S2:
            if (rChar == 0x35)
                currentState = S25;
            else
                currentState = idle;
            break;

        case S25:
            if (rChar == 0x33)
                currentState = S253;
            else
                currentState = idle;
            break;

        case S253:
            if (rChar == 0x34)
            {
                currentState = idle;
                finished = true;
            }
            //else
                //currentState = idle;
            break;

        //case S2534:
            //currentState = idle;
            //break;
    }

    return finished;
}
