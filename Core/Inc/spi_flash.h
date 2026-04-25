//
// Created by Arcana on 9/4/26.
//

#ifndef TFT_SCREEN_SPI_FLASH_H
#define TFT_SCREEN_SPI_FLASH_H

#endif //TFT_SCREEN_SPI_FLASH_H


#ifndef W25QXX_H
#define W25QXX_H

#include "stm32f1xx_hal.h"

// ===== PIN CONFIG =====
#define FLASH_CS_PORT GPIOB
#define FLASH_CS_PIN  GPIO_PIN_12

#define FLASH_CLK_PORT GPIOB
#define FLASH_CLK_PIN  GPIO_PIN_13

#define FLASH_MISO_PORT GPIOB
#define FLASH_MISO_PIN  GPIO_PIN_14

#define FLASH_MOSI_PORT GPIOB
#define FLASH_MOSI_PIN  GPIO_PIN_15

#define UART_DMA_BUF_SIZE 1024   // minimum 1KB (better 2KB)
#define IMAGE_SIZE 42768
#define FLASH_START_ADDR 0x000000
extern uint8_t uart_dma_buf[UART_DMA_BUF_SIZE];
extern uint32_t flash_addr;
// ===== MACROS =====
#define CS_LOW()   HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET)

#define CLK_LOW()  HAL_GPIO_WritePin(FLASH_CLK_PORT, FLASH_CLK_PIN, GPIO_PIN_RESET)
#define CLK_HIGH() HAL_GPIO_WritePin(FLASH_CLK_PORT, FLASH_CLK_PIN, GPIO_PIN_SET)

#define MOSI_LOW() HAL_GPIO_WritePin(FLASH_MOSI_PORT, FLASH_MOSI_PIN, GPIO_PIN_RESET)
#define MOSI_HIGH()HAL_GPIO_WritePin(FLASH_MOSI_PORT, FLASH_MOSI_PIN, GPIO_PIN_SET)

#define READ_MISO() HAL_GPIO_ReadPin(FLASH_MISO_PORT, FLASH_MISO_PIN)

// ===== API =====
void W25Q_Init(void);
uint8_t SPI_Transfer(uint8_t data);

void W25Q_WriteEnable(void);
uint8_t W25Q_ReadStatus(void);
void W25Q_WaitBusy(void);

void W25Q_Read(uint32_t addr, uint8_t *buf, uint16_t len);
void W25Q_PageProgram(uint32_t addr, uint8_t *buf, uint16_t len);
void W25Q_SectorErase(uint32_t addr);
uint32_t W25Q_Write(uint32_t addr, uint8_t *data, uint32_t len);
uint8_t UART_ReadByte(void);
uint32_t UART_ReadU32(void);
void Flash_Write_From_UART(void);
void Flash_Write_Stream(void);
uint16_t DMA_GetWritePtr(void);
void Process_UART_To_Flash(void);
void print(char *text);
void uint16ToHexStr(uint32_t address, char *str);
#endif

/*void printBinary(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    // Shift bit to the LSB position and mask it
    uint8_t bit = (byte >> i) & 1;
    // Convert to ASCII '0' or '1' and send back via UART
    char bitChar = bit ? '1' : '0';
    HAL_UART_Transmit(&huart1, (uint8_t*)&bitChar, 1, 10);
  }
  // Add a space between bytes for readability
  char space = ' || ';
  HAL_UART_Transmit(&huart1, (uint8_t*)&space, 1, 10);
}*/

/* USER CODE BEGIN 2
print(" /// START ERASING DATA /// \n");
for (uint32_t a = 0; a < 50000; a += 4096){
  W25Q_SectorErase(a);
}
print(" /// DATA ERASING COMPLETE /// \n");
HAL_UART_Receive_DMA(&huart1, uart_dma_buf, UART_DMA_BUF_SIZE);
/* USER CODE END 2 */

/* Infinite loop */
/* USER CODE BEGIN WHILE

while (1)
{
  Process_UART_To_Flash();
}
/* USER CODE END 3 */