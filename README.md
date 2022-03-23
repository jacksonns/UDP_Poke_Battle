# UDP Poke Battle
O objetivo deste trabalho prático foi assimilar e exercitar os conceitos sobre programação com sockets e protocolo UDP. O jogo, controlado apenas por linha de comando, se baseia em uma arena onde são posicionados Pokémons atacantes e defensores. Os atacantes começam na primeira coluna e se movem uma posição para frente a cada turno, já os defensores são posicionados aleatoriamente e não se movem. O objetivo é impedir com que os atacantes cheguem ao final da arena e quando qualquer um deles é destruído, um novo surge na posição 0.

O servidor é responsável por executar os comandos enviados pelo cliente, que deve decidir quais pokémons irão realizar a defesa em cada turno (cada defensor só pode defender uma única vez por turno, ou seja, só tira um ponto de vida do atacante a cada turno). Existem apenas três tipos de atacantes: **zubat**, com 1 de vida, **lugia**, com 2 de vida e **mewtwo**, com 3 de vida, que só podem ser barrados por defensores a uma distância de 1 unidade deles. Uma representação da arena de batalha pode ser vista na seguinte imagem:

<p align="center">
  <img src= "https://user-images.githubusercontent.com/42253628/159811416-927cbbfa-5d37-47d6-8812-e91ec139c42e.png" />
</p>

## Instruções

**Passo 1:** Rode o comando `make` para compilação.

**Passo 2:** Inicie o servidor passando como parâmetro o `IP` (**v4** para IPV4 ou **v6** para IPV6) e a `porta`. Como por exemplo:

```bash
./server v4 5151
```

**Passo 3:** Inicie o cliente passando como parâmentro o `endereço` (**127.0.0.1** para IPV4 ou **::1** para IPV6) e a `porta do servidor`. Para o servidor iniciado no Passo 2, por exemplo, executa-se o cliente com o seguinte comando:

```bash
./client 127.0.0.1 5151
```

## Comandos

O cliente pode controlar o jogo com os seguintes comandos:

`getdefenders`: obtém as posições dos pokémons defensores no padrão (X,Y), onde X é a coluna (localização de defesa fixa) e Y é a linha (caminho de ataque).

`getturn x`: obtém as posições dos atacantes e seu respectivo identificador para cada arena de batalha (linhas) no turno x indicado. O primeiro turno é indexado por 0, ou seja, deve executar inicialmente o comando "getturn 0".

`shot x y id`: faz o pokémon na posição **(x,y)** bloquear o atacante identificado por **id**. Recebe 0 caso essa ação for permitida e 1, caso contrário.

`gameover`: obtém os status finais do jogo, como número atacantes destruídos, número de atacantes vitoriosos e tempo gasto no jogo.

`quit`: encerra execução tanto do cliente como do servidor.

Mais informações podem ser encontradas no arquivo [tp2_doc.pdf](https://github.com/jacksonns/UDP_Poke_Battle/blob/main/tp2_doc.pdf).

