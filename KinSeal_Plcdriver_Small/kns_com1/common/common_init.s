
	GBLL    THUMBCODE


	IMPORT Enter_PlcDriver    ; The main entry of mon program 
	
	AREA    Iit,CODE,READONLY

	ENTRY 
    
	
    [ :LNOT:THUMBCODE
    	bl	Enter_PlcDriver        ;Don't use main() because ......
          ;  mov r0,lr
        mov r1,#0x00000000
        mov r2,#0x00450000
        add r1,r2,r1
        ldr r0,[r1] 
        mov pc,r0 
    ]
   
      END
