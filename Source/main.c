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
#include "kly_error.h"
#include "kly_gpio.h"

#define LED_PORT    2
#define LED_POS     9
#define LED_MASK    (0xF << LED_POS)


void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
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
    for( ;; );
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
void __attribute__((optimize("03")))vApplicationStackOverflowHook(TaskHandle_t xTask,
        signed char *pcTaskName)
{
    static const int taskname_buffer_size = 10;
    static char taskname[10]__attribute__((unused));
    static const __attribute__((used)) uint32_t error_const = 0x53544f56; //!< ASCII for STOV

    strncpy(taskname, (char *)pcTaskName, taskname_buffer_size);

    for( ;; );
}

/**
 * Function For Handling MALLOC Errors
 */
void vApplicationMallocFailedHook(void)
{
    static const __attribute__((used)) uint32_t error_const = 0x4d414c4c; //!< ASCII for MALL

    for( ;; );
}



/**
 * Application error handler for asserts. Implemented by application.
 * @param file File name
 * @param line Line number
 * @param msg Error message
 */
void kly_application_error_handler(const char *file, uint32_t line, const char *msg)
{
    static const __attribute__((used)) uint32_t error_const = 0x4b4c5945; //!< ASCII for KLYE

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

    kly_gpio_port_config(LED_PORT, LED_MASK, KLY_GPIO_CONFIG_OUTPUT_PUSH_PULL, KLY_GPIO_PULL_NONE);

    task_tick = xTaskGetTickCount();
    a = 0;
    for(;;)
    {
        kly_gpio_port_write(LED_PORT, LED_MASK, a << LED_POS);

        vTaskDelayUntil(&task_tick, 500 * portTICK_PERIOD_MS);
    }
}



/**
 * Initialize the system clocks.
 */
static void init_clock(void)
{

}



int main(void)
{
    init_clock();

    xTaskCreate(blink_task, "BLINK", 500, NULL, tskIDLE_PRIORITY+4, NULL);

    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    while(true)
    {
        /* FreeRTOS should not be here... FreeRTOS goes back to the start of stack
         * in vTaskStartScheduler function. */
    }
}
