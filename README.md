# bulb-factory

### Passos para fabricação da lâmpada:
As lâmpadas fabricadas pela empresa chagam em uma esteira e são colocadas por um braço robótico em um buffer circular com capacidade para capacidade_buffer lâmpadas. Outro robô retira as lâmpadas do buffer e as coloca em uma bancada de teste, na qual são medidos vários parâmetros relacionados ao funcionamento da lâmpada. Esta bancada de teste permite que sejam testadas até capacidade_bancada lâmpadas simultaneamente. Para que isso seja possível, cada slot da bancada de teste deverá funcionar de forma independente dos demais, realizando o teste da lâmpada que for inserida nele. Existem 4 parâmetros avaliados durante o teste: (1) se o vidro do bulbo da lâmpada está em perfeito estado; (2) se a lâmpada acende; (3) se a rosca da lâmpada está em perfeito estado; e (4) e se a marca e informações da lâmpada estão impressas no bulbo. Caso a lâmpada apresente defeito em um ou mais parâmetros ela será reprovada no teste.

Finalizado o teste, um terceiro robô retira as lâmpadas da bancada de teste e as coloca no compartimento de carga de dois veículos autoguiados (AGVs) com capacidade para capacidade_agv lâmpadas cada. Em um deles são colocadas as lâmpadas que estão com defeito e serão recicladas, e no outro são colocadas as lâmpadas aprovadas no teste, que serão embaladas para serem comercializadas. Sempre que seu compartimento de carga estiver cheio, o AGV o transporta a carga para o depósito e outro AGV vazio toma o seu lugar. O total de AGVs em operação na fábrica é dado por quantidade_agvs.
![Passos para fabricação da lâmpada](https://raw.githubusercontent.com/rafaelbcastilhos/lamp-factory/main/steps.png)


### Compilação e Execução

Para compilar o projeto, execute:

    make

Para limpar completamente o projeto, execute:

    make clean

O programa aceita as seguintes opções na linha de comando:

|**Opção**| **Campo correspondente**| **Descrição**|
|--|--|--|
|**-l**| **config.total_lampadas**|total de lâmpadas que serão testadas durante a simulação|
| **-v** | **config.velocidade_esteira** | velocidade da esteira |
|**-b**| **config.capacidade_buffer**|tamanho do buffer|
|**-t**|**config.capacidade_bancada**|número de lâmpadas que podem ser testadas simultaneamente na bancada de teste|
|**-a**|**config.quantidade_agvs**|número de AGVs em operação na fábrica|
|**-c**|**config.capacidade_agv**|número de lâmpadas que podem ser colocadas no compartimento de carga do AGV|
|**-h**|**-x-x-x-**|imprime uma mensagem de ajuda descrevendo todas as opções|
  
Para executar o programa de modo que 200 lâmpadas sejam testadas, com velocidade da esteira igual a 5, usando um buffer de tamanho 10, uma bancada de teste para 4 lâmpadas e 6 AGVs com capacidade para 12 lâmpadas cada, execute:

    ./program -l 200 -v 5 -b 10 -t 4 -a 6 -c 12


