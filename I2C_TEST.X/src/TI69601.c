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

#include "I2C_relate.h"  // ????????? .h ??
#include "definitions.h"  // ??? Harmony ? SERCOM API ??????
#include <stdint.h>
#include <stdbool.h>
#include "TI69601.h"
#include "global.h"

/* TODO:  Include other files here if needed. */


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



/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */

/** 
  @Function
    int ExampleLocalFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Description
    Full description, explaining the purpose and usage of the function.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

  @Precondition
    List and describe any required preconditions. If there are no preconditions,
    enter "None."

  @Parameters
    @param param1 Describe the first parameter to the function.
    
    @param param2 Describe the second parameter to the function.

  @Returns
    List (if feasible) and describe the return values of the function.
    <ul>
      <li>1   Indicates an error occurred
      <li>0   Indicates an error did not occur
    </ul>

  @Remarks
    Describe any special behavior not described above.
    <p>
    Any additional remarks.

  @Example
    @code
    if(ExampleFunctionName(1, 2) == 0)
    {
        return 3;
    }
 */
static void TI69601_Sent(uint8_t data)
{
//    uint32_t i = 0;
    for (int shift = 0; shift < 8; shift++)
    {
        TI69601_CLK_Clear();
        
        if (data & 0x80)
            TI69601_SIN_Set();
        else
            TI69601_SIN_Clear();

        data <<= 1;

        TI69601_CLK_Clear();       
        
        TI69601_CLK_Set();
        
        SYSTICK_DelayUs(1);
        
    }
    TI69601_CLK_Clear();
    
}

static void TI69601_START(void) {
    TI69601_Sent(0xFF);
    TI69601_Sent(0xFF);
}

static void TI69601_END(void) {
    TI69601_Sent(0xFF);
    TI69601_Sent(0xFF);
    for (volatile int m = 0; m < 200; m++)
        TI69601_Sent(0xFF);
}

void TI69601_SET_VSYNC(uint8_t h0, uint8_t l0, uint8_t h1, uint8_t l1)
{
    TI69601_START();
    TI69601_Sent(0x55);
    TI69601_Sent(0xB0);
    TI69601_Sent(h0); TI69601_Sent(l0);
    TI69601_Sent(h1); TI69601_Sent(l1);
    TI69601_END();
}

void TI69601_BRT_WRITE(uint8_t sub_head_high_byte_0, uint8_t sub_head_low_byte_0,
                       uint8_t sub_head_high_byte_1, uint8_t sub_head_low_byte_1,
                       uint16_t led_no)
{
    uint16_t i = led_no;

    TI69601_START();

    TI69601_Sent(0x55);
    TI69601_Sent(0x60);
    TI69601_Sent(sub_head_high_byte_0);
    TI69601_Sent(sub_head_low_byte_0);
    TI69601_Sent(sub_head_high_byte_1);
    TI69601_Sent(sub_head_low_byte_1);

    while (i > 0)
    {
        temp[0] = (LED[led_no - i] >> 8) & 0xFF;
        temp[1] = LED[led_no - i] & 0xFF;

        TI69601_Sent(temp[0]);
        TI69601_Sent(temp[1]);

        i--;
    }

    TI69601_END();
}

void TI69601_BRT_WRITE_WORD(const uint16_t *s, float scale)
{
    uint16_t j;

    if (BRT_Flag == 0)
    {
        for (j = 0; j < LED_MAX_PCS_NO; j++)
        {
            LED[j] = (uint16_t)(s[j] * scale);
        }
        BRT_Flag = 1;
    }

    TI69601_BRT_WRITE(0x00, 0x00, 0x00, (LED_IC_NO - 1), LED_MAX_PCS_NO);
    TI69601_SET_VSYNC(0x00, 0x00, 0x00, 0x00);
    TI69601_BRT_WRITE(0x00, 0x00, 0x00, (LED_IC_NO - 1), LED_MAX_PCS_NO);
    TI69601_SET_VSYNC(0x00, 0x00, 0x00, 0x00);

    // TI69601_END();  // ?????????
}

void TI69601_SET_CHIP(unsigned char sub_head_high_byte_0,unsigned char sub_head_low_byte_0)
{
	TI69601_START();		
	TI69601_Sent(0x55);
	TI69601_Sent(0x00);
	TI69601_Sent(sub_head_high_byte_0);
	TI69601_Sent(sub_head_low_byte_0);
	TI69601_END();
}

