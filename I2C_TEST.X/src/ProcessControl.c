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
#include <string.h>
#include "I2C_relate.h"
#include "definitions.h"                // SYS function prototypes
#include "global.h"
#include "TI69601.h"
#include "Uart.h"
#include "Tcon_SPI.h"
/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */

extern volatile uint32_t PatternCounter;
extern volatile uint32_t g_ms_ticks;
#define MAX_WRITES        360

#define SN_MAX_LEN     256  
extern volatile uint32_t current_index;
uint8_t flash_data[720]; // ????
uint8_t clean_data[720];
extern uint8_t ucReadData[10]; 

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
void Flash_Erase_4K(uint32_t address);
void Flash_Read_And_Pad(uint32_t address, uint16_t length);
void Flash_Write_Byte(uint32_t addr, uint8_t data);
void Flash_Read_MultiBytes(uint32_t addr, uint8_t* buffer, uint16_t length);
void Save_CurrentIndex_ToFlash(void);
uint16_t Remove_FF_From_FlashData(void);
void Save_IBL_IDD_ToFlash(uint16_t index);
void Flash_Write_U16(uint32_t address, uint16_t data);
void Write_Power_Fault_To_Flash(void);
void Write_DP_Error_To_Flash(void);

typedef enum {
    NONE = -1,
    WHITE,
    RED,
    GREEN,
    BLUE,
    BLACK,
    GRAYBAR,
    COLORBAR
} ColorState;
volatile ColorState current_color = BLUE;
volatile ColorState last_color = NONE;
#define COLORSTATE_MAX COLORBAR

typedef enum {
    AGING,
    BIST,
    STANDBY
} SystemMode;
extern volatile SystemMode current_mode;
uint8_t GetPinStatus(void);
uint32_t GetTimeMs(void)
{
    return g_ms_ticks;   // ???????
}
void Initialization_code(void)
{
    TCON_CS_Set();
    
    BL_VDD1_Set();
    BL_VDD2_Set();
    TCON_VDD_Set();


    //----------------------------------------
    TCON_GRB_Clear();
    RESX_Clear();
    STBYB_Clear();
    TP_GRB_Clear();
    SYSTICK_DelayMs(1);

    TCON_GRB_Set();
    SYSTICK_DelayMs(1);

    RESX_Set();
    TP_GRB_Set();
    SYSTICK_DelayMs(1);

    //RESET=GND, Normal Power off (All Gate on)
    //********************************************		
    All_Gate_ON();
    //********************************************   
    STBYB_Set();

    
    SYSTICK_DelayMs(250);
    I2c_Gen_write_1A_4P_DP(0xC8,0x58,0xA8,0x18,0x23);
 	I2c_Gen_write_1A_1P_DP(0x00,0x20);//Page32
 	I2c_Gen_write_1A_1P_DP(0xFD,0xCC);//Password
 	I2c_Gen_write_1A_1P_DP(0x44,0x02);//Bit1=1	
}
void Panel_InitializeBrightness(void) {
    uint32_t i = 0;

    for (i = 1; i < 11; i++) {
        BRT_Flag = 0;
        TI69601_REG_WRITE_FC3(0x03, (163 * i) / 10); //0x77 0.5mA
        TI69601_BRT_WRITE_ALL_WHITE();
        SYSTICK_DelayMs(500);
    }


}

void pattern_switch(bool is_last_switch) {

    // Pattern 1: ?
    show_black_TCON();
    TI69601_REG_WRITE_FC3(0x03, 0xA3); //0xA3 4.8mA   
    PatternCounter = 0;
    while (PatternCounter < 190) {
        //TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_250306P5, 1);
        if (current_mode == BIST) {
            return;
        }
    }


    // Pattern 2: ?
    show_white_TCON();
    PatternCounter = 0;
    while (PatternCounter < 190) {
        if (is_last_switch && PatternCounter == 180) {
            Save_IBL_IDD_ToFlash(current_index);
            Flash_Read_MultiBytes(0x003000, flash_data, 720);
        }
        if (current_mode == BIST) {
            return;
        }
    }



    // Pattern 3: ?128
    show_gray128_TCON();
    PatternCounter = 0;
    while (PatternCounter < 190) {
        //TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_250306P5, 1);
        //current_mode=BIST;
        if (current_mode == BIST) {
            return;
        }
    }


    // Pattern 4: ?????
    show_h_graybar256_TCON();
    PatternCounter = 0;
    while (PatternCounter < 195) {
        //TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_250306P5, 1);
        if (current_mode == BIST) {
            return;
        }
    }

    // Pattern 5: ?????
    show_colorbar_vgray256_TCON();
    PatternCounter = 0;
    while (PatternCounter < 190) {
        //TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_250306P5, 1);
        if (current_mode == BIST) {
            return;
        }
    }



}

