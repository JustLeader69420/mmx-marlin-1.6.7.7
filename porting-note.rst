#####################
porting note.
#####################


todo list
~~~~~~~~~~~~~~~~~~~~~

1. gpio led                 pass
#. gpio key xmin ymin zmin  pass
#. uart                     pass
#. stepper                  pass @ 1mm 10mm move test.
#. spi flash w25qxx         
#. spi sd card reader       pass
#. touch screen xpt2042     pass
#. fsmc lcd display.        pass. use other ui so far.
#. dma 
#. timer pwm buzzer
#. temp sensor              fix max/min temp bug.
#. runout sensor


gpio. f4
~~~~~~~~~~~~~~~~~~~~~


compatible with st parts:
    verified: gpio R/W


use EXTUI-example so far
~~~~~~~~~~~~~~~~~~~~~

123123
~~~~~~~~~~~~~~~~~~~~~


如何通过串口打印debug信息？？
~~~~~~~~~~~~~~~~~~~~~

也许是通过serial == host 之间这个来输出？


code excute sequence
###############################

main -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\cores\arduino\main.cpp
serialx.begin -- Marlin\src\HAL\STM32\MarlinSerial.h
hw-serial.begin -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\cores\arduino\HardwareSerial.cpp
uart_init -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\libraries\SrcWrapper\src\stm32\uart.c
HAL_Init setTimerIntPrio -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\libraries\SoftwareSerial\src\SoftwareSerial.cpp
tmc_stepper_swserial -- .pio\libdeps\langgo407ve\TMCStepper\src\source\TMC2208Stepper.cpp   setup param for init purpose.
sw_serial -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\libraries\SoftwareSerial\src\SoftwareSerial.cpp


stepper timer tim5
-------------------------------
tim cfg ARR = 2000. psc = 42,  timck = 84mhz
it freq = 84mhz / 42  = 2mhz  / 2k = 1khz


fsmc tft driver 
-------------------------------
1. fsmc rs cs backlight other data pins define & init.
#. fsmc clock init.
#. lcd rw via fsmc like lcd->ram = xxxx;
#. 今天调试时  相同代码情况下断点调试有部分内存会忽然变0的问题 断电重启后不复现  国产MCU 有时真坑。。。