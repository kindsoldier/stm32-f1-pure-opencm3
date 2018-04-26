
/* Author, Copyright: Oleg Borodin <onborodin@gmail.com> 2018 */

#include <libopencm3/cm3/nvic.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/systick.h>


#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/msc.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wctype.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>

#include <syscall.h>

#include <buffer.h>
#include <uastdio.h>

#include <ds3231.h>
#include <datetime.h>

#include <st7735.h>
#include <console.h>

#include <i2creg.h>
#include <mpu6050.h>

mpu_t mpu = {
    .i2c = I2C1,
    .addr = 0x68,
    .q0 = 1.0,
    .q1 = 0.0,
    .q2 = 0.0,
    .q3 = 0.0,
    .integralFBx = 0.0,
    .integralFBy = 0.0,
    .integralFBz = 0.0
};


void delay(uint32_t n) {
    for (volatile int i = 0; i < n; i++)
        __asm__("nop");
}

static void clock_setup(void) {
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_clock_enable(RCC_TIM3);

    rcc_periph_clock_enable(RCC_AFIO);

    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_I2C1);

    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_clock_enable(RCC_SPI2);

    rcc_periph_clock_enable(RCC_ADC1);
    rcc_periph_clock_enable(RCC_DMA1);

}


void uart_setup(void) {

    usart_disable(USART1);
    nvic_enable_irq(NVIC_USART1_IRQ);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_enable_rx_interrupt(USART1);

    usart_enable(USART1);
}

inline bool usart_recv_is_ready(uint32_t usart) {
    return (USART_SR(usart) & USART_SR_RXNE);
}

inline bool usart_rx_int_is_enable(uint32_t usart) {
    return (USART_CR1(USART1) & USART_CR1_RXNEIE);
}

void usart1_isr(void) {
    static uint8_t data = 0;

    if (usart_rx_int_is_enable(USART1) && usart_recv_is_ready(USART1)) {

        data = usart_recv(USART1);
        buffer_put_byte(&stdin_buffer, data);
        buffer_put_byte(&stdout_buffer, data);
        if (data == '\r')
            buffer_put_byte(&stdout_buffer, '\n');
    }
}

static void tim2_setup(void) {
    rcc_periph_clock_enable(RCC_TIM2);

    nvic_enable_irq(NVIC_TIM2_IRQ);
    rcc_periph_reset_pulse(RST_TIM2);

    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_prescaler(TIM2, 31);

    timer_disable_preload(TIM2);
    timer_continuous_mode(TIM2);
    timer_set_period(TIM2, 63);

    timer_set_oc_value(TIM2, TIM_OC1, 1);
    timer_enable_counter(TIM2);

    timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

void tim2_isr(void) {

    uint8_t c;

    if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
        timer_clear_flag(TIM2, TIM_SR_CC1IF);
        while ((c = buffer_get_byte(&stdout_buffer)) > 0) {
            usart_send_blocking(USART1, c);
        }
    }
}

void tim3_setup(void) {
    nvic_enable_irq(NVIC_TIM3_IRQ);
    timer_reset(TIM3);
    timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

    timer_direction_up(TIM3);
    timer_set_prescaler(TIM3, 50);
    timer_disable_preload(TIM3);
    timer_continuous_mode(TIM3);
    timer_set_period(TIM3, 28200);
    timer_set_oc_value(TIM3, TIM_OC1, 1);
    timer_enable_irq(TIM3, TIM_DIER_CC1IE);

    timer_enable_counter(TIM3);
}

void tim3_isr(void) {
    if (timer_get_flag(TIM3, TIM_SR_CC1IF)) {
        timer_clear_flag(TIM3, TIM_SR_CC1IF);
        mpu_update_quaternion(&mpu);
    }
}


void rtc_setup() {
    rtc_auto_awake(RCC_LSE, 0x7FFF);
    nvic_enable_irq(NVIC_RTC_IRQ);
    nvic_set_priority(NVIC_RTC_IRQ, 1);
    rtc_interrupt_enable(RTC_SEC);
}

uint32_t loop_counter = 0;
uint32_t rate_value = 0;


void rtc_isr(void) {
    rtc_clear_flag(RTC_SEC);
    rate_value = loop_counter;
    loop_counter = 0;
}

volatile static uint32_t systick_counter_ms = 0;

static void systick_setup(void) {

    nvic_enable_irq(NVIC_SYSTICK_IRQ);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
    systick_set_reload(9000 - 1);
    systick_interrupt_enable();
    systick_counter_enable();
}

void sys_tick_handler(void) {
    systick_counter_ms++;
}

void delay_ms(uint32_t t) {
    systick_counter_ms = 0;
    while (systick_counter_ms < t);
}


volatile uint16_t adc_res[16];

