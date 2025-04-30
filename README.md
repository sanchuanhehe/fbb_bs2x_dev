# fbb_bs2x

## 介绍

  fbb_bs2x代码仓为支持bs21e解决方案SDK，该SDK包从统一开发平台FBB（Family Big Box，统一开发框架，统一API）构建而来，在该平台上开发的应用很容易被移植到其他星闪解决方案上，有效降低开发者门槛，缩短开发周期，支持开发者快速开发星闪产品。

## 支持的开发板

| 开发板型号                  | 购买链接                                                     | 硬件资料                                                     |
| --------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| HH-D03开发板                | [购买链接](https://e.tb.cn/h.6QTzoydYS0mUA5g?tk=aKlIV23zcP3) | [硬件原理图及HH-D03 星闪开发板规格说明书](docs/hardware/HH-D03/HH-D03_原理图_V01.pdf) |
| DK221星闪2K鼠标完整解决方案 | [购买链接](https://e.tb.cn/h.69IMdsPhGFQWDgS?tk=b25cVdob237) | 后续提供                                                     |

## 目录介绍

| 目录   | 介绍                                                 |
| ------ | ---------------------------------------------------- |
| docs   | 存放资料手册、IO复用关系表、硬件原理图、用户指南手册 |
| src    | SDK源码目录                                          |
| tools  | 开发工具及环境搭建指南                               |
| vendor | 存放对应开发板案例                                   |

## 快速上手

1. 开发环境搭建[参考tools目录README](tools/README.md)
2. 软件资料手册、IO复用关系、debug手册[参考docs/board目录README](docs/README.md)

## 示例教程

HH-D03开发板示例教程

| 一级分类     | 子分类          | 应用示例                                                     | 应用示例                                                  | 应用示例                                      |
| :----------- | --------------- | ------------------------------------------------------------ | --------------------------------------------------------- | --------------------------------------------- |
| **基础驱动** | **GPIO**        | [点亮LED灯案例](vendor/HH-D03/demo/led)                      | [按键检测](vendor/HH-D03/demo/buttondemo)                 | [交通灯案例](vendor/HH-D03/demo/trafficlight) |
|              | **UART**        | [串口通信案例](vendor/HH-D03/demo/uartdemo)                  |                                                           |                                               |
|              | **I2C**         | [0.96寸OLED屏幕驱动案例](vendor/HH-D03/demo/oled)            | [环境监测](vendor/HH-D03/demo/environment)                |                                               |
|              | **PWM**         | [蜂鸣器鸣响](vendor/HH-D03/demo/beep)                        | [RGB灯案例](vendor/HH-D03/demo/pwm_rgb_led)               |                                               |
|              | **watchdog**    | [看门狗案例](src/application/samples/peripheral/watchdog)    |                                                           |                                               |
|              | **i2s**         | [I2S通信案例](src/application/samples/peripheral/i2s)        | I[2S+DMA通信](src/application/samples/peripheral/i2s_dma) |                                               |
|              | **keyscan**     | [键盘扫描](src/application/samples/peripheral/keyscan)       |                                                           |                                               |
|              | **spi**         | [SPI通信案例](src/application/samples/peripheral/spi)        |                                                           |                                               |
|              | **timer**       | [定时器案例](src/application/samples/peripheral/timer)       |                                                           |                                               |
| **操作系统** | **Thread**      | [任务调度使用案例](vendor/HH-D03/demo/thread)                |                                                           |                                               |
|              | **Event**       | [事件使用案例](vendor/HH-D03/demo/event)                     |                                                           |                                               |
|              | **Mutex**       | [互斥锁使用案例](vendor/HH-D03/demo/mutex)                   |                                                           |                                               |
|              | **MessgeQueue** | [消息队列使用案例](vendor/HH-D03/demo/message)               |                                                           |                                               |
| **usb**      | **usb**         | [usb通信案例](vendor/HH-D03/demo/usb_demo)                   | [USB键盘](src/application/samples/products/usb_keyboard)  |                                               |
| **星闪**     | **SLE**         | [sle_uart串口透传](src/application/samples/products/sle_uart) | [SLE测距](vendor/HH-D03/demo/sle_measure_dis)             |                                               |
| **蓝牙**     | **BLE**         | [蓝牙串口透传](src/application/samples/products/ble_uart)    | [蓝牙键盘](src/application/samples/products/ble_keyboard) |                                               |

## 参与贡献

- 参考[社区参与贡献指南](https://gitee.com/HiSpark/docs/blob/master/contribute/%E7%A4%BE%E5%8C%BA%E5%8F%82%E4%B8%8E%E8%B4%A1%E7%8C%AE%E6%8C%87%E5%8D%97.md)
