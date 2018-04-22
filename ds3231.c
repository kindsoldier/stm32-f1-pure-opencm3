
/* Author, Copyright: Oleg Borodin <onborodin@gmail.com> 2018 */

#include <libopencm3/stm32/i2c.h>

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include <ds3231.h>

void ds_write(uint8_t reg, uint8_t data) {
    uint8_t d[2] = { reg, data };
    i2c_transfer7(I2C1, DS3231_ADDR, d, 2, NULL, 0);
}

uint8_t ds_read(uint8_t reg) {
    uint8_t data;
    i2c_transfer7(I2C1, DS3231_ADDR, &reg, 1, &data, 1);
    return data;
}

volatile uint8_t ds_bcd2dec(uint8_t val) {
    return ((val & 0x0F) + (val >> 4) * 10);
}

uint8_t ds_dec2bcd(uint8_t val) {
    return (((val / 10) << 4) | ((val % 10) & 0x0F));
}

#define ds_clear_hi_bit(r) ((r) &= ~(1 << 7))

/* Get time functions */

/* Get sec */
uint8_t ds_get_sec(void) {
    uint8_t r = ds_read(DS_REGSEC);
    ds_clear_hi_bit(r);
    return ds_bcd2dec(r);
}

/* Get min */
uint8_t ds_get_min(void) {
    uint8_t r = ds_read(DS_REGMIN);
    ds_clear_hi_bit(r);
    return ds_bcd2dec(r);

}

/* Get hour */
uint8_t ds_get_hour(void) {
    uint8_t r = ds_read(DS_REGHOUR);
    ds_clear_hi_bit(r);
    /* Only 24h mode */
    r &= ~(1 << 6);
    return ds_bcd2dec(r);
}

/* Get date functions */

/* Get week day */
uint8_t ds_get_wday(void) {
    uint8_t r = ds_read(DS_REGWDAY);
    return r;
}

/* Get day */
uint8_t ds_get_mday(void) {
    uint8_t r = ds_read(DS_REGMDAY);
    return ds_bcd2dec(r);

}

/* Get month */
uint8_t ds_get_mon(void) {
    uint8_t r = ds_read(DS_REGMONTH);
    /* Clear century bit */
    ds_clear_hi_bit(r);
    return ds_bcd2dec(r);

}

/* Get year */
uint16_t ds_get_year(void) {
    /* Get century bit */
    uint8_t cent = ds_read(DS_REGMONTH);
    cent &= (1 << 7);
    /* Get year low digits */
    uint8_t r = ds_read(DS_REGYEAR);
    r = ds_bcd2dec(r);
    /* Add century */
    return cent ? r + 2000 : r + 1900;
}

/* Time set functions */

/* Set sec */
void ds_set_sec(uint8_t num) {
    if (num > 59)
        return;
    ds_write(DS_REGSEC, ds_dec2bcd(num));
}

/*  Set min */
void ds_set_min(uint8_t num) {
    if (num > 59)
        return;
    ds_write(DS_REGMIN, ds_dec2bcd(num));
}

/* Set hour */
void ds_set_hour(uint8_t num) {
    if (num > 23)
        return;
    ds_write(DS_REGHOUR, ds_dec2bcd(num));
}

/* Date set functions */

/* Set day */
void ds_set_mday(uint8_t num) {
    if (num > 31)
        return;
    ds_write(DS_REGMDAY, ds_dec2bcd(num));
}

/* Set month  */
void ds_set_mon(uint8_t num) {
    if (num > 12)
        return;
    /* Read century bit */
    uint8_t cent = ds_read(DS_REGMONTH);
    cent &= (1 << 7);
    /* Pack month with century bit */
    ds_write(DS_REGMONTH, ds_dec2bcd(num) | cent);
}

/* Set year */
void ds_set_year(uint16_t num) {
    if (num < 1900 || num > 2099)
        return;
    /* Get month byte */
    uint8_t month = ds_read(DS_REGMONTH);
    /* Set century bit to month byte */
    if (num < 2000) {
        month &= ~(1 << 7);
        num = num - 1900;
    } else {
        month |= (1 << 7);
        num = num - 2000;
    }
    /* Write month byte */
    ds_write(DS_REGMONTH, month);
    ds_write(DS_REGYEAR, ds_dec2bcd(num));
}

/* Init function, must use before any interrupt start */
void ds_init(void) {
    /* Start oscillator */
    uint8_t r;
    r = ds_read(DS_REGST);
    r &= ~(1 << DS_BIT_OSF);
    ds_write(DS_REGST, r);

    /* Set 24h format */
    r = ds_read(DS_REGHOUR);
    r &= ~(1 << 6);
    ds_write(DS_REGHOUR, r);
}


struct _tm {
    int tm_sec;                 /* seconds (0 - 60) */
    int tm_min;                 /* minutes (0 - 59) */
    int tm_hour;                /* hours (0 - 23) */
    int tm_mday;                /* day of month (1 - 31) */
    int tm_mon;                 /* month of year (0 - 11) */
    int tm_year;                /* year - 1900 */
    int tm_wday;                /* day of week (Sunday = 0) */
    int tm_yday;                /* day of year (0 - 365) */
    int tm_isdst;               /* is summer time in effect? */
    char *tm_zone;              /* abbreviation of timezone name */
    long tm_gmtoff;             /* offset from UTC in seconds */

};

void ds_get_tm(struct tm *date) {
    date->tm_sec = ds_get_sec();
    date->tm_min = ds_get_min();
    date->tm_hour = ds_get_hour();
    date->tm_mday = ds_get_mday();
    date->tm_mon = ds_get_mon();
    date->tm_year = ds_get_year(); 
    date->tm_wday = ds_get_wday();
    date->tm_yday = 0;
    date->tm_isdst = 0;
}

/* EOF */