void First_Cycle(void) {
//    uint32_t start = GetTimeMs();
    STBYB_Set();    
    BRT_Flag = 0;
    SET_TCON_BIST();
    pattern_switch(false);
    if (current_mode == BIST) {
        return;
    }    
    pattern_switch(true);
    if (current_mode == BIST) {
        return;
    }
    TI69601_REG_WRITE_FC3(0x03, 0x00); //0xA3 4.8mA
    show_black_TCON();
    LEAVE_TCON_BIST();
    //----------------------------------------????
//    uint32_t p = GetTimeMs();
//    uint32_t TIME1 = p - start;
//    (void) TIME1;
//    LED2_Toggle();

    //    static bool a,b,c,d,e ;
    //    
    //    a=POWER_FAULT_Get();
    //    b=LDO_PG_Get();
    //    c=BUCK1V1_PG_Get();
    //    d=DD_ERROPB_Get();
    //    e=FAULT_Get();
    //    
    uint8_t packed = GetPinStatus();
    
    ///----------------------------------------DTC??
    //    if(current_index==0)
    //    {
    //        Flash_Erase_4K(PAGE_BASE_ADDR); 
    //    }     
    Flash_Write_Byte(FLASH_ADDR_DATA_START + current_index, packed); //DTC 
    Write_DP_Error_To_Flash();
    Write_Power_Fault_To_Flash();
    current_index++;
    Save_CurrentIndex_ToFlash(); //????
    //Flash_Read_MultiBytes(0x000000, flash_data, 360);
    STBYB_Clear();
    
    PatternCounter = 0;
    while (PatternCounter < 900) {
        //TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_250306P5, 1);
        if (current_mode == BIST) {
            return;
        }
    }
//    p = GetTimeMs();
//    uint32_t TIME2 = p - start;
//    (void) TIME2;
//    LED2_Toggle();
}

void Second_Cycle(void) {
    BRT_Flag = 0;
    pattern_switch(false);
    if (current_mode == BIST) {
        return;
    }
    All_Gate_ON();
    pattern_switch(false);
    if (current_mode == BIST) {
        return;
    }
    All_Gate_ON();
    pattern_switch(true);
    if (current_mode == BIST) {
        return;
    }

    uint8_t packed = GetPinStatus();

    Flash_Write_Byte(FLASH_ADDR_DATA_START + current_index, packed); //DTC
    Write_DP_Error_To_Flash();
    Write_Power_Fault_To_Flash();
    current_index++;
    Save_CurrentIndex_ToFlash(); //????

    //    LED4_Toggle();

}

void bist_mode_loop(void) {

    TI69601_REG_WRITE_FC0TOFC9(0x04,(LED_IC_NO-1),0x00,0x75,0x00,0x00,0x00,0x8F,0x00,0x3E,0x0F,0x81,0x00,0x23,0x42,0x47,0x03,0xFE,0x02,0x20);	//boost	
    while (current_mode == BIST) {
        BRT_Flag = 0;
        //TI69601_BRT_WRITE_WORD(TI69601_CURRENT_U_C134,1);	 			 		   	
		//TI69601_REG_WRITE_FC0TOFC9(0x04,(LED_IC_NO-1),0x00,0x77,0x00,0x00,0x03,0x24,0x00,0x3E,0x0F,0x81,0x00,0x23,0x42,0x47,0x03,0xFE,0x02,0x20); //4.2mA
        if (current_color != last_color) {
            switch (current_color) {
                case WHITE:
                    show_white_TCON();
                    break;
                case RED:
                    show_red_TCON();
                    break;
                case GREEN:
                    show_green_TCON();
                    break;
                case BLUE:
                    show_blue_TCON();
                    break;
                case BLACK:
                    show_black_TCON();
                    break;
                case GRAYBAR:
                    show_h_graybar16_TCON();
                    break;
                case COLORBAR:
                    show_colorbar_TCON();

                    break;
                default:
                    break;
            }
            last_color = current_color; // ????????
        }
        SYSTICK_DelayMs(50);
    }
}

