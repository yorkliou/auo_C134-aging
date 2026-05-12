/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "I2C_relate.h"
#include "definitions.h"                // SYS function prototypes
#include "global.h"
#include "TI69601.h"
#include "ProcessControl.h"
#include "Uart.h"
#include "Tcon_SPI.h"
#include "ALS_I2C.h"



uint32_t current_index = 0;
uint32_t i = 0;
uint32_t a = 0;
#define I2C_SLAVE_ADDR  0x61
volatile bool TC3_flag = false;
#define F_CYCLES 600  
#define S_CYCLES 300 
#define USART_PACKET_MAX_LEN 256

static uint8_t usart_byte = 0;
uint8_t usart_accum_buffer[USART_PACKET_MAX_LEN] = {0};
static uint16_t usart_accum_index = 0;
uint8_t i2c_test_data[2] = {0xAA, 0x55};
volatile bool i2c_transfer_done_flag = true; // ????
volatile bool Counter_Flag = true; // ????
volatile uint32_t PatternCounter=0;
volatile uint32_t g_ms_ticks = 0;
extern uint8_t flash_data[360];
typedef enum {
    AGING,
    BIST,
    STANDBY
} SystemMode;
volatile SystemMode current_mode = AGING;
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

void i2c_transfer_done(uintptr_t context) {
    i2c_transfer_done_flag = true; // ?????????
    //LED1_Toggle();                  // ?????
}

void TC3_TimerExpired(TC_TIMER_STATUS status, uintptr_t context) {//PATTERN SWITCH
    if (status & TC_INTFLAG_OVF_Msk) {
        //LED1_Toggle();
         g_ms_ticks++;   // ?? interrupt = 1ms
    }
}
void TC4_TimerExpired(TC_TIMER_STATUS status, uintptr_t context) {
    if (status & TC_INTFLAG_OVF_Msk) {
        // ???????????? LED
        switch (current_mode)
        {
            case AGING:
                LED1_Toggle();
                LED2_Clear();
                LED3_Clear();
                break;

            case BIST:
                LED2_Toggle();
                LED1_Clear();
                LED3_Clear();
                break;

            case STANDBY:
                LED3_Toggle();
                LED1_Clear();
                LED2_Clear();
                break;

            default:
               
                break;
        }
    }
}
void TC5_TimerExpired(TC_TIMER_STATUS status, uintptr_t context) {//CTPT
    if (status & TC_INTFLAG_OVF_Msk) {
        //LED1_Toggle();
        PatternCounter++;
    }
}
void USART2_Receive(uintptr_t context) 
{
    if (usart_accum_index < USART_PACKET_MAX_LEN) 
    {
        usart_accum_buffer[usart_accum_index++] = usart_byte;

        if (usart_byte == '\r') 
        {
            // ??????????
            if (usart_accum_index < USART_PACKET_MAX_LEN) 
            {
                usart_accum_buffer[usart_accum_index] = '\0';
            }
            else // ??????????
            {
                usart_accum_buffer[USART_PACKET_MAX_LEN - 1] = '\0';
            }
            Parse_USART_Command(usart_accum_buffer, SRC_WIRED);
            usart_accum_index = 0;
        }
    } 
    else // ?????????? \r?????
    {
        usart_accum_index = 0;
    }
    SERCOM2_USART_Read(&usart_byte, 1);
}
void USART2_Transmit( uintptr_t context )
{
    
}
void USART1_Receive(uintptr_t context)
{
    if (usart_accum_index < USART_PACKET_MAX_LEN)
    {
        usart_accum_buffer[usart_accum_index++] = usart_byte;

        if (usart_byte == '\r')  // ?????????
        {
            usart_accum_buffer[usart_accum_index] = '\0';  // ????
            Parse_USART_Command(usart_accum_buffer,SRC_WIRELESS);       // ??????
            usart_accum_index = 0;                          // ?? index
        }
    }
    else
    {
        // ??????
        usart_accum_index = 0;
    }
    // ??????? byte
    SERCOM1_USART_Read(&usart_byte, 1);
}
void USART1_Transmit( uintptr_t context )
{
    
}
int main(void) {
    /* Initialize all modules */
    SYS_Initialize(NULL);
    ADC0_Initialize();
    ADC0_Enable();
    SYSTICK_TimerStart();
    SERCOM5_I2C_CallbackRegister(i2c_transfer_done, (uintptr_t) NULL);
    SERCOM2_USART_ReadCallbackRegister( USART2_Receive, (uintptr_t )NULL );
    SERCOM2_USART_WriteCallbackRegister( USART2_Transmit, (uintptr_t )NULL );
    SERCOM2_USART_Read( &usart_byte, 1);
    SERCOM1_USART_ReadCallbackRegister( USART1_Receive, (uintptr_t )NULL );
    SERCOM1_USART_WriteCallbackRegister( USART1_Transmit, (uintptr_t )NULL );
    SERCOM1_USART_Read( &usart_byte, 1);
    TC3_TimerCallbackRegister(TC3_TimerExpired, (uintptr_t) NULL);
    TC4_TimerCallbackRegister(TC4_TimerExpired, (uintptr_t) NULL);
    TC5_TimerCallbackRegister(TC5_TimerExpired, (uintptr_t) NULL);
    TC5_TimerStart();
    TC4_TimerStart();
    TC3_TimerStart();
    TCON_CS_Set();
    
    //----------------------------------------Power
    BL_VDD1_Set();
    BL_VDD2_Set();
    TCON_VDD_Set();
    SYSTICK_DelayMs(10);

    //----------------------------------------BL

    TI69601_SET_CHIP(0x00, 0x00);
    //TI69601_REG_WRITE_FC0TOFC9(0x04,(LED_IC_NO-1),0x00,0x77,0x00,0x00,0x03,0x24,0x00,0x3E,0x0F,0x81,0x00,0x23,0x42,0x47,0x03,0xFE,0x02,0x20); //4.2mA	
    TI69601_REG_WRITE_FC0TOFC10(0x04, (LED_IC_NO - 1), 0x00, 0x55, 0x00, 0x00, 0x01, 0x00, 0x00, 0x3E, 0x1F, 0x81, 0x00, 0x23, 0x42, 0x47, 0x03, 0xFE, 0x02, 0x60, 0x00, 0x30);//01/22
//    TI69601_REG_WRITE_FC0TOFC10(0x04, (LED_IC_NO - 1), 0x00, 0x75, 0x00, 0x00, 0x00, 0x00, 0x78, 0x3E, 0x1F, 0xD1, 0x00, 0x23, 0x40, 0x07, 0x02, 0x94, 0x02, 0x20, 0x00, 0xD1);
    //------------------------------------------------FC1        ,FC2        ,FC3        ,FC4        ,FC5        ,FC6        ,FC7        ,FC8        ,FC9        ,FC10

    TI69601_BRT_WRITE_ALL_BLACK();
    TI69601_BRT_WRITE_ALL_BLACK();

    TI69601_REG_WRITE_FC9(0x00, 0x00, 0x00, 0x0B, 0x62, 0x60); //Phase shifter mode#4 (00-11)	
    TI69601_REG_WRITE_FC9(0x00, 0x0B, 0x00, 0x17, 0x42, 0x60); //Phase shifter mode#3 (12-23)	
    TI69601_REG_WRITE_FC9(0x00, 0x17, 0x00, 0x23, 0x22, 0x60); //Phase shifter mode#2 (24-35)

    TI69601_REG_WRITE_FC3(0x03, 0xA3); //0xA3 4.8mA

    BRT_Flag = 0;
    TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_C134, 1);
