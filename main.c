#include <stdio.h>
#include <stdlib.h>
#include "cafeteria.h"

//arquivo para guardar produtos
#define ARQ_PRODUTOS "produtos.dat"

static void pausar() {
    printf("\nPressione ENTER para continuar...");
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
    Produto *lista_produtos = carregarProdutos(ARQ_PRODUTOS);
    Pedido  *lista_pedidos  = NULL;

    //função para gerar o id, incrementando +1 sequencialmente
    int prox_id_prod = 1;
    for (Produto *p = lista_produtos; p; p = p->proximo)
        if (p->id >= prox_id_prod) prox_id_prod = p->id + 1;

    int prox_id_ped = 1;

    int opc = -1;
    do {
        printf("\n====================\n");
        printf("  CAFETERIA - MENU  \n");
        printf("====================\n");
        printf("1) Cadastrar produto\n");
        printf("2) Listar produtos\n");
        printf("3) Registrar pedido\n");
        printf("4) Listar pedidos\n");
        printf("5) Salvar produtos\n");
        
        printf("0) Sair\n");
        printf("Escolha: ");

        if (scanf("%d", &opc) != 1) {
            printf("Entrada inválida.\n");
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            continue;
        }

        switch (opc) {
            case 1:
                lista_produtos = cadastrarProduto(lista_produtos, &prox_id_prod);
                pausar();
                break;
            case 2:
                listarProdutos(lista_produtos);
                pausar();
                break;
            case 3:
                lista_pedidos = registrarPedido(lista_pedidos, lista_produtos, &prox_id_ped);
                pausar();
                break;
            case 4:
                listarPedidos(lista_pedidos);
                pausar();
                break;
            case 5:
                if (salvarProdutos(ARQ_PRODUTOS, lista_produtos))
                    printf("Produtos salvos em \"%s\".\n", ARQ_PRODUTOS);
                else
                    printf("Falha ao salvar produtos.\n");
                pausar();
                break;
            case 0:
                /* salva automaticamente ao sair */
                if (salvarProdutos(ARQ_PRODUTOS, lista_produtos))
                    printf("\nProdutos salvos. Encerrando...\n");
                else
                    printf("\nNão foi possível salvar os produtos ao sair.\n");
                break;
            default:
                printf("Opção inválida.\n");
                pausar();
        }
    } while (opc != 0);

    liberarPedidos(lista_pedidos);
    liberarProdutos(lista_produtos);
    
    return 0;
}