void TI69601_REG_WRITE_FC0TOFC10(
    uint8_t h0, uint8_t l0, uint8_t h1, uint8_t l1,
    uint8_t h2, uint8_t l2, uint8_t h3, uint8_t l3,
    uint8_t h4, uint8_t l4, uint8_t h5, uint8_t l5,
    uint8_t h6, uint8_t l6, uint8_t h7, uint8_t l7,
    uint8_t h8, uint8_t l8, uint8_t h9, uint8_t l9,
    uint8_t h10, uint8_t l10)
{
    TI69601_START();

    TI69601_Sent(0x55); // sync
    TI69601_Sent(0x20); // command

    TI69601_Sent(h0);  TI69601_Sent(l0);
    TI69601_Sent(h1);  TI69601_Sent(l1);
    TI69601_Sent(h2);  TI69601_Sent(l2);
    TI69601_Sent(h3);  TI69601_Sent(l3);
    TI69601_Sent(h4);  TI69601_Sent(l4);
    TI69601_Sent(h5);  TI69601_Sent(l5);
    TI69601_Sent(h6);  TI69601_Sent(l6);
    TI69601_Sent(h7);  TI69601_Sent(l7);
    TI69601_Sent(h8);  TI69601_Sent(l8);
    TI69601_Sent(h9);  TI69601_Sent(l9);
    TI69601_Sent(h10); TI69601_Sent(l10);

    TI69601_END();
}

void TI69601_BRT_WRITE_ALL_BLACK(void)
{
    if (BRT_Flag == 0)
    {
        for (uint16_t j = 0; j < LED_MAX_PCS_NO; j++)
        {
            LED[j] = 0x0000; // ???12-bit LED ???
        }
        BRT_Flag = 1;
    }

    // ???? CS ???????????
    // TI69601_CS_Clear();

    TI69601_BRT_WRITE(0x00, 0x00, 0x00, (LED_IC_NO - 1), LED_MAX_PCS_NO);
    TI69601_SET_VSYNC(0x00, 0x00, 0x00, 0x00);
    TI69601_BRT_WRITE(0x00, 0x00, 0x00, (LED_IC_NO - 1), LED_MAX_PCS_NO);
    TI69601_SET_VSYNC(0x00, 0x00, 0x00, 0x00);

    // TI69601_CS_Set();
}
void TI69601_BRT_WRITE_ALL_WHITE(void)
{
    if (BRT_Flag == 0)
    {
        for (uint16_t j = 0; j < LED_MAX_PCS_NO; j++)
        {
            LED[j] = 0x0FFF;  // ??? LED ????
        }
        BRT_Flag = 1;
    }

    // ?? LED ???? + ??
    TI69601_BRT_WRITE(0x00, 0x00, 0x00, (LED_IC_NO - 1), LED_MAX_PCS_NO);
    TI69601_SET_VSYNC(0x00, 0x00, 0x00, 0x00);
    TI69601_BRT_WRITE(0x00, 0x00, 0x00, (LED_IC_NO - 1), LED_MAX_PCS_NO);
    TI69601_SET_VSYNC(0x00, 0x00, 0x00, 0x00);
}

void TI69601_REG_WRITE_FC9(uint8_t chip_idx_start_high_byte,
                            uint8_t chip_idx_start_low_byte,
                            uint8_t chip_idx_end_high_byte,
                            uint8_t chip_idx_end_low_byte,
                            uint8_t sub_head_high_byte,
                            uint8_t sub_head_low_byte)
{
    TI69601_START();

    TI69601_Sent(0x55);                       // ?????
    TI69601_Sent(0x49);                       // ?????? FC9 ???

    TI69601_Sent(chip_idx_start_high_byte);   // ?? chip index ??
    TI69601_Sent(chip_idx_start_low_byte);    // ?? chip index ??
    TI69601_Sent(chip_idx_end_high_byte);     // ?? chip index ??
    TI69601_Sent(chip_idx_end_low_byte);      // ?? chip index ??

    TI69601_Sent(sub_head_high_byte);         // FC9 ???????
    TI69601_Sent(sub_head_low_byte);          // FC9 ???????

    TI69601_END();
}
void TI69601_REG_WRITE_FC3(uint8_t sub_head_high_byte, uint8_t sub_head_low_byte)
{
    TI69601_START();              // ??????? CS ???

    TI69601_Sent(0x55);           // ?????
    TI69601_Sent(0x23);           // FC3 ???
    TI69601_Sent(sub_head_high_byte); // FC3 ????
    TI69601_Sent(sub_head_low_byte);  // FC3 ????

    TI69601_END();                // ??????? CS ???
}
void TI69601_REG_WRITE_FC0TOFC9(uint8_t h0, uint8_t l0, uint8_t h1, uint8_t l1,
                                uint8_t h2, uint8_t l2, uint8_t h3, uint8_t l3,
                                uint8_t h4, uint8_t l4, uint8_t h5, uint8_t l5,
                                uint8_t h6, uint8_t l6, uint8_t h7, uint8_t l7,
                                uint8_t h8, uint8_t l8, uint8_t h9, uint8_t l9)
{
    TI69601_START();        
    TI69601_Sent(0x55);
    TI69601_Sent(0x20);
    
    // ?????????
    uint8_t data[] = {h0, l0, h1, l1, h2, l2, h3, l3, h4, l4, h5, l5, h6, l6, h7, l7, h8, l8, h9, l9};
    
    for(int i = 0; i < 20; i++) {
        TI69601_Sent(data[i]);
    }
    
    TI69601_END();
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