uint8_t GetPinStatus(void) {
    bool a = POWER_FAULT_Get();
    bool b = LDO_PG_Get();
    bool c = BUCK1V1_PG_Get();
    bool d = DD_ERROPB_Get();
    bool e = FAULT_Get();

    uint8_t packed = (e << 4) | (d << 3) | (c << 2) | (b << 1) | a;
    return packed;
}

uint8_t SPI_Transfer_Byte(uint8_t data) {
    uint8_t received = 0;

    for (int i = 7; i >= 0; i--) {
        // ?? MOSI ?????MSB first?
        if (data & (1 << i))
            MOSI_Set();
        else
            MOSI_Clear();

        // CLK ?? ? ? Flash ????
        CLK_Set();

        // ?? MISO ????
        received <<= 1;
        if (MISO_Get())
            received |= 1;

        // CLK ?? ? ???? bit
        CLK_Clear();
    }

    return received;
}

void Flash_Read_JEDEC_ID(uint8_t* id_buf) {
    FLASH_CS_Clear();

    SPI_Transfer_Byte(0x9F); // JEDEC ID ??

    id_buf[0] = SPI_Transfer_Byte(0x00); // Manufacturer ID
    id_buf[1] = SPI_Transfer_Byte(0x00); // Memory Type
    id_buf[2] = SPI_Transfer_Byte(0x00); // Capacity

    FLASH_CS_Set();
}

void Flash_Write_Enable(void) {
    FLASH_CS_Clear();
    SPI_Transfer_Byte(0x06); // Write Enable ??
    FLASH_CS_Set();
}

void Flash_Wait_Ready(void) {
    uint8_t status;

    do {
        FLASH_CS_Clear();
        SPI_Transfer_Byte(0x05); // Read Status Register
        status = SPI_Transfer_Byte(0x00); // Dummy byte
        FLASH_CS_Set();
    } while (status & 0x01); // Busy Bit = 1 ? ????
}

void Flash_Write_Page(uint32_t addr, const uint8_t* data, uint16_t len) {
    if (len == 0 || len > 256)
        return;

    Flash_Write_Enable();

    FLASH_CS_Clear();
    SPI_Transfer_Byte(0x02); // Page Program ??
    SPI_Transfer_Byte((addr >> 16) & 0xFF);
    SPI_Transfer_Byte((addr >> 8) & 0xFF);
    SPI_Transfer_Byte(addr & 0xFF);

    for (uint16_t i = 0; i < len; i++) {
        SPI_Transfer_Byte(data[i]);
    }

    FLASH_CS_Set();
    Flash_Wait_Ready();
}

void Flash_Write_Byte(uint32_t addr, uint8_t data) {
    Flash_Write_Enable();

    FLASH_CS_Clear();
    SPI_Transfer_Byte(0x02); // Page Program
    SPI_Transfer_Byte((addr >> 16) & 0xFF);
    SPI_Transfer_Byte((addr >> 8) & 0xFF);
    SPI_Transfer_Byte(addr & 0xFF);
    SPI_Transfer_Byte(data);
    FLASH_CS_Set();

    Flash_Wait_Ready();
}

void Flash_Read_MultiBytes(uint32_t addr, uint8_t* buffer, uint16_t length) {
    if (buffer == NULL || length == 0)
        return;

    FLASH_CS_Clear();

    SPI_Transfer_Byte(0x03); // Read Data ??
    SPI_Transfer_Byte((addr >> 16) & 0xFF);
    SPI_Transfer_Byte((addr >> 8) & 0xFF);
    SPI_Transfer_Byte(addr & 0xFF);

    for (uint16_t i = 0; i < length; i++) {
        buffer[i] = SPI_Transfer_Byte(0x00); // Dummy byte for clocking data out
    }

    FLASH_CS_Set();
}

void Flash_Erase_4K(uint32_t address) {
    address &= 0xFFFFF000; // ??? 4KB ????

    Flash_Write_Enable();
    FLASH_CS_Clear();

    SPI_Transfer_Byte(0x20); // Sector Erase ??
    SPI_Transfer_Byte((address >> 16) & 0xFF);
    SPI_Transfer_Byte((address >> 8) & 0xFF);
    SPI_Transfer_Byte(address & 0xFF);

    FLASH_CS_Set();
    Flash_Wait_Ready();
}

