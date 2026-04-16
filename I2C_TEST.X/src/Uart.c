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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "definitions.h"  
#include "ProcessControl.h"
#include <stdlib.h>
#include <stdio.h>
#include "I2C_relate.h"
#include "TI69601.h"
#include "global.h"
#include "ProcessControl.h"
#include "ctype.h"
#include "ALS_I2C.h"
/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */
extern uint16_t current_index;
extern uint8_t flash_data[360];
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
typedef enum {
    SRC_WIRED,
    SRC_WIRELESS
} CommandSource;

void Handle_DLDR(CommandSource src);
void Handle_SLEP(CommandSource src);
void Handle_WKUP(CommandSource src);
void Handle_CKAV(CommandSource src);
void Handle_STSN(CommandSource src);
void Handle_GTSN(CommandSource src);
void Handle_GTAS(CommandSource src);
void Handle_STAS(CommandSource src);
void Handle_CTPT(CommandSource src);
void Handle_ERDC(CommandSource src);
void Handle_GDTC(CommandSource src);
void Handle_CKPC(CommandSource src);
void Handle_DLCR(CommandSource src);
void Handle_DLER(CommandSource src);
uint16_t BuildDLDRFrame(const char* prefix, const uint8_t* data, uint16_t data_len, uint8_t* out_frame, uint16_t max_len);
int BuildUARTHexString(const char prefix[4],const char* data,char* out_str,uint16_t max_len);
char* ExtractBetweenFirstTwoCommas(const char* input);
uint16_t ReadCurrentDataFromFlash(uint32_t flash_addr, uint8_t *output_buf);
int BuildDLCRFrame(const uint8_t* data1, uint16_t len1, const uint8_t* data2, uint16_t len2, 
                   char* uart_frame, uint16_t max_len);
extern uint8_t usart_accum_buffer[256];
extern volatile bool CTPT_Flag;
float ilm_voltage ;
float imon_voltage;
float IDD;
float IBL;
char uart_frame[2500];
uint8_t idd_packet[512];
uint8_t ibl_packet[512];
uint8_t raw_data[720];
typedef enum {
    AGING,
    BIST,
    STANDBY
} SystemMode;
extern volatile SystemMode current_mode;
/* ************************************************************************** */


typedef void (*CommandHandler)(CommandSource src);

typedef struct {
    const char* cmdStr;
    CommandHandler handler;
} CommandMap;

const CommandMap commandTable[] = {
    {"DLDR", Handle_DLDR},
    {"SLEP", Handle_SLEP},
    {"WKUP", Handle_WKUP},
    {"CKAV", Handle_CKAV},
    {"STSN", Handle_STSN},
    {"GTSN", Handle_GTSN},
    {"STAS", Handle_STAS},
    {"GTAS", Handle_GTAS},
    {"CTPT", Handle_CTPT},
    {"GDTC", Handle_GDTC},
    {"ERDC", Handle_ERDC},
    {"CKPC", Handle_CKPC},
    {"DLCR", Handle_DLCR},
    {"DLER", Handle_DLER}
};

#define COMMAND_COUNT (sizeof(commandTable)/sizeof(commandTable[0]))
void Parse_USART_Command(const uint8_t* USART_ReceiveData, CommandSource src)
{
    if (USART_ReceiveData == NULL)
        return;

    char cmd[5] = {0};
    strncpy(cmd, (const char*)USART_ReceiveData, 4);

    for (int i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(cmd, commandTable[i].cmdStr) == 0) {
            commandTable[i].handler(src); // ??????
            return;
        }
    }
}
void Handle_DLDR(CommandSource src)  //DTC
{
    //Flash_Read_And_Pad(0x000000,current_index+1);
    Flash_Read_MultiBytes(FLASH_ADDR_DATA_START, flash_data, 360);
    //uint16_t valid_length = Remove_FF_From_FlashData();
    
    uint8_t uart_frame[512];  // buffer
    const char prefix[4] = {'D','L','D','R'};
    uint16_t total_len = BuildDLDRFrame(prefix, flash_data, 360, uart_frame, sizeof(uart_frame));    
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_frame, total_len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_frame, total_len );
    }  
}
void Handle_SLEP(CommandSource src)     //??
{
    STBYB_Clear();
    char uart_str[256];
    const char* data = "";
    int len = BuildUARTHexString("SLEP", data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }
}

