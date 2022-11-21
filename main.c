/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/version.h"

 
#define RAM_STACK_BOTTOM_ADDR   (0x20042000u)
#define RAM_STACK_SIZE          (0x800u)

#define text_section_start(_type)\
({\
	static char start[0] __aligned(4)\
	__attribute__((unused)) \
    __section(".text");\
	(_type)&start;\
})


extern unsigned int __text_start__;
extern unsigned int __text_end__;

void HardFault_Handler(unsigned int * hardfault_args,unsigned int lr_value)
{
#define BL_INS_MASK     (0xF800u)
#define BL_INS_HIGH     (0xF800u)
#define BL_INS_LOW      (0xF000u)
#define BLX_INS_MASK    (0xFF00u)
#define BLX_INX         (0x4700u)

    unsigned int *stack_addr = NULL;
    int i = 0;
    unsigned int text_start = 0;
    unsigned int text_end = 0;
    unsigned int stack_value = 0;
    unsigned short insh = 0;
    unsigned short insl = 0;
    unsigned int print_cnt = 0;
    unsigned int stack_r0 = 0;
    unsigned int stack_r1 = 0;
    unsigned int stack_r2 = 0;
    unsigned int stack_r3 = 0;
    unsigned int stack_r4 = 0;
    unsigned int stack_r5 = 0;
    unsigned int stack_r6 = 0;
    unsigned int stack_r7 = 0;
    unsigned int stack_r8 = 0;
    unsigned int stack_r9 = 0;
    unsigned int stack_r10 = 0;
    unsigned int stack_r11 = 0;
    unsigned int stack_r12 = 0;
    unsigned int stack_lr = 0;
    unsigned int stack_pc = 0;
    unsigned int stack_psr = 0;
    
    stack_r0 = (unsigned int)*hardfault_args;
    stack_r1 = (unsigned int)*(hardfault_args+1);
    stack_r2 = (unsigned int)*(hardfault_args+2);
    stack_r3 = (unsigned int)*(hardfault_args+3);
    stack_r12 = (unsigned int)*(hardfault_args+4);
    stack_lr = (unsigned int)*(hardfault_args+5);
    stack_pc = (unsigned int)*(hardfault_args+6);
    stack_psr = (unsigned int)*(hardfault_args+7);

    text_start = (unsigned int)&__text_start__;
    text_end = (unsigned int)&__text_end__;

    printf("stack psr: 0x%08x\r\n",stack_psr);
    printf("stack r0: 0x%08x\r\n", stack_r0);
    printf("stack r1: 0x%08x\r\n", stack_r1);
    printf("stack r2: 0x%08x\r\n", stack_r2);
    printf("stack r3: 0x%08x\r\n", stack_r3);
    //printf("stack r4: 0x%08x\r\n", stack_r4);
    //printf("stack r5: 0x%08x\r\n", stack_r5);
    //printf("stack r6: 0x%08x\r\n", stack_r6);
    //printf("stack r7: 0x%08x\r\n", stack_r7);
    //printf("stack r8: 0x%08x\r\n", stack_r8);
    //printf("stack r9: 0x%08x\r\n", stack_r9);
    //printf("stack r10: 0x%08x\r\n",stack_r10);
    //printf("stack r11: 0x%08x\r\n",stack_r11);
    printf("stack r12: 0x%08x\r\n", stack_r12);
    printf("stack lr: 0x%08x\r\n", stack_lr);
    printf("stack pc: 0x%08x\r\n", stack_pc);
    printf("current lr: 0x%08x\r\n",lr_value);
    printf("FaultHandler\n");
    //asm("bkpt");
    printf(".text section range:0x%08x-0x%08x .\r\n",text_start,text_end);
    stack_addr = hardfault_args;
    for(i = 1;i < 512; i++)
    {   
        if((unsigned int)stack_addr == RAM_STACK_BOTTOM_ADDR)
        {
            printf("\r\nstack bottom, break !\r\n");
            break;
        }
        // thumb mode
        if(*stack_addr & 0x1)
        {
            // check the addr value if is in .text section range 
            if((*stack_addr >= text_start) && (*stack_addr < text_end))
            {
                stack_value = (*stack_addr & 0xFFFFFFFE) - 4;
                //detect if thumb BL or BLX machine code 
                insl = *(unsigned short*)stack_value;
                insh = *(unsigned short*)(stack_value+2);
                if(((insh & BL_INS_MASK) == BL_INS_HIGH) &&
                    ((insl & BL_INS_MASK) == BL_INS_LOW)
                  )
                {
                    printf("0x%08x-0x%08x|",stack_addr,stack_value);
                    print_cnt++;
                }
                else if((insh & BLX_INS_MASK) == BLX_INX )
                {
                    printf("0x%08x-0x%08x|",stack_addr,stack_value);
                    print_cnt++;
                }
                if(print_cnt%4 == 0)
                {
                    printf("\r\n");
                }
            }
        }
        stack_addr++;
    }
    printf("\r\n");
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    for(;;);
}

