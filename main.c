#include "stm32f10x.h"
#include <string.h>

void USART1_Init(void);
void USART1_SendString(const char *str);
void GPIO_LED_Init(void);
void USART1_SendChar(char c);
char USART1_ReadChar(void);

int main(void) {
    SystemInit();
    GPIO_LED_Init();
    USART1_Init();

    USART1_SendString("Hello from STM32!\r\n");

    char buffer[10];
    uint8_t idx = 0;

    while (1) {
        char c = USART1_ReadChar(); 
        USART1_SendChar(c);           

        // Luu vao buffer
        if (c == '\r' || c == '\n') {  // Ket thúc chuoi khi xuong dòng
            buffer[idx] = '\0';
            idx = 0;

            if (strcmp(buffer, "ON") == 0) {
                GPIO_ResetBits(GPIOC, GPIO_Pin_13); // LED sang (PC13 LOW)
                USART1_SendString("\r\nLED ON\r\n");
            } 
            else if (strcmp(buffer, "OFF") == 0) {
                GPIO_SetBits(GPIOC, GPIO_Pin_13);   // LED tat
                USART1_SendString("\r\nLED OFF\r\n");
            }
        } else if (idx < sizeof(buffer) - 1) {
            buffer[idx++] = c;
        }
    }
}

// ---------- Ham cau hinh ----------
void GPIO_LED_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio);
    GPIO_SetBits(GPIOC, GPIO_Pin_13); // LED tat mac dinh
}

void USART1_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef gpio;
    // TX (PA9)
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    // RX (PA10)
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    USART_InitTypeDef usart;
    usart.USART_BaudRate = 115200;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}

// ---------- Ham UART co ban ----------
void USART1_SendChar(char c) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}
void USART1_SendString(const char *str) {
    while (*str) {
        USART1_SendChar(*str++);
    }
}
char USART1_ReadChar(void) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART1);
}
