#include <string.h>
#include <ctype.h>
#include "cafeteria.h"

//função para limpar o buffer e evitar erro
static void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

//Lê uma linha do teclado e tira o '\n' do final, se tiver.
// Retorna 1 se deu certo e 0 se deu erro na leitura.
int lerLinha(char *buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) {
        return 0;
    }

    int i = 0;
    while (buf[i] != '\0') {
        if (buf[i] == '\n') {
            buf[i] = '\0';
            break;
        }
        i++;
    }

    return 1;
}


//Produtos
Produto* carregarProdutos(const char *caminho) {
    FILE *f = fopen(caminho, "rb");
    if (!f) return NULL;

    Produto *lista = NULL, *fim = NULL;

    ProdutoDisk d;
    while (fread(&d, sizeof(ProdutoDisk), 1, f) == 1) {
        Produto *p = (Produto*)malloc(sizeof(Produto));
        if (!p) { fclose(f); liberarProdutos(lista); return NULL; }
        p->id = d.id;
        strncpy(p->nome, d.nome, sizeof(p->nome));
        p->nome[sizeof(p->nome)-1] = '\0';
        p->preco = d.preco;
        p->estoque = d.estoque;
        p->proximo = NULL;

        if (!lista) lista = p;
        else fim->proximo = p;
        fim = p;
    }
    fclose(f);
    return lista;
}

int salvarProdutos(const char *caminho, Produto *lista) {
    FILE *f = fopen(caminho, "wb");
    if (!f) {
        perror("Erro ao salvar produtos");
        return 0;
    }
    for (Produto *p = lista; p; p = p->proximo) {
        ProdutoDisk d;
        d.id = p->id;
        strncpy(d.nome, p->nome, sizeof(d.nome));
        d.nome[sizeof(d.nome)-1] = '\0';
        d.preco = p->preco;
        d.estoque = p->estoque;
        if (fwrite(&d, sizeof(ProdutoDisk), 1, f) != 1) {
            perror("Erro ao escrever produto");
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 1;
}

Produto* cadastrarProduto(Produto *lista, int *proximo_id_produto) {
    Produto *p = (Produto*)malloc(sizeof(Produto));
    if (!p) {
        perror("malloc");
        return lista;
    }
    p->id = (*proximo_id_produto)++;

    printf("\n--- Cadastro de Produto ---\n");
    printf("Nome: ");
    limparBuffer(); 
    if (!lerLinha(p->nome, sizeof(p->nome)) || p->nome[0] == '\0') {
        printf("Nome inválido.\n");
        free(p);
        return lista;
    }

    printf("Preço (ex.: 12.50): ");
    if (scanf("%f", &p->preco) != 1 || p->preco < 0.0f) {
        printf("Preço inválido.\n");
        free(p);
        limparBuffer();
        return lista;
    }

    printf("Estoque inicial: ");
    if (scanf("%d", &p->estoque) != 1 || p->estoque < 0) {
        printf("Estoque inválido.\n");
        free(p);
        limparBuffer();
        return lista;
    }

    p->proximo = NULL;

  //insere produto no final da lista
    if (!lista) {
        lista = p;
    } else {
        Produto *it = lista;
        while (it->proximo) it = it->proximo;
        it->proximo = p;
    }

    printf("Produto cadastrado! ID: %d | %s | R$ %.2f | Estoque: %d\n",
           p->id, p->nome, p->preco, p->estoque);
    return lista;
}

void listarProdutos(Produto *lista) {
    printf("\n=== Produtos ===\n");
    if (!lista) {
        printf("(vazio)\n");
        return;
    }
    printf("%-4s | %-30s | %-10s | %-7s\n", "ID", "Nome", "Preço", "Estoque");
    printf("---------------------------------------------------------------\n");
    for (Produto *p = lista; p; p = p->proximo) {
        printf("%-4d | %-30s | R$ %-8.2f | %-7d\n", p->id, p->nome, p->preco, p->estoque);
    }
}

Produto* buscarProdutoPorId(Produto *lista, int id) {
    for (Produto *p = lista; p; p = p->proximo)
        if (p->id == id) return p;
    return NULL;
}

void liberarProdutos(Produto *lista) {
    while (lista) {
        Produto *n = lista->proximo;
        free(lista);
        lista = n;
    }
}

//pedidos

Pedido* registrarPedido(Pedido *lista, Produto *lista_produtos, int *proximo_id_pedido) {
    if (!lista_produtos) {
        printf("\nNão há produtos cadastrados para vender.\n");
        return lista;
    }

    Pedido *novo = (Pedido*)malloc(sizeof(Pedido));
    if (!novo) {
        perror("malloc");
        return lista;
    }
    novo->id = (*proximo_id_pedido)++;
    novo->valor_total = 0.0f;

    printf("\n--- Registro de Pedido ---\n");
    printf("Nome do Cliente: ");
    limparBuffer();
    if (!lerLinha(novo->cliente, sizeof(novo->cliente)) || novo->cliente[0] == '\0') {
        printf("Cliente inválido.\n");
        free(novo);
        return lista;
    }

    //loop de itens: escolhe produto e quantidade; dá baixa no estoque
    while (1) {
        int id_prod = -1, qtd = 0;

        listarProdutos(lista_produtos);
        printf("\nInforme o ID do produto (0 para finalizar): ");
        if (scanf("%d", &id_prod) != 1) {
            printf("Entrada inválida.\n");
            limparBuffer();
            continue;
        }
        if (id_prod == 0) break;

        Produto *p = buscarProdutoPorId(lista_produtos, id_prod);
        if (!p) {
            printf("Produto ID %d não encontrado.\n", id_prod);
            continue;
        }

        printf("Quantidade para '%s' (Estoque atual: %d): ", p->nome, p->estoque);
        if (scanf("%d", &qtd) != 1 || qtd <= 0) {
            printf("Quantidade inválida.\n");
            limparBuffer();
            continue;
        }
        if (qtd > p->estoque) {
            printf("Estoque insuficiente. Disponível: %d.\n", p->estoque);
            continue;
        }

      //da baixa no estoque
        p->estoque -= qtd;
        float subtotal = p->preco * qtd;
        novo->valor_total += subtotal;

        printf("%d x %s adicionados. Subtotal: R$ %.2f | Estoque restante: %d\n",
               qtd, p->nome, subtotal, p->estoque);
    }

    if (novo->valor_total <= 0.0f) {
        printf("Nenhum item inserido. Pedido cancelado.\n");
        free(novo);
        return lista;
    }

    //insere pedido no início da
    novo->proximo = lista;
    lista = novo;

    printf("\nPedido registrado! ID: %d | Cliente: %s | Total: R$ %.2f\n",
           novo->id, novo->cliente, novo->valor_total);


    return lista;
}

void listarPedidos(Pedido *lista) {
    printf("\n=== Pedidos ===\n");
    if (!lista) {
        printf("(vazio)\n");
        return;
    }
    printf("%-4s | %-30s | %-10s\n", "ID", "Cliente", "Total");
    printf("-----------------------------------------------\n");
    for (Pedido *p = lista; p; p = p->proximo) {
        printf("%-4d | %-30s | R$ %-8.2f\n", p->id, p->cliente, p->valor_total);
    }
}

void liberarPedidos(Pedido *lista) {
    while (lista) {
        Pedido *n = lista->proximo;
        free(lista);
        lista = n;
    }
}