void Handle_WKUP(CommandSource src)    //??
{
    STBYB_Set();
    char uart_str[256];
    const char* data = "";
    int len = BuildUARTHexString("WKUP", data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }
}

void Handle_CKAV(CommandSource src)     //??????
{
    char uart_str[256];
    //const char* data = "BMW_C169_Y1.0.0";
    const char* data = "BMW_C134_Y1.1.0";
    int len = BuildUARTHexString("CKAV", data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }
}
char result[32] = {0};
void Handle_STSN(CommandSource src)      //??SN
{

    char* result = ExtractBetweenFirstTwoCommas((const char*)usart_accum_buffer);

    if (result != NULL) {
        char uart_str[256];
        const char* data = "";
        int len = BuildUARTHexString("STSN", data, uart_str, sizeof (uart_str));
         if (src == SRC_WIRELESS) {
            SERCOM1_USART_Write( uart_str, len );
        } else if (src == SRC_WIRED) {
            SERCOM2_USART_Write( uart_str, len );
        }
        
        Flash_Write_SN(result);
        free(result);
    }
}
void Handle_GTSN(CommandSource src)     //??SN
{
    char sn_read[256] = {0};
    Flash_Read_SN(sn_read, 256);  // ??sn_read = "12345678;12345678,,,,,,,,"

    // ??? data ????????? sn_read?
    const char* data_prefix = "";  // ???????????? "ABC," ?

    // ?????data_prefix + sn_read
    char combined_data[512] = {0};
    snprintf(combined_data, sizeof(combined_data), "%s%s", data_prefix, sn_read);
    
    char uart_str[256];
    int len = BuildUARTHexString("GTSN", combined_data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }   
}
void Handle_STAS(CommandSource src)     //????
{
    int value = 0;
    char ch = usart_accum_buffer[5];

    if (ch >= '0' && ch <= '9') {
        value = ch - '0';
    }
    
    switch (value) {
    case 0:
        current_mode = AGING;
        break;
    case 1:
        current_mode = BIST;
        break;
    default:
        break;
    }
    
    char uart_str[256];
    const char* data = "";
    int len = BuildUARTHexString("STAS", data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }
}
void Handle_GTAS(CommandSource src)     //????
{
    char uart_str[256];
    const char* data = "";
    if(current_mode ==AGING)
    {
        data="0";
    }
    else if(current_mode == BIST)
    {
        data="1";
    }
    else if(current_mode==STANDBY)
    {
        data="2";
    }
    int len = BuildUARTHexString("GTAS", data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }
}
void Handle_CTPT(CommandSource src)      //????
{
    char uart_str[256];
    const char* data = "";
    int len = BuildUARTHexString("CTPT", data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }

    int value = 0;
    char ch = usart_accum_buffer[5];

    if (ch >= '0' && ch <= '9') {
        value = ch - '0';
    }
    
    switch (value) {
            case 0:
                current_color = BLACK;
                break;
            case 1:
                current_color = RED;
                break;
            case 2:
                current_color = GREEN;
                break;
            case 3:
                current_color = BLUE;
                break;
            case 4:
                current_color = WHITE;
                break;
            case 5:
                current_color = GRAYBAR;
                break;
            case 6:
                current_color = COLORBAR;
                break;
            default:
                break;
    }
    
}
void Handle_GDTC(CommandSource src)      //????DTC??
{
    uint8_t packed = GetPinStatus();
    
    if (als_id_check())
    {
        packed |= (1 << 5);   // ?? bit5 = 1
    }
    else
    {
        packed &= ~(1 << 5);  // ?? bit5 = 0
    }
    
    char packed_str[5];  // 4????? + ?? \0
    snprintf(packed_str, sizeof(packed_str), "%04X", packed);  // ?? packed = 0x1F ? "001F"
    
    char uart_str[256];
    const char* data = "";
    char full_data[128];
    
    snprintf(full_data, sizeof(full_data), "%s,%s", data, packed_str);
    
    int len = BuildUARTHexString("GDTC", full_data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }
}
void Handle_ERDC(CommandSource src)      //????DTC
{
//    Flash_Read_MultiBytes(0x000000, flash_data, 360);
    Flash_Erase_4K(FLASH_ADDR_DATA_START); //DTC
    Flash_Erase_4K(FLASH_ADDR_COUNT_START); //current index
    Flash_Erase_4K(FLASH_ADDR_IBL_CUR); //IBL
    Flash_Erase_4K(FLASH_ADDR_IDD_CUR); //IDD
    Flash_Erase_4K(FLASH_ADDR_DP_ERROR); //DP_ERROR
    Flash_Erase_4K(FLASH_ADDR_POWER_FAULT); //POWER_FAULT
//    Flash_Read_MultiBytes(0x000000, flash_data, 360);
    current_index = 0;
     
    char uart_str[256];
    const char* data = "";
    int len = BuildUARTHexString("ERDC", data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }
}
uint8_t CalculateChecksum(const uint8_t* data, uint16_t len)
{
    uint16_t sum = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return (uint8_t)(sum & 0xFF);
}

