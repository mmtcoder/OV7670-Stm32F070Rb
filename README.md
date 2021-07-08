# Using OV7670 Camera with STM32f070RB(Nucleo Board) and IL9341 LED(With SPI)

Main goal of this project is getting frame syncronuisly and display on the IL9341 LED( without FIFO)
 * using STM32F070RB and it consumes very less place in the SRAM. Every peripheral clock speed is set 48MHz.
 * Display Rendering Time approximately 2 second.
 * If you have using another board please check your System Clock and PWM setting to be 48MHz and (24Mhz for PWM).

For Pin Definitions Please Check main.h class 

SPI Pın Definitions :
PA5 : SPI_SCK
PA6 : SPI_MISO
PA7 : SPI_MOSI
PC9 : SPI_NSS (or CS)

I2C Pin Definition :

PB9 : I2C_SDA
PB8 : I2C_SCL

NOTE : I didnt use 5x5 font.h (or function of the IL9341_GFX.h like DrawCircle, DrawRectangle etc..) for this project.
I didn't delete the codes because maybe it can be useful.
