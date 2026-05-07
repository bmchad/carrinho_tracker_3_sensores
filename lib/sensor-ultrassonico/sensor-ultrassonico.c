#include "sensor-ultrassonico.h"

volatile uint16_t tick_subida = 0;
volatile uint16_t tick_descida = 0;
volatile uint16_t tick = 0;
volatile float t = 0; // tempo que o pulso ficou ativo em microssegundos
volatile float dist = 0; // distancia em cm
const struct device *port_b = DEVICE_DT_GET(B_PORT);

__ramfunc void tpm1_isr(void *arg) {
    TPM1->STATUS |= TPM_STATUS_CH0F_MASK; // zera a flag que gerou a interrupção
    if ((PTB->PDIR & 1) == 1) {
        tick_subida = TPM1->CONTROLS[0].CnV;
        return;
    }
    else {
        tick_descida = TPM1->CONTROLS[0].CnV;
        if (tick_descida > tick_subida) {
            tick = tick_descida - tick_subida;
            t = tick / 3;
            dist = t * 0.017f;
        }
        else {
            tick = 65536 - (tick_subida - tick_descida);
            t = tick / 3;
            dist = t * 0.017f;
        }
    }
}

void sensorUltrassonicoInit(void) {
    // Configura a porta B0 como input
    gpio_pin_configure(port_b, ECHO_PIN, GPIO_INPUT);

    // Configura a porta B2 como output
    gpio_pin_configure(port_b, TRIGGER_PIN, GPIO_OUTPUT);

    // Conecta a interrupção via Zephyr
    IRQ_CONNECT(TPM_IRQ_LINE, TPM_IRQ_PRIORITY, tpm1_isr, NULL, 0);
    irq_enable(TPM_IRQ_LINE);

    // Inicializa TPM1 e TPM0 com módulo e prescaler desejado
    pwm_tpm_Init(TPM1, TPM_PLLFLL, 65535, TPM_CLK, PS_16, EDGE_PWM);
    //pwm_tpm_Init(TPM2, TPM_PLLFLL, 960, TPM_CLK, PS_1, EDGE_PWM);

    // Configura TPM1_CH0 como input capture na borda de subida
    pwm_tpm_Ch_Init(TPM1, 0, TPM_INPUT_CAPTURE_BOTH | TPM_CHANNEL_INTERRUPT, GPIOB, 0);
    //pwm_tpm_Ch_Init(TPM2, 0, TPM_PWM_H, GPIOB, 2);
    //pwm_tpm_CnV(TPM2, 0, 480);
}

float calculaDistancia(void) {
    gpio_pin_set(port_b, TRIGGER_PIN, 0);
    k_busy_wait(2);
    gpio_pin_set(port_b, TRIGGER_PIN, 1);
    k_busy_wait(10);
    gpio_pin_set(port_b, TRIGGER_PIN, 0);
    return dist;
}