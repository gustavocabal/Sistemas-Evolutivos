# Sistemas Evolutivos Aplicados à Robótica (SSC0713)
Este diretório contém o código e a implementação do nosso trabalho final da disciplina Sistemas Evolutivos Aplicados à Robótica.

O objetivo geral do projeto foi aplicar conceitos de algoritmos evolutivos para desenvolver indivíduos capazes de interpretar o ambiente, tomar decisões inteligentes e se adaptar para maximizar sua pontuação.
Para isso, criamos uma variação do jogo Campo Minado, habitada por sapos em evolução.

Video explicando: 
https://youtu.be/5kfnitTOW4o?si=VzprpkWwin_Tg59T

## A procura de um super sapo
Se aprofundando nos conceito do código, podemos dividi-lo em três áreas gerais.

- **Mapa:** Criação da área em que viverá a população e a interação dos individuos com seus obstáculos.
- **Visão:** A capacidade do sapo de observar seu ambiente e alterar sua tomada de decisão.
- **Movimentação:** Escolha e realização de sua ação.
- **Evolução:** Algoritimo genético que permite encontrar os melhores parametros para os genes do sapo.


### Mapa
Quando iniciou o projeto, foi tido multiplas ideias para o ambiente que a população ia viver, porém a que realmente se realizou foi algo similar ao jogo campo minado.

Criamos uma matriz quadrada de tamanho váriavel, onde cada ponto da matriz poderia ter 4 diferentes elementos:
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
Vale notar que, cada individuo da população tem uma caracteristica chamada "int orientacao" que indica a direção que está olhando, sendo possivel, então, observar toda a volta do sapo.

<img width="900" height="800" alt="frog_orientacao" src="https://github.com/user-attachments/assets/b9d780ed-d878-42e0-8989-86739059f87b" />
**Representação das possíveis orientações do sapo**

### Movimentação
Como o mapa é uma matriz, decidimos ir pelo caminho simples e fizemos com que os possíveis movimentos do sapo seja uma matriz 3x3 que envolve o individuo.
Entretanto, ao levar em conta a orientação e a visão, o sapo pode tomar 7 diferentes decisões de movimento:
- Pular em um espaço vazio em sua frente
- Pular em uma mosca em sua frente
- Pular em uma vala em sua frente
- Pular em uma bomba em sua frente
- Girar pra esquerda
- Girar pra direita
- Pular em um espaço fora do campo de visão

<img width="900" height="800" alt="frog_mov" src="https://github.com/user-attachments/assets/e17e8ad2-2eb9-442f-b248-c8befdf0a4e3" />
**Representação dos possíveis movimentos dos sapo**


## Evolução
Para que os sapos se tornem progressivamente mais eficientes em sobreviver e pontuar no ambiente, implementamos um processo evolutivo baseado em um algoritmo genético simples, porém eficaz.
Nosso modelo segue duas ideias fundamentais:

O melhor indivíduo da geração deve ser preservado e servir de base para a próxima
A mutação deve aumentar quando o progresso estagnar, permitindo escapar de ótimos locais

### Seleção
A cada geração, todos os indivíduos são avaliados com base no seu fitness, que considera:
- Número de moscas comidas
- Quantidade de saltos dados
- Bombas pisadas

Após a simulação completa, o melhor indivíduo da população é selecionado como individuo dominante e reproduz para gerar a próxima geração.
Caso a população fique estagnada, a mutação base vai lentamente aumentando até a saída do ótimo local.




