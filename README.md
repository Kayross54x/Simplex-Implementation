# Simplex-Implementation
Implementação do método Simplex como trabalho prático desenvolvido para a disciplina de Pesquisa Operacional.

O objetivo deste trabalho é resolver Programações Lineares (PLs) gerais, a serem fornecidas no formato especificado abaixo. 
Em outras palavras, vamos fazer uma aplicação do método simplex, que irá resolver programações lineares definidas por:

MAX cT * x

ST Ax = b

x >= 0

Encontre o certificado que comprove seu resultado.

O programa irá ler o nome do arquivo por argv.

Entrada

A primeira linha da entrada contém dois inteiros 𝑛 e 𝑚 que representam o número de restrições e de variáveis, respectivamente.
A segunda linha contém 𝑚 m inteiros, 𝑐𝑖, que formam o vetor de custo.

Cada uma das 𝑛 linhas seguintes contém 𝑚+1 valores inteiros que representam as restrições. Para a i-ésima linha, os 𝑚 primeiros valores são 𝑎𝑖1, 𝑎𝑖2, …, 𝑎𝑖𝑚, enquanto o último valor é 𝑏𝑖. 
Note que esses valores, incluindo 𝑏𝑖, podem ser negativos.

1 ≤ 𝑛 ≤ 100

1 ≤ 𝑚 ≤ 100

Saídas:

Escreva o resultado da programação linear de acordo com as especificações seguintes:

Caso em que a PL possui valor ótimo:
  Na primeira linha, escreva "ótima".
  Na segunda linha, escreva o valor objetivo atingido.
  Na terceira linha, escreva uma solução que atinja o valor máximo.
  Na quarta linha, escreva um certificado de optimalidade.
  
Caso em que a PL é inviável:
  Na primeira linha, escreva "inviável".
  Na segunda linha, escreva um certificado de inviabilidade.
  
Caso em que a PL é ilimitada:
  Na primeira linha, escreva "ilimitada".
  Na segunda linha, escreva uma solução viável.
  Na terceira linha, escreva um certificado de ilimitabilidade.
  
Todos os números devem ser escritos com, no máximo, 3 casas decimais.
