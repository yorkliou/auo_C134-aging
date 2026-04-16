/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef UART_H    /* Guard against multiple inclusion */
#define UART_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */

    /*  A brief description of a section can be given directly below the section
        banner.
     */


    /* ************************************************************************** */
    /** Descriptive Constant Name

      @Summary
        Brief one-line summary of the constant.
    
      @Description
        Full description, explaining the purpose and usage of the constant.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
      @Remarks
        Any additional remarks
     */
#define EXAMPLE_CONSTANT 0


    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************

    /*  A brief description of a section can be given directly below the section
        banner.
     */


    // *****************************************************************************

    /** Descriptive Data Type Name

      @Summary
        Brief one-line summary of the data type.
    
      @Description
        Full description, explaining the purpose and usage of the data type.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

      @Remarks
        Any additional remarks
        <p>
        Describe enumeration elements and structure and union members above each 
        element or member.
     */


    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

    typedef enum {
        SRC_WIRED,
        SRC_WIRELESS
    } CommandSource;

    typedef void (*CommandHandler)(CommandSource src);

    typedef struct {
        const char* cmdStr;
        CommandHandler handler;
    }CommandMap;
    void Parse_USART_Command(const uint8_t* USART_ReceiveData, CommandSource src);
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
    uint16_t ReadADC(ADC_POSINPUT posInput);
    uint8_t CalculateChecksum(const uint8_t* data, uint16_t len);
    uint16_t BuildDLDRFrame(const char* prefix, const uint8_t* data, uint16_t data_len, uint8_t* out_frame, uint16_t max_len);
    int BuildUARTHexString(const char prefix[4],const char* data,char* out_str,uint16_t max_len);   
    char* ExtractBetweenFirstTwoCommas(const char* input);
    uint16_t ReadCurrentDataFromFlash(uint32_t flash_addr, uint8_t *output_buf);
    int BuildDLCRFrame(const uint8_t* data1, uint16_t len1, const uint8_t* data2, uint16_t len2, 
                   char* uart_frame, uint16_t max_len);
    size_t  Build_Response1(void);
    size_t Build_Response4(void);
    extern uint8_t usart_accum_buffer[256];
    extern volatile bool CTPT_Flag;
    /*  A brief description of a section can be given directly below the section
        banner.
     */

    // *****************************************************************************
    /**
      @Function
        int ExampleFunctionName ( int param1, int param2 ) 

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


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
