@echo off
cd /D E:\PlatformIO\zoo\village\.pio\build\esp32dev\esp-idf\esp_system || (set FAIL_LINE=2& goto :ABORT)
C:\Users\olmor\.platformio\penv\.espidf-5.3.1\Scripts\python.exe C:/Users/olmor/.platformio/packages/framework-espidf/tools/ldgen/ldgen.py --config E:/PlatformIO/zoo/village/sdkconfig.esp32dev --fragments-list C:/Users/olmor/.platformio/packages/framework-espidf/components/xtensa/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_gpio/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_pm/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_mm/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/spi_flash/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_system/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_system/app.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_common/common.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_common/soc.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_rom/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/hal/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/log/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/heap/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/soc/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_hw_support/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_hw_support/dma/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_hw_support/ldo/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/freertos/linker_common.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/freertos/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/newlib/newlib.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/newlib/system_libs.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_gptimer/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_ringbuf/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_uart/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/app_trace/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_event/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_pcnt/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_spi/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_mcpwm/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_ana_cmpr/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_dac/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_rmt/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_sdm/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_i2c/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_driver_ledc/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/driver/twai/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_phy/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/vfs/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/lwip/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_netif/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/wpa_supplicant/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_wifi/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_adc/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_eth/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_gdbstub/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_psram/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/esp_lcd/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/espcoredump/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/ieee802154/linker.lf;C:/Users/olmor/.platformio/packages/framework-espidf/components/openthread/linker.lf --input E:/PlatformIO/zoo/village/.pio/build/esp32dev/esp-idf/esp_system/ld/sections.ld.in --output E:/PlatformIO/zoo/village/.pio/build/esp32dev/esp-idf/esp_system/ld/sections.ld --kconfig C:/Users/olmor/.platformio/packages/framework-espidf/Kconfig --env-file E:/PlatformIO/zoo/village/.pio/build/esp32dev/config.env --libraries-file E:/PlatformIO/zoo/village/.pio/build/esp32dev/ldgen_libraries --objdump C:/Users/olmor/.platformio/packages/toolchain-xtensa-esp-elf/bin/xtensa-esp32-elf-objdump.exe || (set FAIL_LINE=3& goto :ABORT)
goto :EOF

:ABORT
set ERROR_CODE=%ERRORLEVEL%
echo Batch file failed at line %FAIL_LINE% with errorcode %ERRORLEVEL%
exit /b %ERROR_CODE%