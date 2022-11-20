/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/version.h"

typedef unsigned int uint;

void HardFault_Handler(unsigned int * hardfault_args,unsigned lr_value)
{
    
    unsigned int *stack_addr = NULL;
    int i = 0;
    uint stack_r0;
    uint stack_r1;
    uint stack_r2;
    uint stack_r3;
    uint stack_r4;
    uint stack_r5;
    uint stack_r6;
    uint stack_r7;
    uint stack_r8;
    uint stack_r9;
    uint stack_r10;
    uint stack_r11;
    uint stack_r12;
    uint stack_lr;
    uint stack_pc;
    uint stack_psr;
    
    stack_r0 = (unsigned int)*hardfault_args;
    stack_r1 = (unsigned int)*(hardfault_args+1);
    stack_r2 = (unsigned int)*(hardfault_args+2);
    stack_r3 = (unsigned int)*(hardfault_args+3);
    stack_r12 = (unsigned int)*(hardfault_args+4);
    stack_lr = (unsigned int)*(hardfault_args+5);
    stack_pc = (unsigned int)*(hardfault_args+6);
    stack_psr = (unsigned int)*(hardfault_args+7);



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
    stack_addr = hardfault_args;
    for(i = 1;i < 512; i++)
    {   
        if((unsigned int)stack_addr == 0x20042000)// from .ld in pico sdk 
        {
            printf("\r\nis stack bottom, break !\r\n");
            break;
        }
        printf("0x%08x-0x%08x|",stack_addr,*stack_addr);
        stack_addr++;
        if(i%4 == 0)
        {
            printf("\r\n");
        }
    }
    printf("\r\n");
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
