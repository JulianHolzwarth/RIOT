#include <stdio.h>
#include "mutex.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"

int main(void)
{
    
    // testing semaphore
    puts("starting test");
    SemaphoreHandle_t test_semaphore = xSemaphoreCreateMutex();
    if (xSemaphoreTake(test_semaphore, 0) == pdTRUE){
        puts("true");
    }
    if (xSemaphoreTake(test_semaphore, 0) == pdFALSE){
        puts("false");
    } else
    {
        puts("true");
    }
    
    if (xSemaphoreGive(test_semaphore) == pdTRUE) {
        puts("true");
    }
    vSemaphoreDelete(test_semaphore);
    return 0;
}
