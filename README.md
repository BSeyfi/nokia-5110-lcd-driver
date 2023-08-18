![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
# Nokia 5110 LCD library for Arm® Cortex™-M microcontrollers

## What is it?
This library is a fast and resource-friendly driver for Nokia 5110 LCD designed for STM32F1 Arm-Cortex microcontrollers, providing a collection of functions to facilitate interaction with the display. These functions encompass various capabilities such as initializing the LCD, manipulating the display buffer, and efficiently updating screen content.

## Demo Pics

- ### Character Printing Functionality
  Sample demonstration of the `LCD_write_string` and `LCD_write_char_8x6` functions:
  ![Sample of `write_char` and `write_string`](./assets/ascii-letters.jpg)
  ![Sample of `write_char` and `write_string`](./assets/nokia-driver.jpg)

- ### Picture Drawing Functionality
  Sample demonstration of the `LCD_write_full_pic` function:
  ![Sample of `LCD_write_full_pic`](./assets/dorood%20o%20do%20sad%20bodorood.jpg)

Please review the improved version. If it meets your expectations, feel free to use it as is!

## Features - LCD Driver Functions

1. **`LCD_Init(uint8_t contrast)`**

   Initializes the LCD display with a specified contrast value in the range of 0 to 127.

2. **`LCD_clear()`**

   Clears the entire display buffer.

3. **`LCD_update()`**

   Updates the LCD display by refreshing only the changed sections of the buffer.

4. **`LCD_goto_x_y_chunk(uint16_t x, uint16_t y)`**

   Positions the cursor at a specific 8-bit chunk within the buffer area. Each chunk vertically spans 8 pixels.

5. **`LCD_write_char_8x6(uint8_t chr)`**

   Writes an ASCII character at the current cursor position. Each character occupies an 8x6 pixel matrix.

6. **`LCD_write_string(char *str)`**

   Writes a string of ASCII characters to the buffer, advancing the cursor accordingly.

7. **`LCD_goto_x_y_char_8x6(uint16_t x, uint16_t y)`**

   Sets the cursor to a designated 8x6 pixel character within the buffer.

8. **`decompress_into_buffer(uint8_t *compressed_image, uint8_t *LCD_buffer, uint16_t x_start, uint16_t y_start)`**

   Decompresses a compressed image and copies it into the LCD buffer, starting from specified coordinates. To compress an image, you can use [BICTES](https://github.com/BSeyfi/BICTES) free tool, which employs the [Bicolor Image Compression Protocol For Embedded Systems](https://github.com/BSeyfi/bicolor-image-compression-protocol-for-embedded-systems) for image compression.

9. **`LCD_write_full_pic(uint8_t full_pic[])`**

   Displays a complete image on the LCD using 8-pixel tall chunks in an x-y format. This function utilizes an array `full_pic` containing pixel data. Each chunk contributes to forming the full image on the display.

## Requirements
This project is written with C language and depends on `stm32f1xx_hal.h` and `core_cm3.h` and some other header files that you can find them on [STM32Cube MCU Package for STM32F1 series](https://www.st.com/en/embedded-software/stm32cubef1.html).


## Documentation
This light library is well documented using Doxygen. You can find them on functions signature.

## License

This project is licensed under the [MIT License](./LICENSE). 

The full text of the license can be found in the `LICENSE` file in the root directory of this repository.

### Copyright 2018 Behzad Seyfi