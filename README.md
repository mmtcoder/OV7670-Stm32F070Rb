# OV7670-Stm32F070Rb

USING OV7670 WITH NUCLEO STM32F070RB

Main goal of this project is using OV7670( without FIFO)
 * using STM32F070RB and it consumes very less place in the SRAM. Every peripheral clock speed is set 48MHz.
 * Display Rendering Time approximately 2 second.
 * If you have using another board please check your System Clock and PWM setting to be 48MHz and (24Mhz for PWM).

For Pin Definitions Please Check main.h class 

SPI Pın Defitions that I used:
PA5 : SPI_SCK
PA6 : SPI_MISO
PA7 : SPI_MOSI
PC9 : SPI_NSS (or CS)

I2C Pin Definition :

PB9 : I2C_SDA
PB8 : I2C_SCL
