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

#ifndef TI69601_H    /* Guard against multiple inclusion */
#define TI69601_H


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
    typedef struct _example_struct_t {
        /* Describe structure member. */
        int some_number;

        /* Describe structure member. */
        bool some_flag;

    } example_struct_t;


    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

    /*  A brief description of a section can be given directly below the section
        banner.
     */
    void TI69601_BRT_WRITE_WORD(const uint16_t *s, float scale);
    void TI69601_BRT_WRITE(uint8_t sub_head_high_byte_0, uint8_t sub_head_low_byte_0,
                       uint8_t sub_head_high_byte_1, uint8_t sub_head_low_byte_1,
                       uint16_t led_no);
    void TI69601_SET_VSYNC(uint8_t h0, uint8_t l0, uint8_t h1, uint8_t l1);
    void TI69601_SET_CHIP(unsigned char sub_head_high_byte_0,unsigned char sub_head_low_byte_0);
    void TI69601_REG_WRITE_FC0TOFC10(
    uint8_t h0, uint8_t l0, uint8_t h1, uint8_t l1,
    uint8_t h2, uint8_t l2, uint8_t h3, uint8_t l3,
    uint8_t h4, uint8_t l4, uint8_t h5, uint8_t l5,
    uint8_t h6, uint8_t l6, uint8_t h7, uint8_t l7,
    uint8_t h8, uint8_t l8, uint8_t h9, uint8_t l9,
    uint8_t h10, uint8_t l10);
    void TI69601_BRT_WRITE_ALL_BLACK(void);
    void TI69601_BRT_WRITE_ALL_WHITE(void);
    void TI69601_REG_WRITE_FC9(uint8_t chip_idx_start_high_byte,
                            uint8_t chip_idx_start_low_byte,
                            uint8_t chip_idx_end_high_byte,
                            uint8_t chip_idx_end_low_byte,
                            uint8_t sub_head_high_byte,
                            uint8_t sub_head_low_byte);
    void TI69601_REG_WRITE_FC3(uint8_t sub_head_high_byte, uint8_t sub_head_low_byte);
    void TI69601_REG_WRITE_FC0TOFC9(uint8_t h0, uint8_t l0, uint8_t h1, uint8_t l1,
                                uint8_t h2, uint8_t l2, uint8_t h3, uint8_t l3,
                                uint8_t h4, uint8_t l4, uint8_t h5, uint8_t l5,
                                uint8_t h6, uint8_t l6, uint8_t h7, uint8_t l7,
                                uint8_t h8, uint8_t l8, uint8_t h9, uint8_t l9);
    
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
    int ExampleFunction(int param1, int param2);


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
