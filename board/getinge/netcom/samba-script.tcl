DATAFLASH::Init 0
DATAFLASH::EraseAll
GENERIC::SendFile "output/images/dataflash_at91sam9m10g45ek.bin" 0x0 1
send_file {DataFlash AT45DB/DCB} "output/images/u-boot.bin" 0x8400 0
