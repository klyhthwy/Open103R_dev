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
#include "stm32f1xx.h"
#include "kly_error.h"



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
    init_clock();

    for(;;)
    {
    }
}
