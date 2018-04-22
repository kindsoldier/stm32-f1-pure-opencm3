
/* Author, Copyright: Oleg Borodin <onborodin@gmail.com> 2018 */

#ifndef SDCARD_H_XYZ
#define SDCARD_H_XYZ

/* Definitions for MMC/SDC command */

#define SD_CMD0    0x40         /* R1  GO_IDLE_STATE */
#define SD_CMD1    0x41         /* R1  SEND_OP_COND (MMC) */
#define SD_CMD2    0x42         /* -   NO */
#define SD_CMD3    0x43         /* -   NO */
#define SD_CMD4    0x44         /* -   NO */
#define SD_CMD5    0x45         /* -   RESERVED SDIO */
#define SD_CMD6    0x46         /* R1  SWITCH_FUNC */
#define SD_CMD7    0x47         /* -   NO */
#define SD_CMD8    0x48         /* R7  SEND_IF_COND */
#define SD_CMD9    0x49         /* R1  SEND_CSD */
#define SD_CMD10   0x4A         /* R1  SEND_CID */

#define SD_CMD11   0x4B         /* -   NO */
#define SD_CMD12   0x4C         /* R1b STOP_TRANSMISSION */
#define SD_CMD13   0x4D         /* R2  SEND_STATUS */
#define SD_CMD14   0x4E         /* -   RESERVED */
#define SD_CMD15   0x4F         /* -   NO */
#define SD_CMD16   0x50         /* R1  SET_BLOCKLEN */
#define SD_CMD17   0x51         /* R1  READ_SINGLE_BLOCK */
#define SD_CMD18   0x52         /* R1  READ_MULTIPLE_BLOCK  */
#define SD_CMD19   0x53         /* -   RESERVED */
#define SD_CMD20   0x54         /* -   NO */

#define SD_CMD21   0x55         /* -   RESERVED */
#define SD_CMD22   0x56         /* -   RESERVED */
#define SD_CMD23   0x57         /* -   RESERVED */
#define SD_CMD24   0x58         /* R1  WRITE_BLOCK */
#define SD_CMD25   0x59         /* R1  WRITE_MULTIPLE_BLOCK */
#define SD_CMD26   0x5A         /* -   NO */
#define SD_CMD27   0x5B         /* R1  PROGRAM_CSD */
#define SD_CMD28   0x5C         /* R1b SET_WRITE_PROT !SDHC and !SDXC */
#define SD_CMD29   0x5D         /* R1b CLR_WRITE_PROT !SDHC and !SDXC */
#define SD_CMD30   0x5E         /* R1  SEND_WRITE_PROT !SDHC and !SDXC */

#define SD_CMD31   0x5F         /* -   RESERVED */
#define SD_CMD32   0x60         /* R1  ERASE_WR_BLK_START_ADDR */
#define SD_CMD33   0x61         /* R1  ERASE_WR_BLK_END_ADDR */
#define SD_CMD34   0x62         /* -   RESERVED */
#define SD_CMD35   0x63         /* -   RESERVED */
#define SD_CMD36   0x64         /* -   RESERVED */
#define SD_CMD37   0x65         /* -   RESERVED */
#define SD_CMD38   0x66         /* R1b ERASE */
#define SD_CMD39   0x67         /* -   NO */
#define SD_CMD40   0x68         /* -   NO */
#define SD_CMD41   0x69         /* -   RESERVED */
#define SD_CMD42   0x6A         /* R1  LOCK_UNLOCK */
#define SD_CMD43   0x6B         /* -   RESERVED */
#define SD_CMD44   0x6C         /* -   RESERVED */
#define SD_CMD45   0x6D         /* -   RESERVED */
#define SD_CMD46   0x6E         /* -   RESERVED */
#define SD_CMD47   0x6F         /* -   RESERVED */
#define SD_CMD48   0x70         /* -   RESERVED */
#define SD_CMD49   0x71         /* -   RESERVED */
#define SD_CMD50   0x72         /* -   RESERVED */
#define SD_CMD51   0x73         /* -   RESERVED */
#define SD_CMD52   0x74         /* -   RESERVED */
#define SD_CMD53   0x75         /* -   RESERVED */
#define SD_CMD54   0x76         /* -   RESERVED */
#define SD_CMD55   0x77         /* R1  APP_CMD */
#define SD_CMD56   0x78         /* R1  GEN_CMD */
#define SD_CMD57   0x79         /* -   RESERVED CMD6 */
#define SD_CMD58   0x7A         /* R3  READ_OCR */
#define SD_CMD59   0x7B         /* R1  CRC_ON_OFF */


#define R1_IDLE_STATE     (1 << 0)
#define R1_ERASE_RESET    (1 << 1)
#define R1_ILLEGAL_COM    (1 << 2)
#define R1_CON_CRC_ERR    (1 << 3)
#define R1_ERASE_SEQ_ERR  (1 << 4)
#define R1_ADDR_ERR       (1 << 5)
#define R1_PARAM_ERR      (1 << 6)

#define SD_BLOCK_SIZE           512
#define SD_START_BLOCK_TOKEN    0xFE
#define SD_START_BLOCK_IND      0xFE


void sd_spi_setup(void);
uint8_t sd_command(uint8_t cmd, uint32_t arg, uint8_t csum);
uint8_t sd_cmd9(uint8_t *R2);
uint8_t sd_cmd10(uint8_t *R2);
uint8_t sd_cmd17(uint32_t block, uint8_t *R2);
uint8_t sd_cmd0(void);
uint8_t sd_cmd8(uint8_t *R7);
uint8_t sd_acmd41(void);
uint8_t sd_cmd58(uint8_t *R3);
uint8_t sd_reset(void);

#endif

/* EOF */
