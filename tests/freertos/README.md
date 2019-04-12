# Freertos
Test application for the freertos compatibility layer.
Read the README in sys/freertos for more information.

## Commands
### commands for semaphores
- cmd_test_mutex
  - tests the mutex semaphore
  - Expected result:
    - ```
      starting test: mutex semaphore
      OK
      ```
- cmd_test_recursive_mutex
  - tests the recursive mutex semaphore
  - Expected result:
    - ```
      starting test: recursive mutex semaphore
      OK
      ```
- cmd_test_binary
  - tests the binary semaphore
  - Expected result:
    - ```
      starting test: binary semaphore
      OK
      ```
- cmd_test_counting
  - tests the counting semaphore
  - Expected result:
    - ```
      starting test: counting semaphore
      OK
      ```
### other commands
- cmd_test_task
  - tests the freertos task
  - Expected result:
    - ```
      starting test: task
      OK
      ```
- cmd_test_timer
  - tests the freertos timer
  - Expected result:
    - ```
      starting test: timer
      OK
      ```
- cmd_test_queue
  - tests the freertos queue
  - Expected result:
    - ```
      starting test: queue
      OK
      ```
