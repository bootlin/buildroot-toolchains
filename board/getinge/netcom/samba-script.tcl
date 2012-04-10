DATAFLASH::Init 0
DATAFLASH::EraseAll
GENERIC::SendFile "output/images/dataflash_at91sam9m10g45ek.bin" 0x0 1
send_file {DataFlash AT45DB/DCB} "output/images/u-boot-env.bin" 0x2100 0
send_file {DataFlash AT45DB/DCB} "output/images/u-boot.bin" 0x4200 0
send_file {DataFlash AT45DB/DCB} "output/images/uboot.script.img" 0x37B00 0
send_file {DataFlash AT45DB/DCB} "output/images/u-boot-env.bin" 0x3DE00 0

after 200;
TCL_Write_Int $target(handle) 0xA5000000 0xFFFFFD08; # Set reset mode
after 200;
TCL_Write_Int $target(handle) 0xA500000D 0xFFFFFD00; # Reset!
after 2000;

