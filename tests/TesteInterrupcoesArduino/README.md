# TesteInterrupcoesArduino

Este repositório contém um conjunto de projetos cujo objetivo é testar o consumo de energia do Arduino em duas situações distintas:

* Em modo de operação normal, com um loop principal executando todo o tempo e;

* Em modo de operação reduzida usando interrupções

Neste segundo modo de operação, a CPU e demais circuitos do são colocados para dormir até que um pulso seja gerado em uma das portas de interrupção do Arduino, acordando assim a CPU.

O esquemático do circuito, bem como fotos e/ou vídeos serão acrescentados em breve.

Para rodar estes projetos sem nenhum trabalho adicional, recomenda-se utilizar o seguinte conjunto de softwares:

* [IDE do Arduino na versão 1.6](https://www.arduino.cc/en/Main/Software) e;

* [Visual Studio 2015](https://www.visualstudio.com/pt-br/products/visual-studio-community-vs.aspx) em conjunto com o plugin [Visual Micro para Visual Studio](https://visualstudiogallery.msdn.microsoft.com/069a905d-387d-4415-bc37-665a5ac9caba/)
