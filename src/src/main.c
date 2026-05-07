/* NOTAS
- VÊ BRANCO: output = 0
- VÊ PRETO: output = 1
- É o meio da parte transparente que detecta a linha
- IN1 = motor esquerda para frente
- IN2 = motor esquerda para trás
- IN3 = motor direita para frente
- IN4 = motor direita para trás
- Sensor do pino 20 = esquerda
- Sensor do pino 21 = meio
- Sensor do pino 22 = direita
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
#define INPUT_PIN3 22

void esquerda_direto(int duty_n){
    pwm_tpm_CnV(TPM0, 3, duty_n); //frente esquerda
}
void esquerda_reverso(int duty_n){
    pwm_tpm_CnV(TPM0, 2, duty_n); //trás esquerda
}
void direita_direto(int duty_n){
    pwm_tpm_CnV(TPM0, 0, duty_n); //frente direita
}
void direita_reverso(int duty_n){
    pwm_tpm_CnV(TPM0, 5, duty_n); //trás direita
}
int duty(int n){
    uint16_t d = TPM_MODULE*n/100;
    return d;
}

int main(void)
{
    // Configura os sensores
    const struct device *input_dev;
    int ret1, ret2, ret3, esquerda, meio, direita;
	input_dev = DEVICE_DT_GET(INPUT_PORT);
    if (!input_dev) {
        printk("Erro ao acessar porta\n");
        return 1;
    }
    //esquerda no pino 20, meio no pino 21 e direita no pino 22
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
    ret3 = gpio_pin_configure(input_dev, INPUT_PIN3, GPIO_INPUT);
    if (ret3 != 0) {
        printk("Erro ao configurar pino %d\n", INPUT_PIN3);
        return 1;
    }

    // Configura os motores
    pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Ch_Init(TPM0, 3, TPM_PWM_H,GPIOD,3);
    pwm_tpm_Ch_Init(TPM0, 2, TPM_PWM_H,GPIOD,2);
    pwm_tpm_Ch_Init(TPM0, 0, TPM_PWM_H,GPIOD,0);
    pwm_tpm_Ch_Init(TPM0, 5, TPM_PWM_H,GPIOD,5);
int n=0; char curva = 'd';
    while (1) {
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
        printk("Valor do esquerda: %d\n", esquerda);
        printk("Valor do meio: %d\n", meio);
        printk("Valor do direita: %d\n", direita);
//ptd3 - in1 -> motor e direto
//ptd2  - in2 -> motor e reverso
//ptd0 - in3 -> motor d direto
//ptd5 - in4 -> motor d reverso

uint16_t duty_n = TPM_MODULE*n/100;

//checka se entrou em uma curva
if (esquerda == 0 && meio == 1 && direita == 1) { //curva aberta para a direita
            esquerda_direto(duty(100));
            esquerda_reverso(duty(0));
            direita_direto(duty(0));
            direita_reverso(duty(10));
            curva = 'd';
            n=80;
        }
        else if (esquerda == 0 && meio == 0 && direita == 1) { //curva fechada para a direita
            esquerda_direto(duty(90));
            esquerda_reverso(duty(0));
            direita_direto(duty(0));
            direita_reverso(duty(50));
            curva = 'd';
            n=80;
        }
        else if(esquerda == 1 && meio == 1 && direita == 0) { //curva aberta para a esquerda
            esquerda_direto(duty(0));
            esquerda_reverso(duty(10));
            direita_direto(duty(100));
            direita_reverso(duty(0));
            curva = 'e';
            n=80;
        }
        else if(esquerda == 1 && meio == 0 && direita == 0) { //curva fechada para a esquerda
            esquerda_direto(duty(0));
            esquerda_reverso(duty(50));
            direita_direto(duty(90));
            direita_reverso(duty(0));
            curva = 'e';
            n=80;
        }
        /*
        esquerda = 1 & meio = 1 & direita = 0
        esquerda = 1 & meio = 0 & direita = 1
        esqueda = 0 & meio = 1 & direita = 0
        */
       //SISTEMA DE MEMORIA PARA SE ELE SAIR DA PISTA
       else if(esquerda == 0 && meio == 0 && direita == 0) { //saiu da pista
        //saiu da pista pela direita (estava virando à esquerda)
        if(curva== 'e'){
            while(curva != 'd'){
            esquerda_direto(duty(0));
            esquerda_reverso(duty(50));
            direita_direto(duty(50));
            direita_reverso(duty(0));
            esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
            meio = gpio_pin_get(input_dev, INPUT_PIN2);
            direita = gpio_pin_get(input_dev, INPUT_PIN3);
            if((esquerda == 0 && meio == 1 && direita == 1) || (esquerda == 0 && meio == 0 && direita == 1)) curva = 'd';
            printk("Valor do esquerda: %d\n", esquerda);
            printk("Valor do meio: %d\n", meio);
            printk("Valor do direita: %d\n", direita);
            k_msleep(10);
        }
    }
            //saiu da pista pela esquerda (estava virando à direita)
            else if(curva== 'd'){
            while(curva != 'e'){    
            esquerda_direto(duty(50));
            esquerda_reverso(duty(0));
            direita_direto(duty(0));
            direita_reverso(duty(50));
            esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
            meio = gpio_pin_get(input_dev, INPUT_PIN2);
            direita = gpio_pin_get(input_dev, INPUT_PIN3);
            if((esquerda == 1 && meio == 1 && direita == 0) || (esquerda == 1 && meio == 0 && direita == 0)) curva = 'e';
            printk("Valor do esquerda: %d\n", esquerda);
            printk("Valor do meio: %d\n", meio);
            printk("Valor do direita: %d\n", direita);
            k_msleep(10);
        }
    }
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