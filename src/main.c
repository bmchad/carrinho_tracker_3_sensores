/* NOTAS
- VÊ BRANCO: output = 1
- VÊ PRETO: output = 0
- É o meio da parte transparente que detecta a linha
- IN1 = motor direita para trás
- IN2 = motor direita para frente
- IN3 = motor esquerda para trás
- IN4 = motor esquerda para frente
- Sensor do pino 20 = esquerda
- Sensor do pino 21 = meio
- Sensor do pino 22 = direita
*/

#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // API para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)
#include <pwm_z42.h>                // Biblioteca personalizada com funções de controle do TPM (Timer/PWM Module)
#include <sensor-ultrassonico.h>

// Define o valor do registrador MOD do TPM para configurar o período do PWM
#define TPM_MODULE 1000         // Define a frequência do PWM fpwm = (TPM_CLK / (TPM_MODULE * PS))
#define SLEEP_TIME_MS 1000
#define INPUT_PORT DT_NODELABEL(gpioe)
#define INPUT_PIN1 20
#define INPUT_PIN2 21
#define INPUT_PIN3 22

void direita_m(int duty_n){
    if (duty_n >= 0) {
        pwm_tpm_CnV(TPM0, 2, duty_n); //canal 2 in 2 -> direita para frente
        pwm_tpm_CnV(TPM0, 3, 0);      //canal 3 in 1 -> direita para trás 
    } else {
        pwm_tpm_CnV(TPM0, 3, -duty_n); // Reverso
        pwm_tpm_CnV(TPM0, 2, 0); 
    }
}

void esquerda_m(int duty_n){
    if (duty_n >= 0) {
        pwm_tpm_CnV(TPM0, 5, duty_n); //canal 5 in 4 -> esquerda para frente
        pwm_tpm_CnV(TPM0, 0, 0);      //canal 0 in 3 -> esquerda para trás
    } else {
        pwm_tpm_CnV(TPM0, 0, -duty_n); // Reverso
        pwm_tpm_CnV(TPM0, 5, 0); 
    }
}
int duty(int n){
    uint16_t d = TPM_MODULE*n/100;
    return d;
}
/* CÓDIGO TESTE DE MOTORES ESTÁ FUNCIONANDO PERFEITAMENTE
int main (void){
        pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Ch_Init(TPM0, 3, TPM_PWM_H,GPIOD,3);
    pwm_tpm_Ch_Init(TPM0, 2, TPM_PWM_H,GPIOD,2);
    pwm_tpm_Ch_Init(TPM0, 0, TPM_PWM_H,GPIOD,0);
    pwm_tpm_Ch_Init(TPM0, 5, TPM_PWM_H,GPIOD,5);
    while(1){
direita_m(duty(100));
esquerda_m(duty(0));
k_msleep(2000);

direita_m(duty(0));
esquerda_m(duty(0));
k_msleep(1000);

direita_m(-duty(100));
esquerda_m(duty(0));
k_msleep(2000);

direita_m(duty(0));
esquerda_m(duty(100));
k_msleep(2000);

direita_m(duty(0));
esquerda_m(duty(0));
k_msleep(1000);

direita_m(duty(0));
esquerda_m(-duty(100));
k_msleep(2000);

direita_m(duty(0));
esquerda_m(duty(0));
k_msleep(3000);
    }
    return 0;
}
*/

/*CODIGO PARA TESTAR OS SENSORES
DE FATO 
VÊ PRETO = 0
VÊ BRANCO = 1
int main(void)
{
    // Configura os sensores
    const struct device *input_dev;
    int esquerda, direita, ret1, ret2;
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

	ret2 = gpio_pin_configure(input_dev, INPUT_PIN3, GPIO_INPUT);
    if (ret2 != 0) {
        printk("Erro ao configurar pino %d\n", INPUT_PIN3);
        return 1;
    }

    // Configura os motores
    pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_8, CENTER_PWM);
    pwm_tpm_Ch_Init(TPM0, 3, TPM_PWM_H,GPIOD,3);
    pwm_tpm_Ch_Init(TPM0, 2, TPM_PWM_H,GPIOD,2);
    pwm_tpm_Ch_Init(TPM0, 0, TPM_PWM_H,GPIOD,0);
    pwm_tpm_Ch_Init(TPM0, 5, TPM_PWM_H,GPIOD,5);

    while (1) {
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);

        if (esquerda == 0 && direita == 0) {
            direita_m(duty(100));
            esquerda_m(duty(100));
        }
        else if (esquerda == 1 && direita == 0) {
            direita_m(duty(0));
            esquerda_m(duty(100));
        }
        else if (esquerda == 0 && direita == 1) {
            direita_m(duty(100));
            esquerda_m(duty(0));
        }
        else if (esquerda == 1 && direita == 1) {
            direita_m(duty(100));
            esquerda_m(duty(100));
        }
    }
}
    */
