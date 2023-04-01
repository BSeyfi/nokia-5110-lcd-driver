/**
 *  @file lcd_5110.c
 *  @brief LCD 5110 Lib
 *  @author Behzad Seyfi
 *  COPYRIGHT (c) 2018
 *
 *
 *  Define these pins in your program and hardware
 *	Define 	SPI and name its pins as below
 *  LCD_CLK  : Clock
 *  LCD_MOSI : MOSI
 *  LCD_DC	 : Data/Command
 *  LCD SPI Channel: MAY CHANGE
 */

#include "userconf.h"
#include "timeb.h"
#include "lcd_5110.h"

//file scope variables
static uint8_t LCD_x;
static uint8_t LCD_y;

#ifndef LCD_SPI_Handler
assert("LCD_SPI_Handler is not declared. Declare it in userconf.h")
#endif // !LCD_SPI_Handler

// 5510 Command Set
//Function Set
#define LCD_POWERDOWN_ENABLE                    0x24
#define LCD_POWERDOWN_DISABLE                   0x20

#define LCD_VERTICAL_ENABLE                     0x22
#define LCD_VERTICAL_DISABLE                    0x20

#define LCD_H_EXTENDED_INSTRUCTION              0x21
#define LCD_H_SIMPLE_INSTRUCTION                0x20

#define LCD_DISPLAY_CONTROL_DISPLAY_BLANK        0x08
#define LCD_DISPLAY_CONTROL_NORMAL_MODE         0x0c
#define LCD_DISPLAY_CONTROL_ALL_SEGMENTS_ON     0x09
#define LCD_DISPLAY_CONTROL_INVERTED_MODE       0x0d

#define LCD_SET_Y_ADDRESS                       0x40
#define LCD_SET_X_ADDRESS                       0x80

#define LCD_TEMPERATURE_CONTROL                 0x04

#define LCD_BIAS_SYSTEM                         0x10

#define LCD_SET_VOP                             0x80

#define LCD_X_PIXEL_WIDTH                       84
#define LCD_Y_PIXEL_HEIGTH                      48

#define LCD_X_MAX_CHUNK                         83  //0-83
#define LCD_Y_MAX_CHUNK                         5   //0-5

#define LCD_WIDTH_IN_CHUNK                      84
#define LCD_HEIGHT_IN_CHUNK                     6


#define LCD_BUFFER_SIZE                        504 // == 84*48/8


//Buffer & LCD Curser
//changed area of buffer in pixel
static uint8_t changed_buffer_x_low  = 0;
static uint8_t changed_buffer_y_low  = 0;
static uint8_t changed_buffer_x_high = 0;
static uint8_t changed_buffer_y_high = 0;

// this amount is for initial clearing of lcd
static uint16_t start_in_line_update  = 0;
static uint16_t end_in_line_update    = 0;
static uint16_t cursor_in_line_update = 0;

//courser current position in LCD in Chunks(not pixels)
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static struct _Flag {
    uint8_t in_line_changed : 1;
    uint8_t area_changed : 1;
    
} buffer_flag                     = {0, 0};//flags are used to update lcd and in its special format show in which format