void Flash_Read_And_Pad(uint32_t address, uint16_t length) //? Flash ?????????? 0x00
{
    uint8_t cmd[4];

    if (length > MAX_WRITES)
        length = MAX_WRITES; // ????

    cmd[0] = 0x03; // Read ??
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;

    FLASH_CS_Clear();

    for (int i = 0; i < 4; i++)
        SPI_Transfer_Byte(cmd[i]);

    // ????????
    for (uint16_t i = 0; i < length; i++)
        flash_data[i] = SPI_Transfer_Byte(0x00);

    // ???? 0x00
    for (uint16_t i = length; i < MAX_WRITES; i++)
        flash_data[i] = 0x00;

    FLASH_CS_Set();
}

void Flash_Write_SN(const char* sn_str) {
    if (sn_str == NULL)
        return;

    uint16_t sn_len = strlen(sn_str);
    if (sn_len == 0 || sn_len > SN_MAX_LEN)
        return;

    // ?? 4KB Flash ????? SN ?
    Flash_Erase_4K(FLASH_ADDR_SN_START);

    // ?? SN ??????? 256 bytes??????? Page Program ???
    Flash_Write_Page(FLASH_ADDR_SN_START, (const uint8_t*) sn_str, sn_len);

    // ???? '\0' ?????????
    // Flash_Write_Page(SN_ADDR + sn_len, (const uint8_t*)"\0", 1);
}

void Flash_Read_SN(char* sn_buf, uint16_t buf_size) {
    if (sn_buf == NULL || buf_size == 0)
        return;

    uint8_t temp[SN_MAX_LEN] = {0};

    // ???? SN_MAX_LEN ? bytes
    Flash_Read_MultiBytes(FLASH_ADDR_SN_START, temp, SN_MAX_LEN);

    // ???????? sn_buf????? 0x00, 0xFF ??? buf_size - 1
    uint16_t i;
    for (i = 0; i < buf_size - 1; i++) {
        if (temp[i] == 0x00 || temp[i] == 0xFF)
            break;
        sn_buf[i] = (char) temp[i];
    }
    sn_buf[i] = '\0'; // ????
}

void Load_CurrentIndex(void) {
    uint8_t buffer[4] = {0}; // ?? current_index ? uint32_t?? 4 bytes
    Flash_Read_MultiBytes(FLASH_ADDR_COUNT_START, buffer, 4);

    // ????????? 0xFF ????????
    if (buffer[0] == 0xFF && buffer[1] == 0xFF && buffer[2] == 0xFF && buffer[3] == 0xFF) {
        current_index = 0;
    } else {
        current_index = (uint32_t) buffer[0] << 24 |
                (uint32_t) buffer[1] << 16 |
                (uint32_t) buffer[2] << 8 |
                (uint32_t) buffer[3];
    }

}

void Save_CurrentIndex_ToFlash(void) {
    // ? current_index ?? 4 ? byte
    uint8_t buffer[4];
    buffer[0] = (current_index >> 24) & 0xFF;
    buffer[1] = (current_index >> 16) & 0xFF;
    buffer[2] = (current_index >> 8) & 0xFF;
    buffer[3] = current_index & 0xFF;

    // Flash ??????? 4KB??????
    Flash_Erase_4K(FLASH_ADDR_COUNT_START);

    // ???? 4 bytes
    for (uint8_t i = 0; i < 4; i++) {
        Flash_Write_Byte(FLASH_ADDR_COUNT_START + i, buffer[i]);
    }
}