// prefix: ???? "DLDR"
// input_data: ???? 0xFF ?????
// input_len: ??????
// out_frame: ??? UART ?????
// out_size: ?????
// return: ?????
/**
 * ?? DLDR ??
 * ???DLDR,OK00,DATA(????checksum),,,,,,,,<0x0D>
 * @param prefix    ??????? "DLDR"
 * @param data      ???????? 0xFF??????
 * @param data_len  ???????ex: 360?
 * @param out_frame ????? UART ??
 * @param max_len   out_frame ?????
 * @return ????????
 */
uint16_t BuildDLDRFrame(const char* prefix, const uint8_t* data, uint16_t data_len, uint8_t* out_frame, uint16_t max_len)
{
    uint8_t clean_data[360];
    uint16_t valid_len = 0;

    // ?? 0xFF
    for (uint16_t i = 0; i < data_len; i++) {
        if (data[i] != 0xFF) {
            clean_data[valid_len++] = data[i];
        }
    }

    uint8_t checksum = CalculateChecksum(clean_data, valid_len);

    // ?????????+????+????+??+checksum+8???+0x0D?
    uint16_t needed_len = 4 + 1 + 4 + 1 +1 +2 + valid_len + 1 + (8 * 1) + 1;
    if (needed_len > max_len) return 0;

    uint16_t idx = 0;

    // 1. ?? "DLDR"
    for (int i = 0; i < 4; i++) {
        out_frame[idx++] = prefix[i];
    }

    // 2. ???? + OK00 + ??
    const char* ok_str = ",OK00,";
    for (int i = 0; i < 6; i++) {
        out_frame[idx++] = ok_str[i];
    }

    // 3. ?? "01"
    out_frame[idx++] = 0x01;


    // 4. ?????????2 bytes??????
    out_frame[idx++] = (uint8_t)(valid_len & 0xFF);
    out_frame[idx++] = (uint8_t)((valid_len >> 8) & 0xFF);

    // 5. ??????
    for (uint16_t i = 0; i < valid_len; i++) {
        out_frame[idx++] = clean_data[i];
    }

    // 6. ?? checksum
    out_frame[idx++] = checksum;

    // 7. ?? 8 ???
    for (int i = 0; i < 8; i++) {
        out_frame[idx++] = ',';
    }

    // 8. ???? 0x0D
    out_frame[idx++] = 0x0D;

    return idx;
}
int BuildUARTHexString(
    const char prefix[4],
    const char* data,       // ????????????
    char* out_str,
    uint16_t max_len)
{
    if (!prefix || !out_str || max_len < 32) return -1;

    int idx = 0;

    // ???4 bytes?
    memcpy(&out_str[idx], prefix, 4);
    idx += 4;
    out_str[idx++] = ',';

    // OK00 ??
    memcpy(&out_str[idx], "OK00", 4);
    idx += 4;
    out_str[idx++] = ',';

    // ?? data???????
    char temp_data[128];
    strncpy(temp_data, data, sizeof(temp_data) - 1);
    temp_data[sizeof(temp_data) - 1] = '\0';
    
    //?????? '\r' ? '\n'
    for (int i = 0; i < strlen(temp_data); i++) {
    if (temp_data[i] == '\r' || temp_data[i] == '\n') {
        temp_data[i] = '\0';
        }
    }

    char* token = strtok(temp_data, ",");
    int field_count = 2; // ?? prefix ? OK00 ??????

    while (token && field_count < 9) {
        int token_len = strlen(token);
        if (idx + token_len + 1 >= max_len) return -2;

        memcpy(&out_str[idx], token, token_len);
        idx += token_len;
        out_str[idx++] = ',';
        token = strtok(NULL, ",");
        field_count++;
    }

    // ?????????? 9 ????
    while (field_count < 9) {
        if (idx >= max_len) return -3;
        out_str[idx++] = ',';
        field_count++;
    }

    // ???? 0x0D
    if (idx >= max_len) return -4;
    out_str[idx++] = 0x0D;

    return idx;
}
char* ExtractBetweenFirstTwoCommas(const char* input)
{
    if (input == NULL)
        return NULL;

    const char* first_comma = strchr(input, ',');
    if (!first_comma)
        return NULL;

    const char* second_comma = strchr(first_comma + 1, ',');
    if (!second_comma)
        return NULL;

    size_t len = second_comma - (first_comma + 1);
    char* result = (char*)malloc(len + 1); // +1 for null terminator
    if (!result)
        return NULL;

    strncpy(result, first_comma + 1, len);
    result[len] = '\0';

    return result;
}
uint16_t ReadADC(ADC_POSINPUT posInput)
{
    // ???????? (MUXPOS)
    ADC0_ChannelSelect(posInput,ADC_NEGINPUT_GND);

    // ????
    ADC0_ConversionStart();
    

    // ??????
    while (!ADC0_ConversionStatusGet());

    // ??????
    return ADC0_ConversionResultGet();
}
void Handle_CKPC(CommandSource src)
{
    uint16_t ilmValue = ReadADC(ADC_POSINPUT_AIN5);  // PA05 -> ILM_Analog
    ilm_voltage = ilmValue * 3.3 / 4095.0;    
    uint16_t imonValue = ReadADC(ADC_POSINPUT_AIN7); // PA07 -> IMON_Analog
    imon_voltage = imonValue * 3.3 / 4095.0;
    
     // A
    float temp_IDD = ilm_voltage * 1000000.0f / 1650.0f / 182.0f;
    float temp_IBL = imon_voltage * 1000000.0f / 2100.0f / 246.0f;

    // ??????
    uint8_t IDD_byte = (uint8_t)(temp_IDD * 127.5f);
    uint8_t IBL_byte = (uint8_t)(temp_IBL * 41.19f);

    char uart_str[256];
    char data[16];
    snprintf(data, sizeof(data), "%02X,%02X", IBL_byte, IDD_byte);  // e.g. "3F,7E"
    int len = BuildUARTHexString("CKPC", data, uart_str, sizeof(uart_str));
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write( uart_str, len );
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write( uart_str, len );
    }
    
}
void Handle_DLCR(CommandSource src)
{
    memset(idd_packet, 0, sizeof(idd_packet));
    memset(ibl_packet, 0, sizeof(ibl_packet));

    uint16_t idd_len = ReadCurrentDataFromFlash(FLASH_ADDR_IDD_CUR, idd_packet);
    uint16_t ibl_len = ReadCurrentDataFromFlash(FLASH_ADDR_IBL_CUR, ibl_packet);
      
    
    int frame_len = BuildDLCRFrame(ibl_packet, ibl_len, idd_packet, idd_len, uart_frame, sizeof(uart_frame));
    SERCOM2_USART_Write((uint8_t*)uart_frame, frame_len);
    if (src == SRC_WIRELESS) {
        SERCOM1_USART_Write((uint8_t*)uart_frame, frame_len);
    } else if (src == SRC_WIRED) {
        SERCOM2_USART_Write((uint8_t*)uart_frame, frame_len);
    }
}
uint16_t ReadCurrentDataFromFlash(uint32_t flash_addr, uint8_t *output_buf)
{
    uint16_t read_count = current_index;   // ? current_index ??????
    if (read_count > 360) {                // ?? 360
        read_count = 360;
    }
    //Flash_Read_MultiBytes(0x003000, flash_data, 720);
//    uint8_t raw_data[360] = {0};
//    Flash_Read_MultiBytes(flash_addr, raw_data, read_count);
    memset(raw_data, 0, sizeof(raw_data));           // 360 ? * 2 Bytes
    Flash_Read_MultiBytes(flash_addr, raw_data, read_count * 2);


    // ????? output_buf???: [0x01][Length_L][Length_H][Data...][Checksum]
    output_buf[0] = 0x01;
    output_buf[1] = (uint8_t)(read_count & 0xFF);          // Length Low
    output_buf[2] = (uint8_t)((read_count >> 8) & 0xFF);   // Length High

//    memcpy(&output_buf[3], raw_data, read_count);
    memcpy(&output_buf[3], raw_data, read_count*2);

    // ?? checksum
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < read_count*2; i++) {
        checksum += raw_data[i];
    }
    output_buf[3 + ( read_count*2)] = checksum;

    return 4 + (read_count*2);  // 1(header) + 2(length) + count + 1(checksum)
}
// ?? DLCR ????
int BuildDLCRFrame(const uint8_t* data1, uint16_t len1, const uint8_t* data2, uint16_t len2, 
                   char* uart_frame, uint16_t max_len)
{
    // prefix + OK00 + 2?? = 10 bytes: "DLCR,OK00,"
    const char* prefix = "DLCR,OK00,";
    const int prefix_len = 10;
    const int tail_commas = 7;
    int pos = 0;

    if (!uart_frame || max_len < (prefix_len + len1 + 1 + len2 + tail_commas + 1)) {
        // ????
        return -1;
    }

    // ?? prefix
    memcpy(&uart_frame[pos], prefix, prefix_len);
    pos += prefix_len;

    // ?? DATA1 (ibl_packet)
    memcpy(&uart_frame[pos], data1, len1);
    pos += len1;

    // ???
    uart_frame[pos++] = ',';

    // ?? DATA2 (idd_packet)
    memcpy(&uart_frame[pos], data2, len2);
    pos += len2;

    // ?7???
    for (int i = 0; i < tail_commas; i++) {
        uart_frame[pos++] = ',';
    }

    // ? 0x0D ??
    uart_frame[pos++] = 0x0D;

    return pos;  // ??uart_frame??
}
size_t  Build_Response1(void)
{
    uint16_t pos = 0;            // uart_frame ????
    uint8_t param[5] = {0};
    const uint8_t *ptr = &usart_accum_buffer[5];
    int param_index = 0;

    // ===== ?? Param1~5 =====
    while (*ptr && param_index < 5)
    {
        if (*ptr >= '0' && *ptr <= '9')
            param[param_index++] = *ptr - '0';
        while (*ptr && *ptr != ',') ptr++;
        if (*ptr == ',') ptr++;
    }

    // ===== ? header =====
    pos += sprintf(&uart_frame[pos], "DLER,OK00,");

    // ===== Data1: PF_temp (??? Flash) =====
    if (param[0] == 1)
    {
        uint16_t count = 0;
        uint8_t pf_byte;

        uint16_t count_pos = pos;        // ????????
        pos += 2;                        // ?????

        for (uint16_t i = 0; i < PF_TOTAL_BYTES; i++)
        {
            Flash_Read_MultiBytes(FLASH_ADDR_POWER_FAULT + i, &pf_byte, 1);
            if (pf_byte != 0)
            {
                uart_frame[pos++] = (i + 1) >> 8;
                uart_frame[pos++] = (i + 1) & 0xFF;
                uart_frame[pos++] = pf_byte;
                count++;
            }
        }

        uart_frame[count_pos]     = (count >> 8) & 0xFF;
        uart_frame[count_pos + 1] = count & 0xFF;
    }

    // ===== Data2, Data3, Data5 ?? =====
    pos += sprintf(&uart_frame[pos], ",,,");

    // ===== Data4: DP_temp (??? Flash) =====
    if (param[3] == 1)
    {
        uint16_t count = 0;
        uint8_t block[8];

        uint16_t count_pos = pos;        // ????????
        pos += 2;                        // ????

        for (uint16_t i = 0; i < 360; i++)
        {
            Flash_Read_MultiBytes(FLASH_ADDR_DP_ERROR + i*8, block, 8);
            bool nonzero = false;
            for (uint8_t j = 0; j < 8; j++)
            {
                if (block[j] != 0)
                {
                    nonzero = true;
                    break;
                }
            }

            if (nonzero)
            {
                uart_frame[pos++] = (i + 1) >> 8;
                uart_frame[pos++] = (i + 1) & 0xFF;
                memcpy(&uart_frame[pos], block, 8);
                pos += 8;
                count++;
            }
        }

        uart_frame[count_pos]     = (count >> 8) & 0xFF;
        uart_frame[count_pos + 1] = count & 0xFF;

        
    }
    
    pos += sprintf(&uart_frame[pos], ",,,1,");
    

    // ===== ?? =====
    uart_frame[pos++] = '\r';
    uart_frame[pos]   = '\0';   // ????
    
    return pos;
}

