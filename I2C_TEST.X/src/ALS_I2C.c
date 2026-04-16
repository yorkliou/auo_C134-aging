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

/* This section lists the other files that are included in this file.
 */
#include "definitions.h"  // ??? Harmony ? SERCOM API ??????
#include <stdint.h>
#include <stdbool.h>
/* TODO:  Include other files here if needed. */
#define ALS_ADDR  0x29

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
/* ==========================================
 * SCL ?? (Push-Pull ??)
 * ????????????????
 * ???? Slave ?? Clock Stretching????
 * ========================================== */

void I2C_SCL_High(void) {
    ALS_SCL_InputEnable();   // ??
    // ?? OutputEnable
}

void I2C_SCL_Low(void) {
    ALS_SCL_Clear();
    ALS_SCL_OutputEnable();  // ????
}


/* ==========================================
 * SDA ?? (Open-Drain ????)
 * ???? Input/Output??? SDA ????
 * ========================================== */

void I2C_SDA_High(void) {
    ALS_SDA_Set();          
    ALS_SDA_InputEnable();  
}

void I2C_SDA_Low(void) {
    // ?? Low
    ALS_SDA_Clear();
    ALS_SDA_OutputEnable();
}

void i2c_start(void) {
    I2C_SDA_High();
    I2C_SCL_High();
    SYSTICK_DelayUs(5);
    I2C_SDA_Low();
    SYSTICK_DelayUs(5);
    I2C_SCL_Low();
}

void i2c_stop(void) {
    I2C_SCL_Low();
    I2C_SDA_Low();
    SYSTICK_DelayUs(5);
    I2C_SCL_High();
    SYSTICK_DelayUs(5);
    I2C_SDA_High();
    SYSTICK_DelayUs(5);
}

// ?? true ???? ACK, false ?? NACK
bool i2c_write_byte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        if ((data & 0x80) != 0) {
            I2C_SDA_High();
        } else {
            I2C_SDA_Low();
        }
        SYSTICK_DelayUs(5);
        I2C_SCL_High();
        SYSTICK_DelayUs(5);
        I2C_SCL_Low();
        data <<= 1;
    }

    // Read ACK
    I2C_SDA_High(); // Release SDA
    SYSTICK_DelayUs(5);
    I2C_SCL_High();
    SYSTICK_DelayUs(5);
    
    bool ack = (ALS_SDA_Get() == 0); // SDA Low ?? ACK
    
    I2C_SCL_Low();
    return ack;
}

uint8_t i2c_read_byte(bool send_ack) {
    uint8_t data = 0;
    I2C_SDA_High(); // Make sure SDA is input
    
    for (int i = 0; i < 8; i++) {
        data <<= 1;
        SYSTICK_DelayUs(5);
        I2C_SCL_High();
        SYSTICK_DelayUs(5);
        if (ALS_SDA_Get()) {
            data |= 0x01;
        }
        I2C_SCL_Low();
    }

    // Send ACK/NACK
    if (send_ack) {
        I2C_SDA_Low();
    } else {
        I2C_SDA_High();
    }
    SYSTICK_DelayUs(5);
    I2C_SCL_High();
    SYSTICK_DelayUs(5);
    I2C_SCL_Low();
    I2C_SDA_High(); // Release after ACK

    return data;
}

/* ==========================================
 * 4. ALS Sensor Functions (Ported from Python)
 * ========================================== */

#define ALS_ADDR    0x29  // 0x52 >> 1

// ?? Python ? writeto_mem: Start -> Addr(W) -> Reg -> Data -> Stop
bool i2c_write_mem(uint8_t dev_addr, uint8_t reg, uint8_t data) {
    i2c_start();
    if (!i2c_write_byte((dev_addr << 1) | 0)) { // Write Mode
        i2c_stop(); return false; 
    }
    if (!i2c_write_byte(reg)) {
        i2c_stop(); return false;
    }
    if (!i2c_write_byte(data)) {
        i2c_stop(); return false;
    }
    i2c_stop();
    return true;
}

// ?? Python ? readfrom_mem: Start -> Addr(W) -> Reg -> Restart -> Addr(R) -> Data -> Stop
bool i2c_read_mem(uint8_t dev_addr, uint8_t reg, uint8_t *data) {
    i2c_start();
    if (!i2c_write_byte((dev_addr << 1) | 0)) { // Write Mode
        i2c_stop(); return false;
    }
    if (!i2c_write_byte(reg)) {
        i2c_stop(); return false;
    }
    
    // Restart
    i2c_start(); 
    if (!i2c_write_byte((dev_addr << 1) | 1)) { // Read Mode
        i2c_stop(); return false;
    }
    
    *data = i2c_read_byte(false); // NACK (???? byte)
    i2c_stop();
    return true;
}

void als_init(void) {
    // i2c.writeto_mem(0x52>>1, 0x00, 0x50)
    if (!i2c_write_mem(ALS_ADDR, 0x00, 0x50)) {
        
    }
    SYSTICK_DelayMs(100);

    // i2c.writeto_mem(0x52>>1, 0x01, 0x09)
    if (!i2c_write_mem(ALS_ADDR, 0x01, 0x09)) {
    }
    SYSTICK_DelayMs(100);
}

int als_id_check(void) {
    uint8_t id_l, id_h;

    // Read LSB (0x14)
    if (!i2c_read_mem(ALS_ADDR, 0x14, &id_l)) {
        return 0;
    }
    SYSTICK_DelayMs(100);

    // Read MSB (0x15)
    if (!i2c_read_mem(ALS_ADDR, 0x15, &id_h)) {
        return 0;
    }
    SYSTICK_DelayMs(100);



    if (id_h == 0x00 && id_l == 0x01) { // ?? Python ? ID ????
        return 1;
    } else {
        return 0;
    }
}

void als_data_read(void) {
    uint8_t data_l, data_h;

    // Read Data Low (0x10)
    if (!i2c_read_mem(ALS_ADDR, 0x10, &data_l)) {
        return;
    }
    SYSTICK_DelayMs(100);

    // Read Data High (0x11)
    if (!i2c_read_mem(ALS_ADDR, 0x11, &data_h)) {
        return;
    }
    SYSTICK_DelayMs(100);

    uint16_t als_dec = (data_h * 256) + data_l;
    float als_lx = als_dec * 0.0136f;
    
    if(als_lx)
    {
        
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
