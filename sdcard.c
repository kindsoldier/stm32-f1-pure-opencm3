
/* Author, Copyright: Oleg Borodin <onborodin@gmail.com> 2018 */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include <stdlib.h>
#include <stdio.h>

#include <sdcard.h>

#define SD_CS_PORT GPIOA
#define SD_CS_PIN  GPIO4

#define SD_DI_PORT GPIOA
#define SD_DI_PIN  GPIO7

#define SD_DO_PORT GPIOA
#define SD_DO_PIN  GPIO6

#define SD_CLK_PORT GPIOA
#define SD_CLK_PIN  GPIO5

#define SD_SPI SPI1

void sd_spi_setup(void) {

    gpio_set_mode(SD_CS_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, SD_CS_PIN);
    gpio_set_mode(SD_DI_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, SD_DI_PIN);
    gpio_set_mode(SD_CLK_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, SD_CLK_PIN);

    gpio_set_mode(SD_DO_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, SD_DO_PIN);

    spi_reset(SD_SPI);
    spi_disable(SD_SPI);

    spi_set_unidirectional_mode(SD_SPI);
    spi_disable_crc(SD_SPI);
    spi_set_dff_8bit(SD_SPI);

    spi_set_nss_high(SD_SPI);
    spi_set_baudrate_prescaler(SD_SPI, SPI_CR1_BR_FPCLK_DIV_64);
    spi_set_master_mode(SD_SPI);
    spi_set_clock_polarity_0(SD_SPI);
    spi_set_clock_phase_0(SD_SPI);

    spi_enable_software_slave_management(SD_SPI);
    spi_disable_ss_output(SD_SPI);

    spi_enable(SD_SPI);

}


uint8_t sd_command(uint8_t cmd, uint32_t arg, uint8_t csum) {

#ifdef SD_DEBUG
    printf("\r\n<--- Send CMD%d 0x%08lX\r\n", cmd - 0x40, arg);
    delay(200);
#endif

    /* Dummy transfer */
    spi_xfer(SD_SPI, (uint16_t) 0xFF);

    spi_xfer(SD_SPI, (uint16_t) cmd);
    spi_xfer(SD_SPI, (uint16_t) ((arg >> 24) & 0xFF));
    spi_xfer(SD_SPI, (uint16_t) ((arg >> 16) & 0xFF));
    spi_xfer(SD_SPI, (uint16_t) ((arg >> 8) & 0xFF));
    spi_xfer(SD_SPI, (uint16_t) (arg & 0xFF));
    spi_xfer(SD_SPI, (uint16_t) csum);

    uint16_t i = 0;
    uint8_t r1 = 0xFF;

    while (((r1 = spi_xfer(SD_SPI, 0xFF)) & 0x80) && (i < 16)) {
        i++;
    }
#ifdef SD_DEBUG
    printf("---> R = 0x%02X\r\n", r1);
#endif
    return r1;
}


#define SD_CMD9_TRY 8
uint8_t sd_cmd9(uint8_t * R2) {

    uint8_t i = SD_CMD9_TRY;
    uint8_t R1;

    while (((R1 = sd_command(SD_CMD9, 0x00, 0xFF)) != 0x00) && (i > 0))
        --i;

    if (i == 0)
        return R1;

    i = SD_CMD9_TRY;
    while ((spi_xfer(SD_SPI, 0xFF) != 0xFE) && (i > 0))
        --i;

    if (i == 0)
        return 0xFF;

    for (uint8_t n = 0; n < 18; n++) {
        R2[n] = (uint8_t) (spi_xfer(SD_SPI, 0x00));
    }
    return R1;
}

#define SD_CMD10_TRY 8
uint8_t sd_cmd10(uint8_t * R2) {

    uint8_t i = SD_CMD10_TRY;
    uint8_t R1;

    while (((R1 = sd_command(SD_CMD10, 0x00, 0xFF)) != 0x00) && (i > 0))
        --i;

    if (i == 0)
        return R1;

    i = SD_CMD10_TRY;
    while ((spi_xfer(SD_SPI, 0xFF) != 0xFE) && (i > 0))
        --i;

    if (i == 0)
        return 0xFF;

    for (uint8_t n = 0; n < 18; n++) {
        R2[n] = (uint8_t) (spi_xfer(SD_SPI, 0x00));
    }
    return R1;
}


#define SD_READ_TRY 8
uint8_t sd_cmd17(uint32_t block, uint8_t * R2) {
    uint8_t i = SD_READ_TRY;
    uint8_t R1;

    while (((R1 = sd_command(SD_CMD17, block * SD_BLOCK_SIZE, 0xFF)) != 0x00) && (i > 0))
        --i;
    if (i == 0)
        return R1;

    i = SD_READ_TRY;
    while ((spi_xfer(SD_SPI, 0xFF) != 0xFE) && (i > 0))
        --i;

    if (i == 0)
        return R1;

    for (uint16_t n = 0; n < 514; n++) {
        R2[n] = (uint8_t) (spi_xfer(SD_SPI, 0x00));
    }

    return R1;
}

#define SD_CMD0_TRY 32
uint8_t sd_cmd0(void) {
    uint8_t i = 0;
    uint8_t R1;
    while (((R1 = sd_command(SD_CMD0, 0x00, 0x95)) != 0x01) && (i < SD_CMD0_TRY)) {
        i++;
    }
    return R1;
}

uint8_t sd_cmd8(uint8_t * R7) {
    uint8_t R;
    if ((R = sd_command(SD_CMD8, 0x000001AA, 0x87)) != 0x00)
        return R;

    for (uint8_t i = 0; i < 6; i++) {
        R7[i] = (uint8_t) (spi_xfer(SD_SPI, 0xFF) && 0xFF);
    }
    return R;
}

#define SD_ACMD41_TRY 64
uint8_t sd_acmd41(void) {
    uint8_t i = 0;
    uint8_t R1;
    do {
        R1 = sd_command(SD_CMD55, 0x00, 0xFF);
        R1 = sd_command(SD_CMD41, 0x40000000, 0xFF);
        i++;
    } while ((R1 != 0x00) && (i < SD_ACMD41_TRY));
    return R1;
}

uint8_t sd_cmd58(uint8_t * R3) {
    uint8_t R;
    uint8_t i;
    if ((R = sd_command(SD_CMD58, 0x00, 0xFD)) != 0x00)
        return R;

    for (i = 0; i < 4; i++) {
        R3[i] = spi_xfer(SD_SPI, 0xFF);
    }
    return R;
}

uint8_t sd_reset(void) {

    uint8_t R1;
    uint8_t data;
    uint16_t i;

    gpio_set(SD_CS_PORT, SD_CS_PIN);
    for (uint16_t i = 0; i < 20; i++) {
        spi_xfer(SD_SPI, 0xFF);
    }
    gpio_clear(SD_CS_PORT, SD_CS_PIN);
    if (sd_cmd0() != 0x01)
        return 1;

    uint8_t R7[6];

    if (sd_cmd8(R7) != 0x01)
        return 1;
    if (sd_acmd41() != 0x00)
        return 1;

    //uint8_t R3[5];
    //if (sd_cmd58(R3) != 0x00) return 1;

    return 0;
}

/* EOF */