//Display Buffer
static uint8_t buffer[LCD_BUFFER_SIZE];
//Fonts 5x7
const uint8_t  font_en_8x5[95][5] = {
        {0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x2f, 0x00, 0x00},
        {0x00, 0x07, 0x00, 0x07, 0x00},
        {0x14, 0x7f, 0x14, 0x7f, 0x14},
        {0x24, 0x2a, 0x7f, 0x2a, 0x12},
        {0x32, 0x34, 0x08, 0x16, 0x26},
        {0x36, 0x49, 0x55, 0x22, 0x50},
        {0x00, 0x05, 0x03, 0x00, 0x00},
        {0x00, 0x1c, 0x22, 0x41, 0x00},
        {0x00, 0x41, 0x22, 0x1c, 0x00},
        {0x14, 0x08, 0x3E, 0x08, 0x14},
        {0x08, 0x08, 0x3E, 0x08, 0x08},
        {0x00, 0x00, 0x50, 0x30, 0x00},
        {0x10, 0x10, 0x10, 0x10, 0x10},
        {0x00, 0x60, 0x60, 0x00, 0x00},
        {0x20, 0x10, 0x08, 0x04, 0x02},
        {0x3E, 0x51, 0x49, 0x45, 0x3E},
        {0x00, 0x42, 0x7F, 0x40, 0x00},
        {0x42, 0x61, 0x51, 0x49, 0x46},
        {0x21, 0x41, 0x45, 0x4B, 0x31},
        {0x18, 0x14, 0x12, 0x7F, 0x10},
        {0x27, 0x45, 0x45, 0x45, 0x39},
        {0x3C, 0x4A, 0x49, 0x49, 0x30},
        {0x01, 0x71, 0x09, 0x05, 0x03},
        {0x36, 0x49, 0x49, 0x49, 0x36},
        {0x06, 0x49, 0x49, 0x29, 0x1E},
        {0x00, 0x36, 0x36, 0x00, 0x00},
        {0x00, 0x56, 0x36, 0x00, 0x00},
        {0x08, 0x14, 0x22, 0x41, 0x00},
        {0x14, 0x14, 0x14, 0x14, 0x14},
        {0x00, 0x41, 0x22, 0x14, 0x08},
        {0x02, 0x01, 0x51, 0x09, 0x06},
        {0x32, 0x49, 0x59, 0x51, 0x3E},
        {0x7E, 0x11, 0x11, 0x11, 0x7E},
        {0x7F, 0x49, 0x49, 0x49, 0x36},
        {0x3E, 0x41, 0x41, 0x41, 0x22},
        {0x7F, 0x41, 0x41, 0x22, 0x1C},
        {0x7F, 0x49, 0x49, 0x49, 0x41},
        {0x7F, 0x09, 0x09, 0x09, 0x01},
        {0x3E, 0x41, 0x49, 0x49, 0x7A},
        {0x7F, 0x08, 0x08, 0x08, 0x7F},
        {0x00, 0x41, 0x7F, 0x41, 0x00},
        {0x20, 0x40, 0x41, 0x3F, 0x01},
        {0x7F, 0x08, 0x14, 0x22, 0x41},
        {0x7F, 0x40, 0x40, 0x40, 0x40},
        {0x7F, 0x02, 0x0C, 0x02, 0x7F},
        {0x7F, 0x04, 0x08, 0x10, 0x7F},
        {0x3E, 0x41, 0x41, 0x41, 0x3E},
        {0x7F, 0x09, 0x09, 0x09, 0x06},
        {0x3E, 0x41, 0x51, 0x21, 0x5E},
        {0x7F, 0x09, 0x19, 0x29, 0x46},
        {0x46, 0x49, 0x49, 0x49, 0x31},
        {0x01, 0x01, 0x7F, 0x01, 0x01},
        {0x3F, 0x40, 0x40, 0x40, 0x3F},
        {0x1F, 0x20, 0x40, 0x20, 0x1F},
        {0x3F, 0x40, 0x38, 0x40, 0x3F},
        {0x63, 0x14, 0x08, 0x14, 0x63},
        {0x07, 0x08, 0x70, 0x08, 0x07},
        {0x61, 0x51, 0x49, 0x45, 0x43},
        {0x00, 0x7F, 0x41, 0x41, 0x00},
        {0x55, 0x2A, 0x55, 0x2A, 0x55},
        {0x00, 0x41, 0x41, 0x7F, 0x00},
        {0x04, 0x02, 0x01, 0x02, 0x04},
        {0x40, 0x40, 0x40, 0x40, 0x40},
        {0x00, 0x01, 0x02, 0x04, 0x00},
        {0x20, 0x54, 0x54, 0x54, 0x78},
        {0x7F, 0x48, 0x44, 0x44, 0x38},
        {0x38, 0x44, 0x44, 0x44, 0x20},
        {0x38, 0x44, 0x44, 0x48, 0x7F},
        {0x38, 0x54, 0x54, 0x54, 0x18},
        {0x08, 0x7E, 0x09, 0x01, 0x02},
        {0x0C, 0x52, 0x52, 0x52, 0x3E},
        {0x7F, 0x08, 0x04, 0x04, 0x78},
        {0x00, 0x44, 0x7D, 0x40, 0x00},
        {0x20, 0x40, 0x44, 0x3D, 0x00},
        {0x7F, 0x10, 0x28, 0x44, 0x00},
        {0x00, 0x41, 0x7F, 0x40, 0x00},
        {0x7C, 0x04, 0x18, 0x04, 0x78},
        {0x7C, 0x08, 0x04, 0x04, 0x78},
        {0x38, 0x44, 0x44, 0x44, 0x38},
        {0x7C, 0x14, 0x14, 0x14, 0x08},
        {0x08, 0x14, 0x14, 0x18, 0x7C},
        {0x7C, 0x08, 0x04, 0x04, 0x08},
        {0x48, 0x54, 0x54, 0x54, 0x20},
        {0x04, 0x3F, 0x44, 0x40, 0x20},
        {0x3C, 0x40, 0x40, 0x20, 0x7C},
        {0x1C, 0x20, 0x40, 0x20, 0x1C},
        {0x3C, 0x40, 0x30, 0x40, 0x3C},
        {0x44, 0x28, 0x10, 0x28, 0x44},
        {0x0C, 0x50, 0x50, 0x50, 0x3C},
        {0x44, 0x64, 0x54, 0x4C, 0x44},
        {0x00, 0x08, 0x36, 0x41, 0x00},
        {0x00, 0x00, 0x7F, 0x00, 0x00},
        {0x00, 0x41, 0x36, 0x08, 0x00},
        {0x10, 0x08, 0x08, 0x10, 0x08}
};

