A ideia central é criar um laço while em que a cada ciclo o carrinho procura uma curva (isso é, esquerda = 0 e direita = 1 ou vice versa). Se ele não acha curva ele segue reto com n% de pwm por 10ms, antes de repetir o laço. n é um contador que começa começa em 0, acelerando 1% por ciclo. Caso o carrinho ache uma curva, o contador volta para 80. Logo, o contador só fica em 0 ao ligar o carrinho, mas durante a volta, sua velocidade de reta é no mínimo de 80. Para as curvas, a relação 90 para frente e 40 para trás trouxe bons resultados.

O código funciona de forma satisfatória, mas há um problema físico no carrinho: os sensores estão muito próximos, deixando o carrinho por vezes "preso" em zig-zag.

EVIDÊNCIAS: 
https://drive.google.com/file/d/1Eu0Jbbld2EuSbPxGxIpqz8YSCFXWB1Ga/view?usp=drive_link