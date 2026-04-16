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

#ifndef PROCESSCONTROL_H    /* Guard against multiple inclusion */
#define PROCESSCONTROL_H


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
    void Initialization_code(void);
    void pattern_switch(bool is_last_switch);
    uint32_t GetTimeMs(void);
    void Panel_InitializeBrightness(void);
    void First_Cycle(void);    
    void Second_Cycle(void);
    void bist_mode_loop(void);
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
    extern volatile ColorState current_color ;
    void Flash_Read_JEDEC_ID(uint8_t* id_buf);
    uint8_t SPI_Transfer_Byte(uint8_t data);
    void Flash_Write_Enable(void);
    void Flash_Write_Page(uint32_t addr, const uint8_t* data, uint16_t len);
    void Flash_Wait_Ready(void);
    void Flash_Read_MultiBytes(uint32_t addr, uint8_t* buffer, uint16_t length);
    void Flash_Erase_4K(uint32_t address);
    void Flash_Write_Byte(uint32_t addr, uint8_t data);
    void Flash_Read_And_Pad(uint32_t address, uint16_t length);
    void Flash_Write_SN(const char* sn_str);
    void Flash_Read_SN(char* sn_buf, uint16_t buf_size);
    uint8_t GetPinStatus(void);
    void Load_CurrentIndex(void);
    void Save_CurrentIndex_ToFlash(void);
    uint16_t Remove_FF_From_FlashData(void);
    void Save_IBL_IDD_ToFlash(uint16_t index);
    void Flash_Write_U16(uint32_t address, uint16_t data);
    void Flash_Read_MultiBytes(uint32_t addr, uint8_t* buffer, uint16_t length);
    void Flash_Write_Current_Record(uint16_t current_index, uint8_t* packed);
    void Write_DP_Error_To_Flash(void);
    void Generic_Read_RTQ6749(uint8_t ucCommand, uint8_t ucDataNumber);
    uint8_t Read_PF_Results(void);
    void Write_Power_Fault_To_Flash(void);
     void PowerOn_Seq(void) ;
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