//    if(LR_Check_Get()){            //??
//        TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_250306P5_R, 1);
//    }
//    else{
//        TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_250306P5_L, 1);
//    }    
    
    //TI69601_BRT_WRITE_ALL_WHITE();   
//    Panel_InitializeBrightness();    //10???
    
     //----------------------------------------PowerOnSeq
    PowerOn_Seq();
    //---------------------------------------- Initial para
    Load_CurrentIndex();
    
    Generic_Read_TP_word_mode(0x200056A0,5);
//    als_init();
//    als_id_check();
//    als_data_read();

//    Flash_Erase_4K(0x000000);
//    Flash_Erase_4K(0x004000); 
//    Flash_Erase_4K(0x005000);
//    SYSTICK_DelayMs(1000);
//    Write_DP_Error_To_Flash();
//    Write_Power_Fault_To_Flash();
    //Execute_Read_All();FW Version
    while (true) {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks();
        //    uint8_t id[3];
        //    Flash_Read_JEDEC_ID(id); // ? JEDEC ID        
        switch (current_mode) {
            case AGING:
                if(current_index<241)
                {                
//                    uint32_t start = GetTimeMs();
                    First_Cycle();
//                    uint32_t end = GetTimeMs();
//                    uint32_t TIME = end - start ;
//                    (void)TIME; 
//                    LED2_Toggle();
                    
                }
                else if(current_index>240 && current_index <= 360)
                {
                    if(current_index==241)
                    {
                        //Initialization_code();
                        STBYB_Set(); 
                        TI69601_REG_WRITE_FC3(0x03, 0xA3);
                        SYSTICK_DelayMs(200);
                    }         
                    SET_TCON_BIST();
                    Second_Cycle();
                    LEAVE_TCON_BIST();
                }
                else if(current_index>360)
                {
                    current_mode = STANDBY;
                }                                   
                break;

            case BIST:
                STBYB_Set();
                SYSTICK_DelayMs(200); 
                All_Gate_ON();    
                SET_TCON_BIST();
                show_blue_TCON();
                TI69601_REG_WRITE_FC3(0x03, 0xA3); //0xA3 4.8mA   ;           
                current_color = BLUE;
                bist_mode_loop();
               
                break;

            case STANDBY:
                
                SET_TCON_BIST();  
                pattern_switch(false);
                LEAVE_TCON_BIST();
                break;
        }
        
        SYSTICK_DelayMs(10);
    }
    
    return ( EXIT_FAILURE);
}


/*******************************************************************************
 End of File
 */

