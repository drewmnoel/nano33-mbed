#include "nano_errata.h"
#include <hal/nrf_rtc.h>
#include <hal/nrf_uart.h>
#include <hal/nrf_uarte.h>

void fix_arduino_bootloader() {
  NRF_CLOCK->TRACECONFIG = 0;
  nrf_rtc_event_disable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);
  nrf_rtc_int_disable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);

  nrf_uarte_task_trigger(NRF_UARTE0, NRF_UARTE_TASK_STOPRX);
  while (!nrf_uarte_event_check(NRF_UARTE0, NRF_UARTE_EVENT_RXTO))
    ;
  NRF_UARTE0->ENABLE = 0;
  NRF_UART0->ENABLE = 0;

  NRF_PWM_Type *PWM[] = {NRF_PWM0, NRF_PWM1, NRF_PWM2
#ifdef NRF_PWM3
                         ,
                         NRF_PWM3
#endif
  };

  for (unsigned int i = 0; i < (sizeof(PWM) / sizeof(PWM[0])); i++) {
    PWM[i]->ENABLE = 0;
    PWM[i]->PSEL.OUT[0] = 0xFFFFFFFFUL;
  }
}
