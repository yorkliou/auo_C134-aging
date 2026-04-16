/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "I2C_relate.h"
#include "definitions.h"                // SYS function prototypes
#include "global.h"
#include "TI69601.h"
#include "ProcessControl.h"
#include "Uart.h"
#include <string.h>


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */
/** Descriptive Data Item Name

  @Summary
    Brief one-line summary of the data item.
    
  @Description
    Full description, explaining the purpose and usage of data item.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
  @Remarks
    Any additional remarks
 */
uint8_t ucReadData[10]; 


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */

#define SPI_DELAY_US   100   // ???? 100us?????????? IC ???

static void SPI_Delay(void)
{
    SYSTICK_DelayUs(SPI_DELAY_US);
}

// ?? (CS ??)
static void IC_SPIStart(void)
{
    TCON_MOSI_Clear();
    TCON_CLK_Clear();
    TCON_CS_Clear();
    SPI_Delay();
}

// ?? (CS ??)
static void IC_SPIStop(void)
{
    TCON_MOSI_Clear();
    TCON_CLK_Clear();
    TCON_CS_Set();
    SPI_Delay();
}

// ?? 1 byte ???????? 1 byte
    static void IC_SPISend(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        if (data & 0x80)
            TCON_MOSI_Set();
        else
            TCON_MOSI_Clear();

        SPI_Delay();

        TCON_CLK_Set();
        SPI_Delay();

        TCON_CLK_Clear();
        SPI_Delay();

        data <<= 1;
    }

    // ??????? MOSI
    TCON_MOSI_Clear();
}

// ??? 1 byte (MOSI ?? 0)
unsigned char SPIByteRecv(void)
{
    int i;
    unsigned char bytedata;

    bytedata = 0;

    for(i = 0; i < 8; i++)
    {
        bytedata = bytedata << 1;

        // ?? SDAOUT=IC_SPI_SDO; --> ????? MISO ?
        uint8_t SDAOUT = TCON_MISO_Get();

        SPI_Delay();         // delay(10);
        TCON_CLK_Set();      // IC_SPI_SCL=1;
        SPI_Delay();         // delay(10);
        TCON_CLK_Clear();    // IC_SPI_SCL=0;
        SPI_Delay();         // delay(10);

        if(SDAOUT)
            bytedata = bytedata | 0x01;
    }

    return bytedata;
}


//----------------------------------------------------
// ?????? (?????????)
//----------------------------------------------------


void Generic_Read_TP_byte_mode(unsigned long ucCommand, unsigned char ucDataNumber)
{
    // ????????
    for (uint8_t i = 0; i < 10; i++)
        ucReadData[i] = 0;
    // ===== ????????? =====
    IC_SPIStart();
    IC_SPISend(0x90);
    IC_SPISend((uint8_t)( ucCommand        & 0xFF));
    IC_SPISend((uint8_t)((ucCommand >> 8 ) & 0xFF));
    IC_SPISend((uint8_t)((ucCommand >>16 ) & 0xFF));
    IC_SPISend((uint8_t)((ucCommand >>24 ) & 0xFF));
    IC_SPISend(0x80);                  // Setting Byte ?? (byte mode)
    IC_SPIStop();

    SYSTICK_DelayUs(20);

    // ===== ?????????? =====
    IC_SPIStart();
    IC_SPISend(0x91);
    for (uint8_t i = 0; i < ucDataNumber; i++)
    {
        ucReadData[i] = SPIByteRecv();
        SYSTICK_DelayUs(100);
    }
    IC_SPIStop();
    SYSTICK_DelayUs(10);
}
void Generic_Read_TP_word_mode(uint32_t ucCommand, uint8_t ucDataNumber)
{
    // ????????
    for (uint8_t i = 0; i < 10; i++)
        ucReadData[i] = 0;

    // ===== ????????? =====
    IC_SPIStart();
    IC_SPISend(0x90);
    IC_SPISend((uint8_t)( ucCommand        & 0xFF));
    IC_SPISend((uint8_t)((ucCommand >> 8 ) & 0xFF));
    IC_SPISend((uint8_t)((ucCommand >>16 ) & 0xFF));
    IC_SPISend((uint8_t)((ucCommand >>24 ) & 0xFF));
    IC_SPISend(0x08);                  // Setting Byte (word mode)
    IC_SPIStop();

    SYSTICK_DelayUs(20);

    // ===== ?????????? =====
    IC_SPIStart();
    IC_SPISend(0x91);                  // ????
    for (uint8_t i = 0; i < ucDataNumber; i++)
    {
        ucReadData[i] = SPIByteRecv(); // ???????? SPI ???
        SYSTICK_DelayUs(100);
    }
    IC_SPIStop();
    SYSTICK_DelayUs(10);
}