uint16_t Remove_FF_From_FlashData(void) {
    uint16_t valid_len = 0;

    for (uint16_t i = 0; i < 360; i++) {
        if (flash_data[i] != 0xFF) {
            clean_data[valid_len++] = flash_data[i];
        }
    }

    return valid_len;
}
void Save_IBL_IDD_ToFlash(uint16_t index)
{
    uint16_t ilmValue = ReadADC(ADC_POSINPUT_AIN5);  // PA05 -> ILM_Analog
    float ilm_voltage = ilmValue * 3.3f / 4096.0f;

    uint16_t imonValue = ReadADC(ADC_POSINPUT_AIN7); // PA07 -> IMON_Analog
    float imon_voltage = imonValue * 3.3f / 4096.0f;

    // ???? (uA)
    float temp_IDD = ilm_voltage * 1000000.0f / 1650.0f / 182.0f;
    float temp_IBL = imon_voltage * 1000000.0f / 2100.0f / 246.0f;

    // ????? byte
//    uint8_t IDD_byte = (uint8_t)(temp_IDD * 127.5f);
//    uint8_t IBL_byte = (uint8_t)(temp_IBL * 41.19f);
    uint16_t IDD_byte = (uint16_t)(temp_IDD * 32767.5f);
    uint16_t IBL_byte = (uint16_t)(temp_IBL * 10587.24f);

    // ?? FLASH
//    Flash_Write_Byte(FLASH_ADDR_IDD_CUR + index, IDD_byte);
//    Flash_Write_Byte(FLASH_ADDR_IBL_CUR + index, IBL_byte);
    Flash_Write_U16(FLASH_ADDR_IDD_CUR + (index * 2), IDD_byte);
    Flash_Write_U16(FLASH_ADDR_IBL_CUR + (index * 2), IBL_byte);
}
// ?? 16-bit (2 Bytes)
void Flash_Write_U16(uint32_t address, uint16_t data)
{
    uint8_t low  = (uint8_t)(data & 0xFF);        // ????
    uint8_t high = (uint8_t)((data >> 8) & 0xFF); // ????

    Flash_Write_Byte(address,     high);
    Flash_Write_Byte(address + 1, low);
}
// ????? 8 bytes ?? Flash
void Flash_Write_Current_Record(uint16_t current_index, uint8_t* packed) {
    uint32_t addr = FLASH_ADDR_DP_ERROR  + current_index * 8;
    for (uint8_t i = 0; i < 8; i++) {
        Flash_Write_Byte(addr + i, packed[i]);
    }
}
void Write_DP_Error_To_Flash(void) {

//     ???????? 8 bytes
    ReadAllMappedRegisters(DP_Error);
    
    Flash_Write_Current_Record(current_index, DP_Error);

    
//    uint8_t DP_temp[DP_TOTAL_BYTES];
//    Flash_Read_MultiBytes(FLASH_ADDR_DP_ERROR, DP_temp, 8*360);
//    LED3_Toggle();
   
}

void Generic_Read_RTQ6749(uint8_t ucCommand, uint8_t ucDataNumber)
{
    uint8_t txBuffer[3];
    uint8_t slaveAddrWrite = 0x6B; // 0xD6 >> 1
    uint8_t slaveAddrRead  = 0x6B; // 0xD7 >> 1???????????

    // ? ??? ucReadData[]
    for (uint8_t i = 0; i < ucDataNumber; i++) {
        ucReadData[i] = 0;
    }

    // === ????????? D6 ?? FF,00 ===
    txBuffer[0] = 0xFF;
    txBuffer[1] = 0x00;
    while (!SERCOM5_I2C_Write(slaveAddrWrite, txBuffer, 2)) {
        // ?? LED debug ? timeout
    }
    while (SERCOM5_I2C_IsBusy()) {}

    // === ????? command ===
    txBuffer[0] = ucCommand & 0xFF;
    while (!SERCOM5_I2C_Write(slaveAddrWrite, txBuffer, 1)) {
        // ?? bus ??
    }
    while (SERCOM5_I2C_IsBusy()) {}

    // === ????????? ucDataNumber bytes ===
    while (!SERCOM5_I2C_Read(slaveAddrRead, ucReadData, ucDataNumber)) {
        // ????
    }
    while (SERCOM5_I2C_IsBusy()) {}

    SYSTICK_DelayMs(10); // ?????????
}
uint8_t Read_PF_Results(void)
{
    uint8_t result = 0;
    uint8_t bit3_val = 0;
    uint8_t bit0_val = 0;

    // ?? I2C ?? 0x1D ????? 1 byte
    Generic_Read_RTQ6749(0x1D, 1);

    // ?? ucReadData[0] ???? RTQ6749 ????
    bit3_val = (ucReadData[0] >> 3) & 0x01;  // ? bit[3]
    bit0_val = (ucReadData[0] >> 0) & 0x01;  // ? bit[0]

    // ???? byte (bit1=bit3, bit0=bit0)
    result = (bit3_val << 1) | bit0_val;

    return result;
}
 void Write_Power_Fault_To_Flash(void) {

//     ???????? 8 bytes
    Power_Fault[0]=Read_PF_Results();
      
    Flash_Write_Byte(FLASH_ADDR_POWER_FAULT + current_index, Power_Fault[0]);

    
}
 void PowerOn_Seq(void) 
 {
    TCON_GRB_Set();
    RESX_Set();
    TP_GRB_Set();
    STBYB_Set();
    SYSTICK_DelayMs(500);
 }



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
