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
#include "kly_gpio.h"
#include "kly_error.h"


#define LED_PORT    2
#define LED_POS     9
#define LED_MASK    (0xF << LED_POS)


void HardFault_Handler(void)
{
}



/**
 * Application error handler for asserts. Implemented by application.
 * @param file File name
 * @param line Line number
 * @param msg Error message
 */
void kly_application_error_handler(const char *file, uint32_t line, const char *msg)
{

    for( ;; );
}



/**
 * Initialize the system clocks.
 */
static void init_clock(void)
{

}



int main(void)
{
    int a;

    init_clock();

    kly_gpio_port_config(LED_PORT, LED_MASK,
        KLY_GPIO_CONFIG_OUTPUT_PUSH_PULL, KLY_GPIO_PULL_NONE);

    a = 0;
    for(;;)
    {
        kly_gpio_port_write(LED_PORT, LED_MASK, a << LED_POS);
        a++;
        a %= 0x10;
        for(int i = 0; i < 0x40000; i++);
    }
}
