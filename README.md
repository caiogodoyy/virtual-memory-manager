# Virtual Memory Management
Este projeto consiste em escrever um programa que traduza endereço lógico para físico para um espaço de endereço virtual de tamanho 65.536 bytes. Seu programa lerá de um arquivo contendo endereços lógicos e, usando um TLB e uma page table, irá traduzir cada endereço lógico para seu endereço físico correspondente e imprimir o valor do byte armazenado no endereço físico traduzido. Seu objetivo de aprendizado é usar a simulação para entender as etapas envolvidas na traduzindo endereços lógicos para físicos. Isso incluirá a resolução de falhas de página usando paginação por demanda, gerenciando um TLB e implementando um algoritmo de  substituição de página.

- CESAR School
- Software Insfrastructure (Operating Systems)
- Professor Erico Teixeira
- Reference: Operating System Concepts 10th Edition, by Abraham Silberschatz et al (Chapter 10)

## Comandos
+ Clonar o repositório
```bash
git clone https://github.com/caiogodoyy/virtual-memory-manager
cd virtual-memory-manager/
```
+ Compilar:
```bash
make
```
+ Executar:
```bash
make run
```
+ Limpar após uso:
```bash
make clean
```

<div align="center">
<img src="https://media.giphy.com/media/1U4S8219ByoGk/giphy.gif" width="200px"/>
</div>