#define DATA4_MAX_ENTRIES     180
#define UART_FRAME_MAX_SIZE   2000

size_t Build_Response4(void)
{
    uint16_t pos = 0;
    uint8_t block[8];
    uint16_t valid_count = 0;
    static uint16_t last_index = 0;
    bool second_part = false;
    bool has_more = false;   // ????1???0=????, 1=???

    const uint8_t *cmd = &usart_accum_buffer[0];

    // === ?????????? ===
    // cmd ? uint8_t* ? char*?????????
    if (cmd[17] == '2')  // ? 12 ??????????????
    {
        second_part = true;
    }
    else if (cmd[17] == '1')
    {
        last_index = 0;    // ?1?????????
    }

    // === Header ===
    pos += sprintf((char*)&uart_frame[pos], "DLER,OK00,,,,");

    uint16_t count_pos = pos;
    pos += 2; // ????? count

    uint16_t start_index = second_part ? last_index : 0;

    for (uint16_t i = start_index; i < 360; i++)  //??
    {
        Flash_Read_MultiBytes(FLASH_ADDR_DP_ERROR + i * 8, block, 8);

        bool nonzero = false;
        for (uint8_t j = 0; j < 8; j++)
        {
            if (block[j] != 0)
            {
                nonzero = true;
                break;
            }
        }

        if (nonzero)
        {
            // ?? index + block data
            uart_frame[pos++] = (i + 1) >> 8;
            uart_frame[pos++] = (i + 1) & 0xFF;
            memcpy(&uart_frame[pos], block, 8);
            pos += 8;
            valid_count++;

            // ???????????????
            if (!second_part && valid_count >= DATA4_MAX_ENTRIES)
            {
                has_more = true;       // ????????
                last_index = i + 1;    // ???????
                break;
            }

            if (pos >= UART_FRAME_MAX_SIZE - 20)
            {
                has_more = true;
                last_index = i + 1;
                break;
            }
        }
    }

    // ??????????????????
    if (second_part || !has_more)
        has_more = false;
    else
        has_more = true;

    uart_frame[count_pos]     = (valid_count >> 8) & 0xFF;
    uart_frame[count_pos + 1] = valid_count & 0xFF;

    // === ???? ===
    if (has_more)
        pos += sprintf((char*)&uart_frame[pos], ",,,0,"); // ???? ? ??1=0
    else
        pos += sprintf((char*)&uart_frame[pos], ",,,1,"); // ???? ? ??1=1

    uart_frame[pos++] = '\r';
    uart_frame[pos]   = '\0';

    // ?????????????
    if (second_part)
        last_index = 0;

    return pos;
}
void Handle_DLER(CommandSource src)  //DTC
{
    uint8_t param[5] = {0};
    const char *ptr = strchr((const char *)usart_accum_buffer, ',');  // ?????????
    if (ptr == NULL) return;

    ptr++; // ???????
    for (int i = 0; i < 5; i++) {
        if (*ptr == '\0') break;
        param[i] = *ptr - '0';
        ptr = strchr(ptr, ','); // ??????
        if (ptr == NULL) break;
        ptr++;
    }

    // ????????
    if (param[0] == 1) {
        size_t len;
        len = Build_Response1();

        if (src == SRC_WIRELESS)
        {
            SERCOM1_USART_Write( uart_frame, len );
        } else if (src == SRC_WIRED) {
            SERCOM2_USART_Write( uart_frame, len );
        }
    }
    else if (param[3] == 1) {
        size_t len;
        len = Build_Response4();

        if (src == SRC_WIRELESS)
        {
            SERCOM1_USART_Write( uart_frame, len );
        } else if (src == SRC_WIRED) {
            SERCOM2_USART_Write( uart_frame, len );
        }
    }
    else {
        // ???????
    }

    
}

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
