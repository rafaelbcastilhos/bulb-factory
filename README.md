# lamp-factory

### Passos para fabricação da lâmpada:
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