//private in-lib functions

/**
  * @brief Send single data byte
  * @param command command byte to be send
  */
void _send_single_command(uint8_t command);

/**
 * @brief Send single data byte
 * @param data data byte to be sent
 */
void _send_single_data(uint8_t data);


/**
 * @brief Send multi-byte in serie from an array
 * @param data data sub-array to be sent
 * @param start_index first index of data chunk selected from array (starting byte)
 * @param end_index last index of data chunk selected from array
 */
void _send_multi_data(uint8_t data[], size_t start_index, size_t end_index);

void LCD_Init(uint8_t contrast) {
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, 0);
    //wait
    delayUS_DWT(100);
    
    //Reset bit initialize
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, 1);
    //wait
    delayUS_DWT(100);
    
    //Reset
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, 0);
    delayUS_DWT(1000);
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, 1);
    
    //wait
    delayUS_DWT(10);
    
    //Extended Mode Enable
    _send_single_command(LCD_H_EXTENDED_INSTRUCTION);
    
    //Set Contrast(VOP)
    if (contrast > 0x7f) {
        contrast = 0x7f;
    }
    _send_single_command(LCD_SET_VOP | contrast);
    
    //Set Bias
    _send_single_command(LCD_BIAS_SYSTEM | 0x03);
    
    //Extended Mode Disable
    _send_single_command(LCD_H_SIMPLE_INSTRUCTION);
    
    //Display in Normal Mode
    _send_single_command(LCD_DISPLAY_CONTROL_NORMAL_MODE);
    
    //Set X,Y
    _send_single_command(LCD_SET_X_ADDRESS | 0x00);
    _send_single_command(LCD_SET_Y_ADDRESS | 0x00);
    
    //Clear buffer
    LCD_clear();
    
    //update
    LCD_update();
}

void _send_single_command(uint8_t command) {
    //select to write command
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 0);
    
    //Non-Blocking SPI Transmit---Check does it sent before following on
    HAL_SPI_Transmit(&LCD_SPI_Handler, &command, 1, 1);
    
    while (HAL_SPI_GetState(&LCD_SPI_Handler) != HAL_SPI_STATE_READY);
}

void _send_single_data(uint8_t data) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 1);
    //Non-Blocking SPI Transmit---Check for if it does sent before follow on
    HAL_SPI_Transmit(&LCD_SPI_Handler, &data, 1, 1);
    while (HAL_SPI_GetState(&LCD_SPI_Handler) != HAL_SPI_STATE_READY);
}

/// <summary>
///	sends multiple bytes in a row
/// </summary>
/// <param name="data">data array contains data bytes to be sent</param>
/// <param name="start_index"></param>
/// <param name="end_index"></param>
void _send_multi_data(uint8_t data[], size_t start_index, size_t end_index) {
    //select to write command
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 1);
    
    //To control max allowed elements
    if (end_index < start_index)
        return;
    if (end_index > LCD_BUFFER_SIZE)
        end_index = LCD_BUFFER_SIZE;
    
    //Non-Blocking SPI Transmit---Check does it sent before following on
    HAL_SPI_Transmit(&LCD_SPI_Handler, data + start_index, end_index - start_index, 10);
    while (HAL_SPI_GetState(&LCD_SPI_Handler) != HAL_SPI_STATE_READY);
    
}

void LCD_clear(void) {
    for (uint16_t i = 0; i < LCD_BUFFER_SIZE; i++)
        buffer[i] = 0x00;
    start_in_line_update  = 0;
    end_in_line_update    = LCD_BUFFER_SIZE;
    cursor_in_line_update = LCD_BUFFER_SIZE;
    buffer_flag.in_line_changed = 1;
}

