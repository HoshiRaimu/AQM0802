/*
 * File:   main.c
 * Author: raimu
 *
 * Created on 2021/03/15, 21:34
 */

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#include <xc.h>
#include <stdint.h>

#define I2C_ACK 0x00
#define I2C_NACK 0xff

void i2cProtocolStart();
void i2cProtocolStop();
void i2cProtocolSendData(uint8_t);
uint8_t i2cProtocolCheckAck();

void main(void) {
    //PICマイコンの設定
    OSCCON = 0b01101000;
    ANSELA = 0b00000000;
    TRISA  = 0b00000110;     //RA1とRA2を入力に設定
    
    //I2Cの設定
    SSP1STAT = 0x80;   // クロック信号は100kHzを使用
    SSP1CON1 = 0x28;   // I2C通信のマスターモードを有効化
    SSP1CON3 = 0x00;   // CON3はデフォルト設定
    SSP1ADD  = 0x09;   //クロック信号速度を100kHzに設定
    return;
}

void i2cProtocolStart() {
    // SSP1CON2レジスタのSENビットを1に設定すると
    // スタートコンディションが生成される
    // 発行が完了するとSSP1IFが1になるのでwhile文で待つ
    
    SSP1IF = 0;
    SSP1CON2bits.SEN = 1;
    while(SSP1IF == 0) {};
    SSP1IF = 0;
    
    return;
}

void i2cProtocolStop() {
    // SSP1CON2レジスタのPENビットを1に設定すると
    // ストップコンディションが生成される
    // 発行が完了するとSSP1IFが1になるのでwhile文で待つ
    
    SSP1IF = 0;
    SSP1CON2bits.PEN = 1;
    while(SSP1IF == 0) {};
    SSP1IF = 0;
    
    return;
}

void i2cProtocolSendData(uint8_t data) {
    // SSP1BUFに送信したいデータをセットすると、そのデータが送信される
    // 発行が完了するとSSP1IFが1になるのでwhile文で待つ
    
    SSP1IF = 0;
    SSP1BUF = data;
    while(SSP1IF == 0) {};
    SSP1IF = 0;
    
    return;
}

uint8_t i2cProtocolCheckAck() {
    uint8_t ack_status;
    
    if(SSP1CON2bits.ACKSTAT) {
        ack_status = I2C_NACK;
    }else {
        ack_status = I2C_ACK;
    }
    
    return ack_status;
}