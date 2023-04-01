/**
*  @file lcd_5110.h
*  @brief LCD 5110 Initializer
*  @author Behzad Seyfi 
*  (c) 2018
*/

#ifndef LCD_5110_DRIVER
#define LCD_5110_DRIVER

#ifdef __cplusplus
extern "C"
{
#endif

#include "userconf.h"


/**
 * @brief Initializes LCD - MUST BE call as soon as possible- Reset pin must be low as default
 * @param contrast set contrast of LCD 0-127
 */
void LCD_Init(uint8_t contrast);

/**
 * @brief clears whole buffer
 */
void LCD_clear(void);

/**
 * @brief updates LCD smartly - updates LCD with changed part of buffer
 */
void LCD_update(void);

/**
 * @brief goto 8-bit chunck in buffer area
 * @param x x of chunk
 * @param y y of chunk
 * @note  each chunk consists of 8 pixel vertically
 */
void LCD_goto_x_y_chunk(uint16_t x, uint16_t y);

/**
 * @brief write an ascii character in current cursor of buffer (cursor also goes forward) - each character is 8x6 pixels
 * @param chr
 */
void LCD_write_char_8x6(uint8_t chr);

/**
 * @brief write ascii based string in buffer
 * @param str an array of chars
 */
void LCD_write_string(char *str);

/**
 * @brief goto 8x6 pixel char in buffer
 * @param x x of char
 * @param y y of char
 * @note each char consists of 8x6 pixels
 */
void LCD_goto_x_y_char_8x6(uint16_t x, uint16_t y);

void decompress_into_buffer(uint8_t *compressed_image, uint8_t *LCD_buffer, uint16_t x_start, uint16_t y_start);
//todo 2 may be remove due to further updates - write 8x1 chunk
void LCD_write_full_pic(uint8_t full_pic[]);

#ifdef __cplusplus
}  /* extern "C" */
#endif


#endif
