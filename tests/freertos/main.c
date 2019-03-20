
#include <stdio.h>
#include "mutex.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"

void print_semaphore_count(SemaphoreHandle_t xSemaphore)
{
    UBaseType_t temp_count = uxSemaphoreGetCount(xSemaphore);
        printf("Semaphor Count: %i\n",temp_count);
}

void test_semaphore(SemaphoreHandle_t testing_semaphore)
{
    if (testing_semaphore == NULL) {
        puts("Error in semaphor creation");
        return;
    }
    puts("semaphore is created");
    print_semaphore_count(testing_semaphore);
    if (xSemaphoreGive(testing_semaphore) == pdTRUE) {
        puts("Semaphore unblock true");
    }
    else {
        puts("Semaphore unblock false");
    }
    print_semaphore_count(testing_semaphore);
    if (xSemaphoreTake(testing_semaphore, 0) == pdTRUE) {
        puts("First semaphore block true");
    }
    else {
        puts("First semaphore block false");
    }
    print_semaphore_count(testing_semaphore);
    
    if (xSemaphoreTake(testing_semaphore, 0) == pdTRUE) {
        puts("Second semaphore block true");
    }
    else {
        puts("Second semaphore block false");
    }

    print_semaphore_count(testing_semaphore);

    if (xSemaphoreGive(testing_semaphore) == pdTRUE) {
        puts("Semaphore unblock true");
    }
    else {
        puts("Semaphore unblock false");
    }

    print_semaphore_count(testing_semaphore);

    if (xSemaphoreGive(testing_semaphore) == pdTRUE) {
        puts("Semaphore unblock true");
    }
    else {
        puts("Semaphore unblock false");
    }
    print_semaphore_count(testing_semaphore);
    vSemaphoreDelete(testing_semaphore);
    puts("Semaphore deleted");
}


int main(void)
{
    
    // testing semaphore
    puts("starting test");
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateCounting(3,2);
    test_semaphore(testing_semaphore);
    return 0;
}
