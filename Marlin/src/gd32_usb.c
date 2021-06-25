
// #include <gd32f4xx.h>
#include "drv_usb_hw.h"
#include "drv_usb_core.h"
#include "gd32_usbd_core.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "cdc_acm_core.h"
#include "drv_usbh_int.h"
#include "drv_usbd_int.h"
// #include "rtt.h"
// #include "log.h"
#include "stm32yyxx_ll_pwr.h"   //use stm32 common rcc & nvic
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"

usbh_host usb_host_msc;     
usb_core_driver usbh_core;  //usb fs as host
usb_core_driver cdc_acm;    //usb hs as device
usb_cdc_handler *pcdc;


#include "PeripheralPins.h"
#include "pinmap.h"

void usb_fs_rcc_gpio_nvic_ll_init()
{
  const PinMap *map = NULL;
#if defined(PWR_CR2_USV)
  /* Enable VDDUSB on Pwrctrl CR2 register*/
#if !defined(STM32WBxx)
  if (__HAL_RCC_PWR_IS_CLK_DISABLED()) {
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddUSB();
    __HAL_RCC_PWR_CLK_DISABLE();
  } else
#endif
  {
    HAL_PWREx_EnableVddUSB();
  }
#endif

    /* 1. Configure USB FS GPIOs */
    map = PinMap_USB_OTG_FS;
    while (map->pin != NC) {
      pin_function(map->pin, map->function);
      map++;
    }

    /* Enable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

#if defined (USE_USB_INTERRUPT_REMAPPED)
    /*USB interrupt remapping enable */
    __HAL_REMAPINTERRUPT_USB_ENABLE();
#endif

    
}


const PinMap PinMap_USB_OTG_HS_langgo[] = {
  {PB_14, USB_OTG_HS, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF12_OTG_HS_FS)}, // USB_OTG_HS_DM
  {PB_15, USB_OTG_HS, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF12_OTG_HS_FS)}, // USB_OTG_HS_DP
  {NC,    NP,    0},  //must have
};

void usb_hs_rcc_gpio_nvic_ll_init()
{
    const PinMap *map = NULL;
    /* 2. Configure USB HS GPIOs */
    map = PinMap_USB_OTG_HS_langgo;
    while (map->pin != NC) {
        pin_function(map->pin, map->function);
        map++;
    }

    /* Enable USB HS Clock */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

    
}

void gd32_usb_device_cdc_init()
{
    usb_hs_rcc_gpio_nvic_ll_init();
    //usb device cdc on USB_HS
    usbd_init (&cdc_acm,
              USB_CORE_ENUM_HS,
              &cdc_desc,
              &cdc_class);
}

void gd32_usb_host_msc_init()
{
    usb_fs_rcc_gpio_nvic_ll_init();
    /* register device class */
    usbh_class_register(&usb_host_msc, &usbh_msc);

    //usb host msc on USB_FS
    usbh_init(&usb_host_msc,
              &usbh_core,
              USB_CORE_ENUM_FS,
              &usr_cb);

    HAL_NVIC_SetPriority(OTG_HS_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);

    /* Set USB FS Interrupt priority & enable it */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}


void gd32_usb_loop()
{
#if GD32_USB_ENABLE
    usbh_core_task(&usb_host_msc);

    if (USBD_CONFIGURED == cdc_acm.dev.cur_status) {
        if (0U == cdc_acm_check_ready(&cdc_acm)) {
            cdc_acm_data_receive(&cdc_acm);
        } else {
            pcdc = (usb_cdc_handler *)cdc_acm.dev.class_data[CDC_COM_INTERFACE];
            if (pcdc->receive_length) {
                // LOGI("[cdc] %d: %s", pcdc->receive_length, pcdc->data);
                // delay (2U);
            }
            cdc_acm_data_send(&cdc_acm);
        }
    }
#endif
}

// irq 
void OTG_FS_IRQHandler(void)
{
    usbh_isr(&usbh_core);
}

void OTG_HS_IRQHandler(void)
{
    usbd_isr(&cdc_acm);
}