void dma_setup(void) {

    dma_disable_channel(DMA1, DMA_CHANNEL1);

    dma_enable_circular_mode(DMA1, DMA_CHANNEL1);
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);

    dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
    dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);

    dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
    dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t) & ADC_DR(ADC1));

    dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) & adc_res);
    dma_set_number_of_data(DMA1, DMA_CHANNEL1, 2);

    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);
    dma_enable_channel(DMA1, DMA_CHANNEL1);

}

void adc_dma_setup(void) {

    uint8_t channel_seq[16];

    adc_power_off(ADC1);

    adc_enable_scan_mode(ADC1);
    adc_set_continuous_conversion_mode(ADC1);
    adc_disable_discontinuous_mode_regular(ADC1);

    adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
    adc_set_right_aligned(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);

    adc_power_on(ADC1);

    delay(10);

    adc_reset_calibration(ADC1);
    adc_calibrate(ADC1);

    adc_enable_temperature_sensor();

    channel_seq[0] = 16;
    channel_seq[1] = 17;

    adc_set_regular_sequence(ADC1, 2, channel_seq);

    adc_enable_dma(ADC1);
    delay(100);

    adc_start_conversion_regular(ADC1);
    nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);

}

void dma1_channel1_isr(void) {
    dma_clear_interrupt_flags(DMA1, DMA_CHANNEL1, DMA_IFCR_CGIF1);
}

void nvic_setup(void) {
    //nvic_enable_irq(NVIC_HARD_FAULT_IRQ);
}

int16_t get_mcu_temp(void) {
    float V_25 = 1.45;
    float Slope = 4.3e-3;
    float Vref = 1.78;
    float V_sense = adc_res[0] / 4096.0 * Vref;
    float temp = (V_25 - V_sense) / Slope + 25.0;
    return (int16_t) temp;
}

void demo_gpio_setup(void) {
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO6);
    gpio_clear(GPIOC, GPIO6);
}

int main(void) {
    clock_setup();
    io_setup();
    uart_setup();
    rtc_setup();
    systick_setup();

    //nvic_setup();
    dma_setup();
    adc_dma_setup();

    uint32_t i = 0;

    mpu_i2c_setup();
    mpu_setup(&mpu);

    lcd_spi_setup();
    console_setup();

    lcd_setup();
    lcd_clear();

    delay_ms(10);

    tim2_setup();
    tim3_setup();

    console_puts(&console, "STM32 CONSOLE V0.1\n");
    console_puts(&console, "READY>");

    while (1) {
        #define STR_LEN 12

        uint8_t str[STR_LEN + 1];

        snprintf(str, STR_LEN, "Rat %4d", rate_value);
        console_xyputs(&console, 2, 0, str);

        snprintf(str, STR_LEN, "Tmc %4d", get_mcu_temp());
        console_xyputs(&console, 2, 9, str);

        #if 0
        uint8_t rdata = i2c_read_reg(I2C1, 0x68, 0x75);
        snprintf(str, STR_LEN, "R %02X", rdata);
        console_xyputs(&console, 4, 0, str);
        #endif

        #if 0
        int16_t ax, ay, az, gx, gy, gz;
        mpu_get_raw_data(&mpu, &ax, &ay, &az, &gx, &gy, &gz);

        snprintf(str, STR_LEN, "%8ld", ax);
        console_xyputs(&console, 4, 0, str);

        snprintf(str, STR_LEN, "%8ld", ay);
        console_xyputs(&console, 5, 0, str);
        #endif

        #if 0
        double gxc, gyc, gzc, axc, ayc, azc;
        mpu_get_conv_data(&mpu, &axc, &ayc, &azc, &gxc, &gyc, &gzc);

        snprintf(str, STR_LEN, "%8ld", (int32_t)(axc * 100.0f));
        console_xyputs(&console, 4, 0, str);

        snprintf(str, STR_LEN, "%8ld", (int32_t)(ayc * 100.0f));
        console_xyputs(&console, 5, 0, str);
        #endif

        #if 1
        double roll, pitch, yaw;
        mpu_get_roll_pitch_yaw(&mpu, &roll, &pitch, &yaw);

        snprintf(str, STR_LEN, "R %6ld", (int32_t)(roll * 62.5));
        console_xyputs(&console, 3, 0, str);

        snprintf(str, STR_LEN, "P %6ld", (int32_t)(pitch * 62.5));
        console_xyputs(&console, 4, 0, str);
        #endif

        snprintf(str, STR_LEN, "0x%08X", i);
        console_xyputs(&console, 8, 0, str);

        uint16_t y_value = i % (159 - 10);
        uint16_t y_prev;

        uint16_t x_value = i % (127 - 10);
        uint16_t x_prev;

        if ((x_prev != x_value) && (y_prev != y_value)) {
            lcd_write_rect(20, y_prev, 10, 10, 0x0F00);
            lcd_write_rect(20, y_value, 10, 10, 0x1234);
        }
        y_prev = y_value;
        x_prev = x_value;

        delay(1000);
        loop_counter++;

        i++;
    }

    return 0;
}

/* EOF */