uint8_t ReadMappedByte(uint32_t address)
{
    uint32_t regVal = 0;
    uint8_t result = 0;

    // ??? 4 bytes (32 bit)
    Generic_Read_TP_word_mode(address, 4);

    // ???? ucReadData ?? 32-bit regVal
    regVal = ((uint32_t)ucReadData[3] << 24) |
             ((uint32_t)ucReadData[2] << 16) |
             ((uint32_t)ucReadData[1] << 8)  |
             ((uint32_t)ucReadData[0]);

    switch(address)
    {
        case 0x50002188: // Bit[21..16]
            result = (regVal >> 16) & 0x3F; // 6-bit
            break;

        case 0x5000218C: // Bit[2..0]
            result = regVal & 0x07; // 3-bit
            break;

        case 0x5000215C: // Bit[29]
            result = (regVal >> 29) & 0x01;
            break;

        case 0x50002258: // Bit[24,20,16,13,12,11,10,8]
            result = (((regVal >> 24) & 0x01) << 7) |
                     (((regVal >> 20) & 0x01) << 6) |
                     (((regVal >> 16) & 0x01) << 5) |
                     (((regVal >> 13) & 0x01) << 4) |
                     (((regVal >> 12) & 0x01) << 3) |
                     (((regVal >> 11) & 0x01) << 2) |
                     (((regVal >> 10) & 0x01) << 1) |
                     (((regVal >> 8)  & 0x01) << 0);
            break;

        case 0x50002264: // Bit[31..25]
            result = (regVal >> 25) & 0x7F; // 7-bit
            break;

        case 0x50002268: // Bit[4..0]
            result = regVal & 0x1F; // 5-bit
            break;

        case 0x5000225C: // Bit[31..30],[0]
            result = (((regVal >> 30) & 0x03) << 1) | (regVal & 0x01);
            break;

        case 0x50000060: // Bit[15,13,7,6,5,4,2,0]
            result = (((regVal >> 15) & 0x01) << 7) |
                     (((regVal >> 13) & 0x01) << 6) |
                     (((regVal >> 7)  & 0x01) << 5) |
                     (((regVal >> 6)  & 0x01) << 4) |
                     (((regVal >> 5)  & 0x01) << 3) |
                     (((regVal >> 4)  & 0x01) << 2) |
                     (((regVal >> 2)  & 0x01) << 1) |
                     (((regVal >> 0)  & 0x01) << 0);
            break;

        default:
            result = 0; // ??????? 0
            break;
    }

    return result;
}
void ReadAllMappedRegisters(uint8_t result[8])
{
    memset(result, 0, 8);
    
    result[0] = ReadMappedByte(0x50002188); // Bit[21..16]
    result[1] = ReadMappedByte(0x5000218C); // Bit[2..0]
    result[2] = ReadMappedByte(0x5000215C); // Bit[29]
    result[3] = ReadMappedByte(0x50002258); // Bit[24,20,16,13,12,11,10,8]
    result[4] = ReadMappedByte(0x50002264); // Bit[31..25]
    result[5] = ReadMappedByte(0x50002268); // Bit[4..0]
    result[6] = ReadMappedByte(0x5000225C); // Bit[31,30,0]
    result[7] = ReadMappedByte(0x50000060); // Bit[15,13,7,6,5,4,2,0]
}
/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

// *****************************************************************************

/** 
  @Function
    int ExampleInterfaceFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Remarks
    Refer to the example_file.h interface header for function usage details.
 */

/* *****************************************************************************
 End of File
 */