void LCD_write_char_8x6(uint8_t chr)//todo 1 I char chr)
{
    // if a new charater is entered but buffer is full then regret it.
    if (cursor_in_line_update + 6 > LCD_BUFFER_SIZE)
        return;
    for (uint16_t n = 0; n < 5; n++)
        buffer[cursor_in_line_update++] = font_en_8x5[chr - 32][n];
    //add a vertical space also horizontal row pixels is integral multipe of one charcter
    buffer[cursor_in_line_update++]     = 0x00;
    //if cursor is out of <start to end region> then update end_in_line_update
    if (cursor_in_line_update > end_in_line_update)
        end_in_line_update = cursor_in_line_update;
    buffer_flag.in_line_changed = 1;
    
}


void LCD_update(void) {
    //if both  types of buffer demonestrations have been changed
    if (buffer_flag.area_changed && buffer_flag.in_line_changed) {
        //todo Improve
    }
        //if just in-line type of buffer has been changed
    else if (buffer_flag.in_line_changed) {
        //goto xy command to lcd
        _send_single_command(LCD_SET_X_ADDRESS | (start_in_line_update % LCD_BUFFER_SIZE) % (LCD_X_MAX_CHUNK + 1));
        _send_single_command(
                LCD_SET_Y_ADDRESS | (uint8_t) ((start_in_line_update % LCD_BUFFER_SIZE) / (LCD_X_MAX_CHUNK + 1)));
        
        _send_multi_data(buffer, start_in_line_update, end_in_line_update);
        
        start_in_line_update  = end_in_line_update % LCD_BUFFER_SIZE;
        end_in_line_update    = start_in_line_update;
        cursor_in_line_update = start_in_line_update;
        buffer_flag.in_line_changed = 0;
        
    }
        //if just area type of buffer has been changed
    else if (buffer_flag.area_changed) {
        //todo Improve
    }
}


void LCD_goto_x_y_chunk(uint16_t x, uint16_t y) {
    //control to not be out of range
    if (x > LCD_X_MAX_CHUNK || y > LCD_Y_MAX_CHUNK || x < 0 || y < 0)
        return;
    
    uint16_t x_y_in_line_format = x + y * (LCD_X_MAX_CHUNK + 1);
    if (start_in_line_update > x_y_in_line_format)
        start_in_line_update = x_y_in_line_format;
    
    if (end_in_line_update < x_y_in_line_format)
        end_in_line_update = x_y_in_line_format;
    
    cursor_in_line_update = x_y_in_line_format;
    
}

void LCD_goto_x_y_char_8x6(uint16_t x, uint16_t y) {
    LCD_goto_x_y_chunk(x * (LCD_Y_MAX_CHUNK + 1), y);
}


void LCD_write_string(char *str) {
    while (*str) {
        LCD_write_char_8x6(*str);
        str++;
    }
}

