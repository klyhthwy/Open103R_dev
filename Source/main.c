/**
 * @file main.c
 *
 * @author kelly.hathaway
 * @date Initial: Jan 28, 2017
 * @version 1
 * @date Released: Not Released
 * @details
 */


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f1xx.h"
#include "kly_error.h"
#include "kly_gpio.h"


#define LED_PORT    2
#define LED_POS     9
#define LED_MASK    (0xF << LED_POS)



void __attribute__((optimize("00"))) prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */
    volatile uint32_t r0 __attribute__((unused));
    volatile uint32_t r1 __attribute__((unused));
    volatile uint32_t r2 __attribute__((unused));
    volatile uint32_t r3 __attribute__((unused));
    volatile uint32_t r12 __attribute__((unused));
    volatile uint32_t lr __attribute__((unused)); /* Link register. */
    volatile uint32_t pc __attribute__((unused)); /* Program counter. */
    volatile uint32_t psr __attribute__((unused));/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    KLY_FAIL("HARD_FAULT");
}

void HardFault_Handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
}


/**
 * Function For Handling Stack OVERFLOWS
 * @param xTask
 * @param pcTaskName
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
    static const char err_msg[] = "STACK_OVERFLOW in Task: ";
    static char stack_overflow_msg[sizeof(err_msg) + configMAX_TASK_NAME_LEN];

    strcpy(stack_overflow_msg, err_msg);
    strncpy(&stack_overflow_msg[sizeof(err_msg)], (char *)pcTaskName, configMAX_TASK_NAME_LEN);
    KLY_FAIL(stack_overflow_msg);
}

/**
 * Function For Handling MALLOC Errors
 */
void vApplicationMallocFailedHook(void)
{
    KLY_FAIL("MALLOC_FAILED");
}



/**
 * Application error handler for asserts. Implemented by application.
 * @param file File name
 * @param line Line number
 * @param msg Error message
 */
void __attribute__((optimize("00"))) kly_application_error_handler(const char *file, uint32_t line, const char *msg)
{

    for( ;; );
}



/**
 * Test task
 * @param pvParameters
 */
static void blink_task(void *pvParameters)
{
    TickType_t task_tick;
    uint32_t a;

    kly_gpio_port_initialize(LED_PORT);
    kly_gpio_port_config(LED_PORT, LED_MASK, KLY_GPIO_CONFIG_OUTPUT_PUSH_PULL, KLY_GPIO_PULL_NONE);

    task_tick = xTaskGetTickCount();
    a = 0;
    for(;;)
    {
        kly_gpio_port_write(LED_PORT, LED_MASK, a << LED_POS);
        a++;
        a %= 0x10;

        vTaskDelayUntil(&task_tick, 500 / portTICK_PERIOD_MS);
    }
}



/**
 * Initialize the system clocks.
 */
static void init_clock(void)
{
    register uint32_t config;

    /// Enable the HSE and wait for it to stabilize.
    RCC->CR |= RCC_CR_HSEON;
    while((RCC->CR & RCC_CR_HSERDY_Msk) == 0);

    /// Configure the PLL. The PLL must be configured in the CFGR register before
    /// turning it on in the CR.
    config = RCC->CFGR;
    config &= ~(RCC_CFGR_PLLMULL_Msk | RCC_CFGR_PLLXTPRE_Msk | RCC_CFGR_PLLSRC_Msk);
    config |= RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLXTPRE_HSE | RCC_CFGR_PLLSRC;
    RCC->CFGR = config;

    RCC->CR |= RCC_CR_PLLON;
    while((RCC->CR & RCC_CR_PLLRDY_Msk) == 0);

    /// Update the flash wait states for highest frequency clock.
    config = FLASH->ACR;
    config &= ~FLASH_ACR_LATENCY_Msk;
    config |= FLASH_ACR_LATENCY_2;
    FLASH->ACR = config;

    /// Switch to the PLL as system clock.
    config = RCC->CFGR;
    config &= ~(RCC_CFGR_SW_Msk);
    config |= RCC_CFGR_SW_PLL;
    RCC->CFGR = config;
    while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);

    /// Disable the HSI.
    RCC->CR &= ~RCC_CR_HSION;

    SystemCoreClockUpdate();
}



/**
 * Application main.
 * @return
 */
int main(void)
{
    init_clock();

    xTaskCreate(blink_task, "BLINK", 500, NULL, tskIDLE_PRIORITY+1, NULL);

    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    while(true)
    {
        /* FreeRTOS should not be here... FreeRTOS goes back to the start of stack
         * in vTaskStartScheduler function. */
    }
}
