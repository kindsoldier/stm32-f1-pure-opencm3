
/* Author, Copyright: Oleg Borodin <onborodin@gmail.com> 2018 */

#ifndef DS3231_H_IUI
#define DS3231_H_IUI

#define DS3231_ADDR  0x68

#define I2C_READY    0
#define I2C_FAIL     1

#define DS_REGSEC       0x00
#define DS_REGMIN       0x01
#define DS_REGHOUR      0x02

#define DS_REGWDAY      0x03
#define DS_REGMDAY      0x04
#define DS_REGMONTH     0x05
#define DS_REGYEAR      0x06

#define DS_REGST        0x0E

#define DS_BIT_OSF     7
#define DS_BIT_24H     6

void ds_write(uint8_t reg, uint8_t data);
uint8_t ds_read(uint8_t reg);
volatile uint8_t ds_bcd2dec(uint8_t val);
uint8_t ds_dec2bcd(uint8_t val);
uint8_t ds_get_sec(void);
uint8_t ds_get_min(void);
uint8_t ds_get_hour(void);
uint8_t ds_get_wday(void);
uint8_t ds_get_mday(void);
uint8_t ds_get_mon(void);
uint16_t ds_get_year(void);
void ds_set_sec(uint8_t num);
void ds_set_min(uint8_t num);
void ds_set_hour(uint8_t num);
void ds_set_mday(uint8_t num);
void ds_set_mon(uint8_t num);
void ds_set_year(uint16_t num);
void ds_init(void);

void ds_get_tm(struct tm *date);

#endif

/* EOF */