void decompress_into_buffer(uint8_t *compressed_image, uint8_t *LCD_buffer, uint16_t x_start, uint16_t y_start) {
    //# No marker for LCD_buffer is dedicated. This means whole buffer MUST be refreshed afterwards.
    //protocol version = 1
    //For V1 no extra code needed
    //
    uint32_t index_compressedImage = 0;
    uint16_t width_compressedImage;
    uint16_t height_compressedImage;
    uint16_t offset_compressedImage;
    uint8_t  blank_chunk_color;//==1 means dominant color is inverted(pixel's state on ==BLACK)
    
    //index 0 = version and inversion state
    if (compressed_image[index_compressedImage++] == 1)
        blank_chunk_color = 255;
    else
        blank_chunk_color      = 0;
    //index 1 & 1+ : width
    width_compressedImage      = compressed_image[index_compressedImage++];
    if (width_compressedImage > 127)
        width_compressedImage  = width_compressedImage & (uint16_t) 127 + compressed_image[index_compressedImage++];
    //index 2 & 2+ : height
    height_compressedImage     = compressed_image[index_compressedImage++];
    if (height_compressedImage > 127)
        height_compressedImage = height_compressedImage & (uint16_t) 127 + compressed_image[index_compressedImage++];
    //index 3 & 3+ : offset
    offset_compressedImage     = compressed_image[index_compressedImage++];
    if (offset_compressedImage > 127)
        offset_compressedImage = offset_compressedImage & (uint16_t) 127 + compressed_image[index_compressedImage++];
    
    //# check margins to not be out of LCD boarder and correct it if needed
    if (x_start + width_compressedImage > LCD_WIDTH_IN_CHUNK)
        x_start           = LCD_WIDTH_IN_CHUNK - width_compressedImage;
    if (y_start + height_compressedImage > LCD_HEIGHT_IN_CHUNK)
        y_start           = LCD_HEIGHT_IN_CHUNK - height_compressedImage;
    
    
    //todo move variables to top
    uint16_t non_blank_chunk_count;
    uint16_t blank_chunk_count;
    uint16_t index_buffer = (uint16_t) (x_start + y_start * LCD_WIDTH_IN_CHUNK);
    uint16_t x_end        = x_start + width_compressedImage;
    uint16_t fragment_first_x_pos;//always becomes 0< x <width of image in each row iteration
    uint16_t fragment_last_x_pos;
    uint16_t counter;
    
    //write blank chunks to offset part
    for (counter = 0; counter < offset_compressedImage; counter++)
        LCD_buffer[index_buffer++] = blank_chunk_color;
    
    //# Write down bytes
    
    
    //write fragments
    fragment_first_x_pos = offset_compressedImage;
    fragment_last_x_pos  = offset_compressedImage;//will be added with non_blank_chunk_count after extraction
    do {
        non_blank_chunk_count     = compressed_image[index_compressedImage];
        //read non-blank chunks count
        if (non_blank_chunk_count > 127)
            non_blank_chunk_count = non_blank_chunk_count & (uint16_t) 127 + compressed_image[index_compressedImage++];
        fragment_last_x_pos += non_blank_chunk_count;
        
        //read blank chunks count
        blank_chunk_count     = compressed_image[index_compressedImage];
        if (blank_chunk_count > 127)
            blank_chunk_count = blank_chunk_count & (uint16_t) 127 + compressed_image[index_compressedImage];
        
        
        //write chunk bytes to buffer
        
        //# Ver2
        uint16_t rows        = (uint16_t) (fragment_last_x_pos / width_compressedImage);
        uint16_t current_row = 0;
        do {
            uint16_t k;
            if (current_row == rows)
                //process last row in multiple rows beside single row
                k = fragment_last_x_pos % width_compressedImage - (uint16_t) (rows == 0) * fragment_first_x_pos;
            else
                k = current_row == 0 ? width_compressedImage - fragment_first_x_pos : width_compressedImage;
            
            for (uint16_t counter2 = 0; counter2 < k; counter2++)
                LCD_buffer[index_buffer++] = compressed_image[index_compressedImage++];
            //if there is just single row or we are in last row then don't goto head of next non-existing line
            index_buffer += (uint16_t) (current_row < rows) * LCD_WIDTH_IN_CHUNK - width_compressedImage;
            //todo blank and improve non-blank
        } while (current_row++ < rows);
        fragment_first_x_pos = fragment_last_x_pos % width_compressedImage;
        fragment_last_x_pos =
                blank_chunk_count != 0 ? fragment_first_x_pos + blank_chunk_count : width_compressedImage;
        
        //# blanks
        rows                 = (uint16_t) (fragment_last_x_pos / width_compressedImage);
        current_row          = 0;
        do {
            uint16_t k;
            if (current_row == rows)
                //process last row in multiple rows beside single row
                k = fragment_last_x_pos % width_compressedImage - (uint16_t) (rows == 0) * fragment_first_x_pos;
            else
                k = current_row == 0 ? width_compressedImage - fragment_first_x_pos : width_compressedImage;
            
            for (uint16_t counter2 = 0; counter2 < k; counter2++)
                LCD_buffer[index_buffer++] = blank_chunk_color;
            //if there is just single row or we are in last row then don't goto head of next non-existing line
            index_buffer += (uint16_t) (current_row < rows) * LCD_WIDTH_IN_CHUNK - width_compressedImage;
            //todo blank and improve non-blank
        } while (current_row++ < rows);
        //fixme
        fragment_first_x_pos = fragment_last_x_pos % width_compressedImage;
        fragment_last_x_pos =
                blank_chunk_count != 0 ? fragment_first_x_pos + blank_chunk_count : width_compressedImage;
        
    } while (blank_chunk_count == 0);
}

//todo remove or improve(little image but full buffer-->bad image)
void LCD_write_full_pic(uint8_t full_pic[]) {
    LCD_goto_x_y_chunk(0, 0);
    int n;
    for (n = 0; n < LCD_BUFFER_SIZE; n++)
        buffer[n] = full_pic[n];
    cursor_in_line_update  = n;
    if (cursor_in_line_update > end_in_line_update)
        end_in_line_update = cursor_in_line_update;
    buffer_flag.in_line_changed = 1;
}
