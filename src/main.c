/* NOTAS
- VÊ BRANCO: output = 0
- VÊ PRETO: output = 1
- É o meio da parte transparente que detecta a linha
- IN1 = motor esquerda para frente
- IN2 = motor esquerda para trás
- IN3 = motor direita para frente
- IN4 = motor direita para trás
- Sensor do pino 20 = esquerda
- Sensor do pino 21 = direita
 */

#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // API para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)
#include <pwm_z42.h>                // Biblioteca personalizada com funções de controle do TPM (Timer/PWM Module)

// Define o valor do registrador MOD do TPM para configurar o período do PWM
#define TPM_MODULE 1000         // Define a frequência do PWM fpwm = (TPM_CLK / (TPM_MODULE * PS))
#define SLEEP_TIME_MS 1000
#define INPUT_PORT DT_NODELABEL(gpioe)
#define INPUT_PIN1 20
#define INPUT_PIN2 21

int main(void)
{
    // Configura os sensores
    const struct device *input_dev;
    int ret1, ret2, esquerda, direita;
	input_dev = DEVICE_DT_GET(INPUT_PORT);
    if (!input_dev) {
        printk("Erro ao acessar porta\n");
        return 1;
    }

	ret1 = gpio_pin_configure(input_dev, INPUT_PIN1, GPIO_INPUT);
    if (ret1 != 0) {
        printk("Erro ao configurar pino %d\n", INPUT_PIN1);
        return 1;
    }

	ret2 = gpio_pin_configure(input_dev, INPUT_PIN2, GPIO_INPUT);
    if (ret2 != 0) {
        printk("Erro ao configurar pino %d\n", INPUT_PIN2);
        return 1;
    }

    // Configura os motores
    pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Ch_Init(TPM0, 3, TPM_PWM_H,GPIOD,3);
    pwm_tpm_Ch_Init(TPM0, 2, TPM_PWM_H,GPIOD,2);
    pwm_tpm_Ch_Init(TPM0, 0, TPM_PWM_H,GPIOD,0);
    pwm_tpm_Ch_Init(TPM0, 5, TPM_PWM_H,GPIOD,5);
int n=0;
    while (1) {
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		direita = gpio_pin_get(input_dev, INPUT_PIN2);
        printk("Valor do esquerda: %d\n", esquerda);
        printk("Valor do direita: %d\n", direita);
//ptd3 - in1 -> motor e direto
//ptd2  - in2 -> motor e reverso
//ptd0 - in3 -> motor d direto
//ptd5 - in4 -> motor d reverso

uint16_t duty_n = TPM_MODULE*n/100;
//uint16_t duty_0 = TPM_MODULE*0/100;
//uint16_t duty_10 = TPM_MODULE*10/100;
//uint16_t duty_20 = TPM_MODULE*20/100;
//uint16_t duty_30 = TPM_MODULE*30/100;
//uint16_t duty_40 = TPM_MODULE*40/100;
uint16_t duty_50 = TPM_MODULE*50/100;
//uint16_t duty_60 = TPM_MODULE*60/100;
//uint16_t duty_70 = TPM_MODULE*70/100;
//uint16_t duty_80 = TPM_MODULE*80/100;
uint16_t duty_90 = TPM_MODULE*90/100;
//uint16_t duty_100 = TPM_MODULE*100/100;
//checka se entrou em uma curva
if (esquerda == 1 && direita == 0) {
            pwm_tpm_CnV(TPM0, 3, 0);
            pwm_tpm_CnV(TPM0, 2, duty_50);
            pwm_tpm_CnV(TPM0, 0, duty_90); 
            pwm_tpm_CnV(TPM0, 5, 0);
            printk("Passou aqui: esquerda == 0 && direita == 1\n");
           // k_msleep(20);
            n=80;
        }
        else if (esquerda == 0 && direita == 1) {
            pwm_tpm_CnV(TPM0, 3, duty_90);
            pwm_tpm_CnV(TPM0, 2, 0);
            pwm_tpm_CnV(TPM0, 0, 0);
            pwm_tpm_CnV(TPM0, 5, duty_50);
          //  k_msleep(20);
            n=80;
        }
        //não entrou, acelera
        //aceleração gradual a cada 10ms
        else {
        pwm_tpm_CnV(TPM0, 3, duty_n); //frente esquerda
        pwm_tpm_CnV(TPM0, 2, 0); //trás esquerda
        pwm_tpm_CnV(TPM0, 0, duty_n); //frente direita
        pwm_tpm_CnV(TPM0, 5, 0); //trás direita
         //   k_msleep(10);
            if(n<=100) n++;
    }
    k_msleep(10);
}
    return 0;
}