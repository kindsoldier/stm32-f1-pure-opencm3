
#ifndef LCD_ST7735R_H 
#define LCD_ST7735R_H


#define LCD_SWRESET  0x01    /* Software Reset */
#define LCD_RDDID    0x04    /* Read Display ID */
#define LCD_RDDST    0x09    /* Read Display Status */
#define LCD_RDDPM    0x0A    /* Read Display Power Mode */
#define LCD_RDDMADCTL        0x0B    /* Read Display MADCTL */
#define LCD_RDDCOLMOD        0x0C    /* Read Display Pixel Format */
#define LCD_RDDIM    0x0D    /* Read Display Image Mode */
#define LCD_RDDSM    0x0E    /* Read Display Signal Mode */
#define LCD_SLPIN    0x10    /* Sleep In */
#define LCD_SLPOUT   0x11    /* Sleep Out */
#define LCD_PTLON    0x12    /* Partial Display Mode On */
#define LCD_NORON    0x13    /* Normal Display Mode On */
#define LCD_INVOFF   0x20    /* Display Inversion Off */
#define LCD_INVON    0x21    /* Display Inversion On */
#define LCD_GAMSET   0x26    /* Gamma Set */
#define LCD_DISPOFF  0x28    /* Display Off */
#define LCD_DISPON   0x29    /* Display On */
#define LCD_CASET    0x2A    /* Column Address Set */
#define LCD_RASET    0x2B    /* Row Address Set */
#define LCD_RAMWR    0x2C    /* Memory Write */
#define LCD_RGBSET   0x2D    /* Color Setting for 4K, 65K and 262K */
#define LCD_RAMRD    0x2E    /* Memory Read */
#define LCD_PTLAR    0x30    /* Partial Area */
#define LCD_TEOFF    0x34    /* Tearing Effect Line OFF */
#define LCD_TEON     0x35    /* Tearing Effect Line ON */
#define LCD_MADCTL   0x36    /* Memory Data Access Control */
#define LCD_IDMOFF   0x38    /* Idle Mode Off */
#define LCD_IDMON    0x39    /* Idle Mode On */
#define LCD_COLMOD   0x3A    /* Interface Pixel Format */
#define LCD_RDID1    0xDA    /* Read ID1 Value */
#define LCD_RDID2    0xDB    /* Read ID2 Value */
#define LCD_RDID3    0xDC    /* Read ID3 Value */
#define LCD_FRMCTR1  0xB1    /* Frame Rate Control 0xIn normal mode/ Full colors) */
#define LCD_FRMCTR2  0xB2    /* Frame Rate Control (In Idle mode/ 8-colors) */
#define LCD_FRMCTR3  0xB3    /* Frame Rate Control 0xIn Partial mode/ full colors) */
#define LCD_INVCTR   0xB4    /* Display Inversion Control */
#define LCD_DISSET5  0xB6    /* Display Function set 5 */
#define LCD_PWCTR1   0xC0    /* Power Control 1 */
#define LCD_PWCTR2   0xC1    /* Power Control 2 */
#define LCD_PWCTR3   0xC2    /* Power Control 3 0xin Normal mode/ Full colors) */
#define LCD_PWCTR4   0xC3    /* Power Control 4 0xin Idle mode/ 8-colors) */
#define LCD_PWCTR5   0xC4    /* Power Control 5 0xin Partial mode/ full-colors) */
#define LCD_VMCTR1   0xC5    /* VCOM Control 1 */
#define LCD_VMOFCTR  0xC7    /* VCOM Offset Control */
#define LCD_WRID2    0xD1    /* Write ID2 Value */
#define LCD_WRID3    0xD2    /* Write ID3 Value */
#define LCD_NVFCTR1  0xD9    /* NVM Control Status */
#define LCD_NVFCTR2  0xDE    /* NVM Read Command */
#define LCD_NVFCTR3  0xDF    /* NVM Write Command */
#define LCD_GMCTRP1  0xE0    /* Gamma 0x`+'polarity) Correction Characteristics Setting */
#define LCD_GMCTRN1  0xE1    /* Gamma `-'polarity Correction Characteristics Setting */


#define LCD_TFTWIDTH  127 //127
#define LCD_TFTHEIGHT 159 //127

#define LCD_BLACK     0x0000
#define LCD_BLUE      0x001F
#define LCD_RED       0xF800
#define LCD_GREEN     0x07E0
#define LCD_CYAN      0x07FF
#define LCD_MAGENTA   0xF81F
#define LCD_YELLOW    0xFFE0
#define LCD_WHITE     0xFFFF


#define LCD_MADCTL_MY       0x80
#define LCD_MADCTL_MX       0x40
#define LCD_MADCTL_MV       0x20
#define LCD_MADCTL_ML       0x10
#define LCD_MADCTL_RGB      0x00
#define LCD_MADCTL_BGR      0x08
#define LCD_MADCTL_MH       0x04

typedef struct font {
    uint8_t width;
    uint8_t height;
    uint8_t start;
    uint8_t length;
    const uint8_t *bitmap;
} font_t;


//inline void _delay_ms(uint16_t ms);
//inline void spi_wait_busy(uint32_t spi);
//inline void lcd_write_byte(uint8_t c);
//inline void lcd_write_word(uint16_t w);
//inline void lcd_write_command(uint8_t c);


void lcd_spi_setup(void);


void spi_write_word_array(uint16_t w, uint16_t n);
void lcd_hard_reset(void);
void lcd_setup(void);
void lcd_write_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void lcd_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lcd_draw_pixel(uint8_t x, uint8_t y, uint16_t color);
void lcd_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void lcd_draw_vline(int16_t x, int16_t y, int16_t l, uint16_t color);
void lcd_draw_hline(int16_t x, int16_t y, int16_t l, uint16_t color);
void lcd_draw_rect(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t color);
uint16_t lcd_rgb2color(uint8_t r, uint8_t g, uint8_t b);
void lcd_orient0(void);
void lcd_clear(void);
void lcd_draw_char(uint16_t xbase, uint16_t ybase, font_t *font, uint8_t c);

#endif
