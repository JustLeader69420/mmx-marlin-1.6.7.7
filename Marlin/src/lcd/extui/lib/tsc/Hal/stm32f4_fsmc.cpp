#include "../../../../../inc/MarlinConfig.h"

#if defined(BTT_FSMC_LCD) // FSMC on 100/144 pins SoCs

#include "stm32f4_fsmc.h"
// #include <libmaple/fsmc.h>
// #include <libmaple/gpio.h>
// #include <libmaple/dma.h>
// #include <boards.h>

/**
 * FSMC LCD IO
 */
#define __ASM __asm
#define __STATIC_INLINE static inline

// __attribute__((always_inline)) __STATIC_INLINE void __DSB() {
//   __ASM volatile ("dsb 0xF":::"memory");
// }

// TFTLCD_TypeDef *TFTLCD;  // LCD FSMC Control Address

static uint8_t fsmcInit = 0;

void LCD_FSMCInit(uint8_t cs, uint8_t rs) {
  if (fsmcInit) return;
  fsmcInit = 1;

  TFT_FSMC::Init();

#if 0
  uint32_t controllerAddress;
  if (fsmcInit) return;
  fsmcInit = 1;

  switch (cs) {
    case FSMC_CS_NE1: controllerAddress = (uint32_t)FSMC_BANK1_1; break;
    #if ENABLED(STM32_XL_DENSITY)
      case FSMC_CS_NE2: controllerAddress = (uint32_t)FSMC_BANK1_2; break;
      case FSMC_CS_NE3: controllerAddress = (uint32_t)FSMC_BANK1_3; break;
      case FSMC_CS_NE4: controllerAddress = (uint32_t)FSMC_BANK1_4; break;
    #endif
    default: return;
  }

  #define _ORADDR(N) controllerAddress |= (_BV32(N) - 2)

  switch (rs) {
    #if ENABLED(STM32_XL_DENSITY)
      case FSMC_RS_A0:  _ORADDR( 1); break;
      case FSMC_RS_A1:  _ORADDR( 2); break;
      case FSMC_RS_A2:  _ORADDR( 3); break;
      case FSMC_RS_A3:  _ORADDR( 4); break;
      case FSMC_RS_A4:  _ORADDR( 5); break;
      case FSMC_RS_A5:  _ORADDR( 6); break;
      case FSMC_RS_A6:  _ORADDR( 7); break;
      case FSMC_RS_A7:  _ORADDR( 8); break;
      case FSMC_RS_A8:  _ORADDR( 9); break;
      case FSMC_RS_A9:  _ORADDR(10); break;
      case FSMC_RS_A10: _ORADDR(11); break;
      case FSMC_RS_A11: _ORADDR(12); break;
      case FSMC_RS_A12: _ORADDR(13); break;
      case FSMC_RS_A13: _ORADDR(14); break;
      case FSMC_RS_A14: _ORADDR(15); break;
      case FSMC_RS_A15: _ORADDR(16); break;
    #endif
    case FSMC_RS_A16: _ORADDR(17); break;
    case FSMC_RS_A17: _ORADDR(18); break;
    case FSMC_RS_A18: _ORADDR(19); break;
    case FSMC_RS_A19: _ORADDR(20); break;
    case FSMC_RS_A20: _ORADDR(21); break;
    case FSMC_RS_A21: _ORADDR(22); break;
    case FSMC_RS_A22: _ORADDR(23); break;
    case FSMC_RS_A23: _ORADDR(24); break;
    #if ENABLED(STM32_XL_DENSITY)
      case FSMC_RS_A24: _ORADDR(25); break;
      case FSMC_RS_A25: _ORADDR(26); break;
    #endif
    default: return;
  }

  rcc_clk_enable(RCC_FSMC);

  gpio_set_mode(GPIOD, 14, GPIO_AF_OUTPUT_PP);  // FSMC_D00
  gpio_set_mode(GPIOD, 15, GPIO_AF_OUTPUT_PP);  // FSMC_D01
  gpio_set_mode(GPIOD,  0, GPIO_AF_OUTPUT_PP);  // FSMC_D02
  gpio_set_mode(GPIOD,  1, GPIO_AF_OUTPUT_PP);  // FSMC_D03
  gpio_set_mode(GPIOE,  7, GPIO_AF_OUTPUT_PP);  // FSMC_D04
  gpio_set_mode(GPIOE,  8, GPIO_AF_OUTPUT_PP);  // FSMC_D05
  gpio_set_mode(GPIOE,  9, GPIO_AF_OUTPUT_PP);  // FSMC_D06
  gpio_set_mode(GPIOE, 10, GPIO_AF_OUTPUT_PP);  // FSMC_D07
  gpio_set_mode(GPIOE, 11, GPIO_AF_OUTPUT_PP);  // FSMC_D08
  gpio_set_mode(GPIOE, 12, GPIO_AF_OUTPUT_PP);  // FSMC_D09
  gpio_set_mode(GPIOE, 13, GPIO_AF_OUTPUT_PP);  // FSMC_D10
  gpio_set_mode(GPIOE, 14, GPIO_AF_OUTPUT_PP);  // FSMC_D11
  gpio_set_mode(GPIOE, 15, GPIO_AF_OUTPUT_PP);  // FSMC_D12
  gpio_set_mode(GPIOD,  8, GPIO_AF_OUTPUT_PP);  // FSMC_D13
  gpio_set_mode(GPIOD,  9, GPIO_AF_OUTPUT_PP);  // FSMC_D14
  gpio_set_mode(GPIOD, 10, GPIO_AF_OUTPUT_PP);  // FSMC_D15

  gpio_set_mode(GPIOD,  4, GPIO_AF_OUTPUT_PP);  // FSMC_NOE
  gpio_set_mode(GPIOD,  5, GPIO_AF_OUTPUT_PP);  // FSMC_NWE

  gpio_set_mode(PIN_MAP[cs].gpio_device, PIN_MAP[cs].gpio_bit, GPIO_AF_OUTPUT_PP);  //FSMC_CS_NEx
  gpio_set_mode(PIN_MAP[rs].gpio_device, PIN_MAP[rs].gpio_bit, GPIO_AF_OUTPUT_PP);  //FSMC_RS_Ax

  #if ENABLED(STM32_XL_DENSITY)
    FSMC_NOR_PSRAM4_BASE->BCR = FSMC_BCR_WREN | FSMC_BCR_MTYP_SRAM | FSMC_BCR_MWID_16BITS | FSMC_BCR_MBKEN;
    FSMC_NOR_PSRAM4_BASE->BTR = (FSMC_DATA_SETUP_TIME << 8) | FSMC_ADDRESS_SETUP_TIME;
  #else // PSRAM1 for STM32F103V (high density)
    FSMC_NOR_PSRAM1_BASE->BCR = FSMC_BCR_WREN | FSMC_BCR_MTYP_SRAM | FSMC_BCR_MWID_16BITS | FSMC_BCR_MBKEN;
    FSMC_NOR_PSRAM1_BASE->BTR = (FSMC_DATA_SETUP_TIME << 8) | FSMC_ADDRESS_SETUP_TIME;
  #endif

  afio_remap(AFIO_REMAP_FSMC_NADV);
  TFTLCD = (TFTLCD_TypeDef*)controllerAddress;
#endif
}

void LCD_WriteData(uint16_t data) {
  // TFT_FSMC::LCD->RAM = data;
  TFT_FSMC::WriteData(data);
  __DSB();
}

void LCD_WriteReg(uint16_t Reg) {
  // TFT_FSMC::LCD->REG = Reg;
  TFT_FSMC::WriteReg(Reg);
  __DSB();
}

uint16_t LCD_ReadData(void)
{
  // volatile uint16_t ram;
  // ram = TFT_FSMC::LCD->RAM;	
  // return ram;
}

void LCD_WriteMultiple(uint16_t color, uint32_t cnt)
{
  TFT_FSMC::WriteMultiple(color, cnt);
}

#endif // BTT_FSMC_LCD
