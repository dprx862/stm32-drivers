/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_ina219_interface_template.c
 * @brief     driver ina219 interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-06-13
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/06/13  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_ina219_interface.h"
#include <stdarg.h>

/**
 * @brief iic bus send start
 * @note  none
 */
static void a_iic_start(void)
{
    I2C_SET_SCL
    I2C_SET_SDA
    I2C_DELAY
    I2C_CLEAR_SDA
    I2C_DELAY
    I2C_CLEAR_SCL
    I2C_DELAY
}

/**
 * @brief iic bus send stop
 * @note  none
 */
static void a_iic_stop(void)
{
    I2C_CLEAR_SDA
    I2C_DELAY
    I2C_SET_SCL
    I2C_DELAY
    I2C_SET_SDA
    I2C_DELAY
}

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t ina219_interface_iic_init(void)
{

    I2C_SET_SDA;
    I2C_SET_SCL;

    return 0;
}

static uint8_t a_iic_read_sda(void)
{
    if (HAL_GPIO_ReadPin(BM_SDA_GPIO_Port, BM_SDA_Pin) == GPIO_PIN_SET)
        return 1;
    else
        return 0;
    return 0;
}

/**
 * @brief  iic wait ack
 * @return status code
 *         - 0 get ack
 *         - 1 no ack
 * @note   none
 */
static uint8_t a_iic_wait_ack(void)
{
    uint16_t uc_err_time = 0;

    I2C_SET_SDA
    I2C_DELAY
    I2C_SET_SCL
    I2C_DELAY
    
    while (a_iic_read_sda() != 0)
    {
        uc_err_time++;
        if (uc_err_time > 250)
        {
            a_iic_stop();
            
            return 1;
        }
    }
    I2C_CLEAR_SCL;
    
    return 0;
}

/**
 * @brief iic bus send ack
 * @note  none
 */
static void a_iic_ack(void)
{
    I2C_CLEAR_SCL;
    I2C_CLEAR_SDA;
    I2C_DELAY;
    I2C_SET_SCL;
    I2C_DELAY;
    I2C_CLEAR_SCL;
}

/**
 * @brief iic bus send nack
 * @note  none
 */
static void a_iic_nack(void)
{
    I2C_CLEAR_SCL;
    I2C_SET_SDA;
    I2C_DELAY;
    I2C_SET_SCL;
    I2C_DELAY;
    I2C_CLEAR_SCL;
}

/**
 * @brief     iic send one byte
 * @param[in] txd is the sent byte
 * @note      none
 */
static void a_iic_send_byte(uint8_t txd)
{
    uint8_t t;
    
    I2C_CLEAR_SCL;
    for (t = 0; t < 8; t++)
    {

        if ((txd & 0x80)) {
            I2C_SET_SDA;
        }
        else
        {
            I2C_CLEAR_SDA;
        }

        txd <<= 1;
        I2C_DELAY;
        I2C_SET_SCL;
        I2C_DELAY;
        I2C_CLEAR_SCL;
    }
}

/**
 * @brief     iic read one byte
 * @param[in] ack is the sent ack
 * @return    read byte
 * @note      none
 */
static uint8_t a_iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;
    
    for (i = 0; i < 8; i++)
    {
        I2C_CLEAR_SCL;
        I2C_DELAY;
        I2C_SET_SCL;
        receive <<= 1;
        if (a_iic_read_sda() != 0)
        {
            receive++;
        }
        I2C_DELAY;
    }
    if (ack != 0)
    {
        a_iic_ack();
    }
    else
    {
        a_iic_nack();
    }
    
    return receive;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t ina219_interface_iic_deinit(void)
{

    I2C_SET_SDA;
    I2C_SET_SCL;

    return 0;
}

/**
 * @brief      interface iic bus read
 * @param[in]  addr is the iic device write address
 * @param[in]  reg is the iic register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ina219_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    /* send a start */
    a_iic_start();
    
    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();
        
        return 1;
    }
    
    /* send the reg */
    a_iic_send_byte(reg);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();
        
        return 1;
    }
    
    /* send a start */
    a_iic_start();
    
    /* send the read addr */
    a_iic_send_byte(addr + 1);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();
        
        return 1;
    }
    
    /* read the data */
    while (len != 0)
    {
        /* if the last */
        if (len == 1)
        {
            /* send nack */
            *buf = a_iic_read_byte(0);
        }
        else
        {
            /* send ack */
            *buf = a_iic_read_byte(1);
        }
        len--;
        buf++;
    }
    
    /* send a stop */
    a_iic_stop();
    
    return 0;
}

/**
 * @brief     interface iic bus write
 * @param[in] addr is the iic device write address
 * @param[in] reg is the iic register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ina219_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint16_t i; 
    
    /* send a start */
    a_iic_start();
    
    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();
        
        return 1;
    }
    
    /* send the reg */
    a_iic_send_byte(reg);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();
        
        return 1;
    }
    
    /* write the data */
    for (i = 0; i < len; i++)
    {
        /* send one byte */
        a_iic_send_byte(buf[i]);
        if (a_iic_wait_ack() != 0)
        {
            a_iic_stop(); 
            
            return 1;
        }
    }
    
    /* send a stop */
    a_iic_stop();

    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void ina219_interface_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void ina219_interface_debug_print(const char *const fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    printf(fmt, args);
    va_end(args);    
}
