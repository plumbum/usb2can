
# Self protect firmware

```c
/* Read protect the flash.  NEVER EVER set this to level 2.  You can't
 * write to the chip ever again after that. */
FLASH_OB_Unlock();
FLASH_OB_RDPConfig( OB_RDP_Level_1 );
if (FLASH_OB_Launch() != FLASH_COMPLETE)
{
    err_printf("Error enabling RDP\n");
}
FLASH_OB_Lock();
```

http://stackoverflow.com/questions/25770483/how-to-protect-reading-flash-of-stm32f10x
