//
// Created by Arcana on 8/4/26.
//

#include "../Inc/tft_driver.h"

#include <string.h>

#include "main.h"
#include "spi_flash.h"

extern SPI_HandleTypeDef hspi1;

extern uint8_t data_t[]= {IDMON};

void tft_send(uint8_t *data, _Bool type, uint16_t length) {
    if (type) DC_DATA; else DC_COMMAND;
    HAL_SPI_Transmit(&hspi1, data, length, 100);
}

void tft_init(void) {
    RES_LOW;
    HAL_Delay(2);
    RES_HIGH;
    TCS_LOW;
    HAL_Delay(120);
    tft_send((uint8_t[]){SWERSET},0,1);
    HAL_Delay(120);
    tft_send((uint8_t[]){SLPOUT},0,1); //sleep out
    HAL_Delay(120);
    //set interface pixel format
    tft_send((uint8_t[]){COLMOD},0,1);
    tft_send((uint8_t[]){0x05},1,1);//configuration

    // personal data
    tft_send((uint8_t[]){INVON},0,1);
    tft_send((uint8_t[]){GAMSET},0,1);
    tft_send((uint8_t[]){0x08},1,1);

    // memory data access control
    tft_send((uint8_t[]){MADCTL},0,1);
    tft_send((uint8_t[]){0x00},1,1);//configuration

    // display on
    tft_send((uint8_t[]){DISPON},0,1);
    // CS_HIGH;
}
void tft_def_window(uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye) {
    uint8_t cmd_x[] = {xs >> 8, xs & 0xFF, xe >> 8, xe & 0xFF};
    uint8_t cmd_y[] = {ys >> 8, ys & 0xFF, ye >> 8, ye & 0xFF};
    TCS_LOW;
    tft_send((uint8_t[]){CASET}, 0, 1);
    tft_send(cmd_x, 1, 4);
    tft_send((uint8_t[]){RASET}, 0, 1);
    tft_send(cmd_y, 1, 4);
    // CS_HIGH;
}
void send_data(uint32_t start_addr, uint32_t end_addr) {
    tft_send((uint8_t[]){RAMWR}, 0, 1);
    uint8_t buffer[256];
    uint32_t address = start_addr;
    uint32_t remaining = end_addr - start_addr;
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
    while (remaining > 0)
    {
        uint16_t chunk = (remaining > sizeof(buffer)) ? sizeof(buffer) : remaining;

        // Read from W25Q into buffer
        W25Q_Read(address, buffer, chunk);
        // Send directly to LCD
        HAL_SPI_Transmit(&hspi1, buffer, chunk, HAL_MAX_DELAY);
        address += chunk;
        remaining -= chunk;
    }
    TCS_HIGH;
}