int main(void){
    // Configura os sensores
    const struct device *input_dev;
    int ret1, ret2, ret3, esquerda, meio, direita;
	input_dev = DEVICE_DT_GET(INPUT_PORT);
    if (!input_dev) {
        printk("Erro ao acessar porta\n");
        return 1;
    }
    //esquerda no pino 20, meio no pino 21 e direita no pino 22, gpioe
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

    //configuração do ultrassom
float distancia;
sensorUltrassonicoInit();
//configuração do sistema de memória
int n=0; 
char curva = 'd'; 
int vmin = 60;
int sleep = 1;
    while (1) {
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
        printk("Valor do esquerda: %d\n", esquerda);
        printk("Valor do meio: %d\n", meio);
        printk("Valor do direita: %d\n", direita);
//ptd3 - in1 -> motor d inverso
//ptd2  - in2 -> motor d direto
//ptd0 - in3 -> motor e inverso
//ptd5 - in4 -> motor e direto
//o carrinho começa com o sensor do meio à esquerda da linha
//checa se tem esta longe de um obstáculo
distancia = calculaDistancia();
printk("Distancia: %f\n", distancia);
        if(distancia>25){
       //SISTEMA DE MEMORIA PARA SE ELE SAIR DA PISTA
       if(esquerda == 1 && meio == 0 && direita == 1) { 
        // a situação esquerda = 0, meio = 1 e direita = 0 é a iminencia de sair
        if(curva== 'e'){
            esquerda_m(duty(40));
            direita_m(duty(60)); //deve corrigir ao lado contrário da inércia
            if((esquerda == 1 && meio == 0 && direita == 0) || (esquerda == 1 && meio == 1 && direita == 0)) curva = 'd';
            k_msleep(sleep);
    }
            //saiu da pista pela esquerda (estava virando à direita)
            else if(curva== 'd'){
            esquerda_m(duty(60));
            direita_m(duty(40)); //deve corrigir ao lado contrário da inércia
            if((esquerda == 0 && meio == 0 && direita == 1) || (esquerda == 0 && meio == 1 && direita == 1)) curva = 'e';
            k_msleep(sleep);
        }
        n = vmin;
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
}
/*
       if(esquerda == 0 && meio == 1 && direita == 0) { 
        esquerda_m(duty(80));
        direita_m(duty(80));
        k_msleep(sleep);
        n = vmin;
       }
        */
       else if(esquerda == 1 && meio == 1 && direita == 1) { //saiu da pista
        //saiu da pista pela direita (estava virando à esquerda)
        //na pratica: se os sensores são todos 0, o melhor comportamento é ele corrigir levemete para o lado da curva
        if(curva== 'e'){
            esquerda_m(duty(55));
            direita_m(duty(70));
            if((esquerda == 1 && meio == 0 && direita == 0) || (esquerda == 1 && meio == 1 && direita == 0)) curva = 'd';
            k_msleep(sleep);
            n = vmin;
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
        }
            //saiu da pista pela esquerda (estava virando à direita)
            else if(curva== 'd'){
            esquerda_m(duty(70));
            direita_m(duty(55));
            if((esquerda == 0 && meio == 0 && direita == 1) || (esquerda == 0 && meio == 1 && direita == 1)) curva = 'e';
            k_msleep(sleep);
            n=vmin;
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
}
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
       }
    //sistema de seugurança para se o sistema de memória falhar, ele para o carrinho
    else if((esquerda == 0 && meio == 1 && direita == 1) || (esquerda == 0 && meio == 0 && direita == 1)){ //de corrigir à esquerda
            curva = 'e';
            esquerda_m(duty(0));
            direita_m(duty(60));
            k_msleep(sleep);
            n = vmin;
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
    }
    else if((esquerda == 1 && meio == 1 && direita == 0) || (esquerda == 1 && meio == 0 && direita == 0)){ //de corrigir à direita
            curva = 'd';
            esquerda_m(duty(60));
            direita_m(duty(0));
            k_msleep(sleep);
            n = vmin;
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
    }
    //não saiu e o sistema de segurança foi verificado, acelera
        //aceleração gradual a cada 10ms
        else {
            uint16_t duty_n = TPM_MODULE*n/100;
        pwm_tpm_CnV(TPM0, 5, duty_n); //frente esquerda
        pwm_tpm_CnV(TPM0, 0, 0); //trás esquerda
        pwm_tpm_CnV(TPM0, 2, duty_n); //frente direita
        pwm_tpm_CnV(TPM0, 3, 0); //trás direita
        k_msleep(sleep);
        if(n<=80) n++;
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		meio = gpio_pin_get(input_dev, INPUT_PIN2);
		direita = gpio_pin_get(input_dev, INPUT_PIN3);
    }
}
else if(distancia<=25){
    // Freia primeiro
    esquerda_m(duty(0));
    direita_m(duty(0));
    k_msleep(200);
        esquerda_m(-duty(50));
        direita_m(-duty(50));
        k_msleep(800);
}
    }
    return 0;
}