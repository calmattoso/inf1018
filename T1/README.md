/* Carlos\_Augusto\_Lima\_Mattoso 1210553 */

Relatório de Testes
===================
  O trabalho foi testado através de uma sequência de testes 
manuais para cada uma das funções pedidas. Uma função `big_dump`
foi feita para exibir os **BigInt**s e assim avaliar se os resultados
estavam corretos.
  O programa de testes recebia dois números, e realizava uma 
série de operações sobre eles para verificar se tudo dava certo.

**Obs**: As funções que executam operações sobre dois **BigInts**
`a` e `b`, retornando o resultado em `res`, podem ser usadas para fazer
`a = a operacao b`, da seguinte forma:  `OP(a, a, b)`. O formato de
`OP(b, a, b)`  também é suportado.

## 1) big\_val && big\_uval ##
  
Valores testados -- **ambas**

	-1, 1, 2, -2, 3, 255, 2147483647, -2147483648

Os números foram gerados com os valores esperados. No caso
dos negativos, o sinal foi replicado como esperado.

## 2) big\_sum && big\_sub ##

Valores testados - `big_sum`


    ( -1 +  1) ->   0, teste de overflow, ok!
    ( -1 +  2) ->   1, teste de overflow, ok!
    (  2 +  3) ->   5, ok!
    ( -1 + -1) ->  -2, teste de soma de negativos, ok!
    (255 +  1) -> 256, ok!
 
Valores testados - `big_sub`

Para o caso da `big_sub`, o mesmo foi feito. A `big_sub`é
simplesmente a soma do primeiro número com o inverso do
segundo. Criou-se uma função auxiliar `big_minus` para negativar o
segundo número.

    (-1 - -1) ->  0, teste de inversão de negativo, ok!
    (-1 -  1) -> -2, ok!
    (-1 -  2) -> -3, teste de inversão de positivos, ok!
    (2  -  3) -> -1, ok
    (2147483647 -2147483648) -> 4294967295, ok!

## 3) big\_mul & big\_umul ##

O algoritmo implementado segue o algoritmo tradicional de 
multiplicação, *shiftando* o `a` para a esquerda, e somando-o à
resposta sempre que se encontrar um 1 no `b`. O processo de 
*shifts* foi otimizado, só ocorrendo todos os *shifts* necessários
ao encontrarmos um 1 em `b`.
  
O algoritmo de `big_umul`, também funciona para valores *signed*.

Valores testados (os negativos para caso *signed*)

    (-1 * -1) ->  1, ok!
    (-1 * -2) ->  2, ok!
    (-1 *  2) -> -2, ok! 

   `big_mul` ou `big_umul` abaixo

    (3  *  2) ->  6, ok!
    (2147483647 * 2) -> 4294967294, ok!

## 4) big\_cmp & big\_ucmp ##

Valores testados -- `big_cmp`
  
    (-1  cmp -1) -> ==, ok!
    (-1  cmp -2) ->  >, ok!
    (-2  cmp -1) ->  <, ok!
    (-1  cmp  2) ->  <, ok!
    (2   cmp  3) ->  <, ok!
    (2   cmp  2) -> ==, ok!
    (2147483647 cmp 2147483647) -> ==, ok
  
Valores testados -- `big_ucmp`
  
    (-1 ucmp  2) ->   >, ok!
    (-1 ucmp -1) ->  ==, ok!
    (2  ucmp -1) ->   <, ok!

    (3  ucmp  2) ->   >, ok!
    (2  ucmp  2) ->  ==, ok!
    (2  ucmp  3) ->   <, ok!
        

## 5) big\_shl & big\_shr ##

Um detalhe importante é ressaltar que caso o número de bits a
serem *shiftados* seja negativo, **a operação contrária é executada**.
Isto é, para um dado **BigInt** `a` fazer:

- `big_shr(res, a, n)`, **n < 0** --> é feito `big_shl(res, a, -n)`
- `big_shl(res, a, n)`, **n < 0** --> é feito `big_shr(res, a, -n)`

Valores testados -- `big_shl`

    (16 shl 0)    ->   16, ok! -- caso limite
    (16 shl 128)  ->    0, ok! -- caso limite
    (16 shl 5813) ->    0, ok! -- caso limite
    (16 shl 2)    ->   64, ok!
    (16 shl 9)    -> 8192, ok!

    (16 shl -1)   ->    8, ok! 
    ( 8 shl -1)   ->    4, ok! 

    (-1 shl 1)    -> -2, ok! -- teste de negativo

Valores testados -- `big_shr`
  
    (  16 shr 0)    -> 16, ok! -- caso limite
    (  16 shr 128)  ->  0, ok! -- caso limite
    (  16 shr 5813) ->  0, ok! -- caso limite
    (  16 shr 2)    ->  4, ok!
    (8192 shr 9)    -> 16, ok!

    (16 shr -1)   ->  32, ok! 
    ( 8 shr -1)   ->  16, ok! 
    
    (-1 shr 1)    -> 0x7f no byte mais 
                     significativo, ok!

----------





