# Additional clean files

file(REMOVE_RECURSE
  "api_telegram_org.pem.S"
  "bootloader\\bootloader.bin"
  "bootloader\\bootloader.elf"
  "bootloader\\bootloader.map"
  "config\\sdkconfig.cmake"
  "config\\sdkconfig.h"
  "digi_cert_g2.pem.S"
  "esp-idf\\esptool_py\\flasher_args.json.in"
  "flash_app_args"
  "flash_bootloader_args"
  "flash_project_args"
  "flasher_args.json"
  "isrg_root_x1.pem.S"
  "ldgen_libraries"
  "ldgen_libraries.in"
  "project_elf_src_esp32.c"
  "village.bin"
  "village.map"
)
