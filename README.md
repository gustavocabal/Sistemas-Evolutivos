# Sistemas Evolutivos Aplicados à Robótica (SSC0713)
Este diretório contém o código e a implementação do nosso trabalho final da disciplina Sistemas Evolutivos Aplicados à Robótica.

O objetivo geral do projeto foi aplicar conceitos de algoritmos evolutivos para desenvolver indivíduos capazes de interpretar o ambiente, tomar decisões inteligentes e se adaptar para maximizar sua pontuação.
Para isso, criamos uma variação do jogo Campo Minado, habitada por sapos em evolução.


## A procura de um super sapo
Se aprofundando nos conceito do código, podemos dividi-lo em três áreas gerais.

- **Mapa:** Criação da área em que viverá a população e a interação dos individuos com seus obstáculos.
- **Visão:** A capacidade do sapo de observar seu ambiente e alterar sua tomada de decisão.
- **Movimentação:** Escolha e realização de sua ação.
- **Evolução:** Algoritimo genético que permite encontrar os melhores parametros para os genes do sapo.


### Mapa
Quando iniciou o projeto, foi tido multiplas ideias para o ambiente que a população ia viver, porém a que realmente se realizou foi algo similar ao jogo campo minado.

Criamos uma matriz quadrada de tamanho váriavel, onde cada ponto da matriz poderia ter 4 diferentes elementos,
- Moscas (M): aumentam consideravelmente a pontuação do sapo
- Bombas (B): matam o sapo, o impedindo de se mover pelo resto da geração
- Valas (V): atrapalham os sapos, causando que ele fique parado por um turno
- Nada ( ): não causa nada.

Cada elemento afeta o comportamento e a sobrevivência do sapo de forma distinta.
A geração do mapa é aleatória, garantindo que a solução evoluída precise ser geral, e não decorada ou específica para um único cenário.


### Visão
Inicialmente, consideramos implementar sapos sem qualquer percepção do ambiente, limitados a movimentos totalmente aleatórios.
No entanto, esse modelo leva a indivíduos frágeis, altamente dependentes do layout exato do mapa.

Com isso, elaboramos uma lógica de que o sapo poderia identificar as três casa em sua frente naquele momento do código, e levar essa informação em consideração antes de realizar o próximo movimento.
Vale notar que, cada individuo da população tem uma caracteristica chamada "int orientacao" que indica a direção que está olhando, sendo possivel, então, observar toda a volta do sapo

<img width="1526" height="1432" alt="frog" src="https://github.com/user-attachments/assets/a295d8cf-2c53-4110-9a60-d54fceb563f9" />
**Representação das possíveis orientações do sapo**

### Movimentação

<img width="1438" height="1266" alt="frog" src="https://github.com/user-attachments/assets/2ff6395a-b5b2-4bcc-a18d-d2cfd76a0f6e" />
**Representação dos possíveis movimentos dos sapo**




