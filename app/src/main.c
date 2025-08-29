#include "stm32g4xx_hal.h"

#include "settings.h"
#include "jobs.h"

JobsDef Application;
SerialPortDef Serial;

int main(void) {
    HAL_Init();
    initialization(&Application.hardware);
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    createJobs(&Application);
    vTaskStartScheduler();

    while (1) {
    }

    return 1;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif
