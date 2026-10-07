#include <sys/types.h>
#include <sys/stat.h>

#include "btree.h"

#ifndef _BTREE_CPP
#define	_BTREE_CPP

bool fileExists(const char *filename) { struct stat statBuf; if (stat(filename,&statBuf) < 0) return false; return S_ISREG(statBuf.st_mode); }

btree::btree() {
    char nomearquivo[20] = "arvoreb.dat";

    // se arquivo ja existir, abrir e carregar cabecalho
    if (fileExists(nomearquivo)) {
        // abre arquivo
        arquivo = fopen(nomearquivo,"r+");
        leCabecalho();
    }
    // senao, criar novo arquivo e salvar o cabecalho
    else {
        // cria arquivo
        arquivo = fopen(nomearquivo,"w+");

        // atualiza cabecalho
        cabecalhoArvore.numeroElementos = 0;
        cabecalhoArvore.paginaRaiz = -1;
        salvaCabecalho();
    }
}

btree::~btree() {
    // fechar arquivo
    fclose(arquivo);
}

int btree::computarTaxaOcupacao() {
    return 0;
}

void btree::insereChave(int chave, int valor) {

    insereRecursivo(chave, valor, cabecalhoArvore.paginaRaiz, 1);
}

void btree::insereRecursivo(int chave, int valor, int pagina, int nivel) {

    // verificar se raiz existe!

    // chamada recursiva incrementando nível até chegar na folha (quando nivel == cabecalho.alturaArvore)

    // na volta da recursão, situações:
    // 1) chave/valor inserido na pagina sem alterar maior valor da página
    // 2) overflow, divisão da pagina, para a página existente atualizar a maior chave e para
    //    a nova pagína, armazenar o identificador da página e a maior chave

    // cabecalho contem o numero da pagina raiz

    // se (cabecalhoArvore.paginaRaiz == 1) entao raiz eh a unica pagina. Ler raiz, inserir e salvar. Senao...
            // Exemplo:
            // pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
            // adicionar <chave,valor> na pagina pg
            // salvar pagina: salvaPagina(pg->numeroPagina, pg);

    // senao...

    // ler pagina raiz: pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);

    // se inserir, atualizar cabecalho
    cabecalhoArvore.numeroElementos++;
    salvaCabecalho();
}

void btree::removeChave(int chave) {
	// neste trabalho, não é necessário implementar a remoção!

    // se remover, atualizar cabecalho
    if (true) {
        cabecalhoArvore.numeroElementos--;
        salvaCabecalho();
    }
}

int btree::buscaChave(int chave) {
    if ( cabecalhoArvore.numeroElementos == 0 || cabecalhoArvore.paginaRaiz == -1 ) return -1; // Trativa para o caso da árvore for vazia

    pagina * C = lePagina(cabecalhoArvore.paginaRaiz);               // C = nó raiz
    int alturaAtualArvore = 1;

    while ( alturaAtualArvore < cabecalhoArvore.alturaArvore ) {     // Enquanto C não for um nó folha
        int i = 0;

        while (i < C -> numeroElementos && chave > C -> chaves[i]) { // i = menor número tal que v <= Ki
            i++;
        }

        int proximaPagina;

        if (i == C -> numeroElementos) {
            proximaPagina = C -> valores[C -> numeroElementos];       // Próxima página é definida como o último ponteiro não nulo no nó
        }
        else if ( chave == C -> chaves[i]) {
            proximaPagina = C -> valores [i+1];                       // Próxima página é definida como o registro seguinte
        }else {
            proximaPagina = C -> valores [i];                         // Próxima página é definida como a posição correspondente
        }

        pagina * aux = lePagina(proximaPagina);
        delete C;                                                     // Libera a memória da página atual
        C = aux;

        alturaAtualArvore++;
    }
    for ( int i = 0; i < C -> numeroElementos; i++ ) {                // Se para algum elemento de C, Ki == v, portanto retorna Pi
        if ( C -> chaves[i] == chave) {
            int valor = C -> valores[i];
            delete C;                                                 // Libera a memória antes de retornar
            return valor;
        }
    }
    delete C;                                                          // Libera a memória do nó folha
    return -1;                                                         // Não existe um registro com o valor de chave forneceido
}

#endif	/* _BTREE_CPP */