unsigned int showBuildVersion(void)
{
    const char Date[12] = __DATE__;
    const char Time[9] = __TIME__;
    unsigned int ulCpuInfo = 0;
    unsigned int ulCpuImplementor = 0;
    unsigned int ulCpuVariant = 0;
    unsigned int ulCpuArchitecture = 0;
    unsigned int ulCpuPartno = 0;
    unsigned int ulCpuRevision = 0;

    ulCpuInfo =  *(volatile unsigned int *)(PPB_BASE + M0PLUS_CPUID_OFFSET);    
    ulCpuImplementor  = (ulCpuInfo & M0PLUS_CPUID_IMPLEMENTER_BITS) >> M0PLUS_CPUID_IMPLEMENTER_LSB;
    ulCpuVariant  = (ulCpuInfo & M0PLUS_CPUID_VARIANT_BITS) >> M0PLUS_CPUID_VARIANT_LSB;
    ulCpuArchitecture = (ulCpuInfo & M0PLUS_CPUID_ARCHITECTURE_BITS) >> M0PLUS_CPUID_ARCHITECTURE_LSB;
    ulCpuPartno = (ulCpuInfo & M0PLUS_CPUID_PARTNO_BITS) >> M0PLUS_CPUID_PARTNO_LSB;
    ulCpuRevision = (ulCpuInfo & M0PLUS_CPUID_REVISION_BITS);
    printf("cpu implementor:0x%02x,variant:0x%01x,architecture:0x%01x,partno:0x%03x,revision:0x%01x\r\n", ulCpuImplementor ,ulCpuVariant ,ulCpuArchitecture ,ulCpuPartno ,ulCpuRevision );
    printf("build time:%s-%s.\r\n",Date,Time);
    printf("sdk version:%s.\r\n",PICO_SDK_VERSION_STRING);
    return 0;
}

int test_hardfault_b(int a,int b,int *c)
{
        int value = 0;
        int *p ;
        printf("enter function %s\r\n",__func__);
       *c = a / b;
        printf("c = %d\n",*c);

        p = (int *)0x00;
        value = *p;
        printf("addr:0x%02x value:0x%08x\r\n",(int)p,value);
        p = (int *)0x04;
        value = *p;
        printf("addr:0x%02x value:0x%08x\r\n",(int)p,value);        
        p = (int *)0x03;
        value = *p;
        printf("addr:0x%02x value:0x%08x\r\n",(int)p,value);
        printf("quit function %s\r\n",__func__);
        return 0;
}

int test_hardfault_a(int a,int b,int *c)
{
    int sum = a+b;
    printf("enter function %s\r\n",__func__);
    test_hardfault_b(a,b,c);
    printf("quit function %s \r\n",__func__);
    return 0;
}

int  test_hardfault(int a, int b, int *c)
{
    int sum = a+b;
    printf("enter function %s\r\n",__func__);
    test_hardfault_a(a,b,c);
    printf("quit function %s\r\n",__func__);
    return 0;
}


int main() 
{
    int a = 100;
    int b = 0;
    int cnt = 0;
    int c = 0;
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    stdio_init_all();
    printf("hello,rp2040!.\n");
    showBuildVersion();
    for(cnt = 0;cnt<10;cnt++)
    {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
    test_hardfault(a,b,&c);
    return 0;
}
