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

#ifndef I2C_UTIL_H    /* Guard against multiple inclusion */
#define I2C_UTIL_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */
#include <stdint.h> 

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

    /**
    @Function
    void I2c_Gen_write_1A_1P_TCON(unsigned int reg, unsigned char length, unsigned char parameter1);

     @Summary
    ??????? I2C ??? TCON?

    @Description
    ?? I2C ???? register?parameter?CRC ????
    */
    void I2c_Gen_write_1A_1P_TCON(unsigned int reg, unsigned char length, unsigned char parameter1);
    void SET_TCON_BIST();
    void LEAVE_TCON_BIST();
    void show_white_TCON();
    void I2c_Gen_write_1A_1P_DP(uint8_t reg, uint8_t parameter1);
    void I2c_Gen_write_1A_4P_DP(uint8_t reg,uint8_t parameter1,uint8_t parameter2,uint8_t parameter3,uint8_t parameter4);
    void show_red_TCON();
    void show_green_TCON() ;
    void show_blue_TCON();
    void show_black_TCON();
    void show_gray128_TCON();
    void show_colorbar_TCON();
    void show_h_graybar256_TCON();
    void show_colorbar_vgray256_TCON();
    void show_v_graybar256_TCON(); 
    void show_h_graybar16_TCON();
    void All_Gate_ON(void);
    void Execute_Read_All(void) ;
    void Generic_Read_TCON(uint16_t ucCommand, uint8_t ucDataNumber);

    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

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

#endif /* I2C_UTIL_H */

/* *****************************************************************************
 End of File
 */
