#ifndef CAFETERIA_H
#define CAFETERIA_H

#include <stdio.h>
#include <stdlib.h>


//estrutura de produto
typedef struct Produto {
    int id;
    char nome[50];
    float preco;
    int estoque;
    struct Produto *proximo;
} Produto;

//estrutura de pedido
typedef struct Pedido {
    int id;
    char cliente[50];
    float valor_total;
    struct Pedido *proximo;
} Pedido;


//estrutura para guardar no arquivo
typedef struct {
    int id;
    char nome[50];
    float preco;
    int estoque;
} ProdutoDisk;


Produto* carregarProdutos(const char *caminho);
int      salvarProdutos(const char *caminho, Produto *lista);
Produto* cadastrarProduto(Produto *lista, int *proximo_id_produto);
void     listarProdutos(Produto *lista);
Produto* buscarProdutoPorId(Produto *lista, int id);
void     liberarProdutos(Produto *lista);

Pedido*  registrarPedido(Pedido *lista, Produto *lista_produtos, int *proximo_id_pedido);
void     listarPedidos(Pedido *lista);
void     liberarPedidos(Pedido *lista);

#endif
