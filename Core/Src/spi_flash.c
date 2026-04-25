//
// Created by Arcana on 9/4/26.
//

#include "../Inc/spi_flash.h"

#include <string.h>

extern UART_HandleTypeDef huart1;
void print(char *text) {
    HAL_UART_Transmit(&huart1, (uint8_t *)text, strlen(text), 1000);
}
static void SPI_Delay(void) {
}

uint8_t SPI_Transfer(uint8_t data)
{
    uint8_t recv = 0;
    for(int i=0;i<8;i++)
    {
        // MOSI
        if(data & 0x80) MOSI_HIGH();
        else MOSI_LOW();

        data <<= 1;

        // CLK rising edge
        CLK_HIGH();
        SPI_Delay();

        // Read MISO
        recv <<= 1;
        if(READ_MISO()) recv |= 1;

        // CLK falling edge
        CLK_LOW();
        SPI_Delay();
    }
    return recv;
}

void W25Q_WriteEnable(void)
{
    CS_LOW();
    SPI_Transfer(0x06); // Write Enable
    CS_HIGH();
}

uint8_t W25Q_ReadStatus(void)
{
    uint8_t status;

    CS_LOW();
    SPI_Transfer(0x05); // Read Status Register
    status = SPI_Transfer(0xFF);
    CS_HIGH();

    return status;
}
void W25Q_WaitBusy(void)
{
    while(W25Q_ReadStatus() & 0x01); // BUSY bit
}

void W25Q_Read(uint32_t addr, uint8_t *buf, uint16_t len)
{
    CS_LOW();

    SPI_Transfer(0x03); // Read Data
    SPI_Transfer(addr >> 16);
    SPI_Transfer(addr >> 8);
    SPI_Transfer(addr);

    for(int i=0;i<len;i++)
        buf[i] = SPI_Transfer(0xFF);

    CS_HIGH();
}

void W25Q_PageProgram(uint32_t addr, uint8_t *buf, uint16_t len)// MAX 256 bytes !!
{
    W25Q_WriteEnable();

    CS_LOW();

    SPI_Transfer(0x02); // Page Program

    SPI_Transfer(addr >> 16);
    SPI_Transfer(addr >> 8);
    SPI_Transfer(addr);

    for(int i=0;i<len;i++)
        SPI_Transfer(buf[i]);

    CS_HIGH();

    W25Q_WaitBusy();
}
uint32_t W25Q_Write(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t remaining = len;
    //uint32_t addr = addre; // fucking Genius coding by AI
    while (remaining > 0)
    {
        uint16_t page_offset = addr % 256;
        uint16_t space = 256 - page_offset;

        uint16_t chunk = (remaining < space) ? remaining : space;

        W25Q_PageProgram(addr, data, chunk);

        addr += chunk;
        data += chunk;
        remaining -= chunk;
    }
    return addr;
}



volatile uint16_t dma_write_ptr;  // where DMA is writing
uint16_t read_ptr = 0;            // where you read from
uint8_t uart_dma_buf[UART_DMA_BUF_SIZE];

uint16_t DMA_GetWritePtr(void)
{
    return UART_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
}

uint32_t flash_addr = FLASH_START_ADDR;

void Process_UART_To_Flash(void)
{//             | data avaliable to pick up 0-1024
    uint16_t write_ptr = DMA_GetWritePtr();

    while (read_ptr != write_ptr)
    {
        uint16_t chunk;

        if (write_ptr > read_ptr)
        {
            chunk = write_ptr - read_ptr;
        }
        else
        {
            chunk = UART_DMA_BUF_SIZE - read_ptr;
        }

        // Limit chunk to safe write size
        if (chunk > 256)
            chunk = 256;

        // Write to flash
        W25Q_Write(flash_addr, &uart_dma_buf[read_ptr], chunk);

        flash_addr += chunk;

        read_ptr = (read_ptr + chunk) % UART_DMA_BUF_SIZE;

        write_ptr = DMA_GetWritePtr();  // update
    }
    char hexBuffer[5];
    uint16ToHexStr(flash_addr, hexBuffer);
    HAL_UART_Transmit(&huart1, (uint8_t*)hexBuffer, 4, 100);
}


uint8_t UART_ReadByte(void)
{
    uint8_t b;
    HAL_UART_Receive(&huart1, &b, 1, HAL_MAX_DELAY);
    return b;
}

/*uint32_t UART_ReadU32(void)
{
    uint32_t val = 0;

    val |= UART_ReadByte() << 24;
    val |= UART_ReadByte() << 16;
    val |= UART_ReadByte() << 8;
    val |= UART_ReadByte();

    return val;
}*/

#define FLASH_START_ADDR  0x000000
#define BUFFER_SIZE       256
uint8_t rx_buffer[BUFFER_SIZE];
void Flash_Write_From_UART(void) // final trigger.
{
    uint32_t size;
    uint32_t addr = FLASH_START_ADDR;
    uint32_t remaining;

    // 1. Wait for header
    if (UART_ReadByte() != 0xAA) return;
    if (UART_ReadByte() != 0x55) return;

    // 2. Read size
    size = UART_ReadU32();
    remaining = size;
    // 3. Erase required sectors
    uint32_t erase_addr = addr;
    uint32_t end_addr = addr + size;

    while (erase_addr < end_addr)
    {
        W25Q_SectorErase(erase_addr);
        erase_addr += 4096;
    }

    // 4. Receive + write loop
    while (remaining > 0)
    {
        uint16_t chunk = (remaining > BUFFER_SIZE) ? BUFFER_SIZE : remaining;

        HAL_UART_Receive(&huart1, rx_buffer, chunk, HAL_MAX_DELAY);

        W25Q_Write(addr, rx_buffer, chunk);

        addr += chunk;
        remaining -= chunk;
    }
}


void W25Q_SectorErase(uint32_t addr)
{
    W25Q_WriteEnable();

    CS_LOW();

    SPI_Transfer(0x20); // Sector erase
    SPI_Transfer(addr >> 16);
    SPI_Transfer(addr >> 8);
    SPI_Transfer(addr);

    CS_HIGH();

    W25Q_WaitBusy();
}

void W25Q_Init(void)
{
    CS_HIGH();
    CLK_LOW();
}

///////


void uint16ToHexStr(uint32_t address, char *str) {
    const char *hex_digits = "0123456789ABCDEF";

    // We only care about the lower 16 bits for a 4-character hex output
    uint16_t val = (uint16_t)(address & 0xFFFF);

    str[0] = hex_digits[(val >> 12) & 0x0F]; // Top 4 bits
    str[1] = hex_digits[(val >> 8) & 0x0F];  // Next 4 bits
    str[2] = hex_digits[(val >> 4) & 0x0F];  // Next 4 bits
    str[3] = hex_digits[val & 0x0F];         // Last 4 bits
    str[4] = '\0'; // Null terminator to end the string
}
