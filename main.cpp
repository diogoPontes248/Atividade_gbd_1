#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class MeuArquivo {
public:
    struct cabecalho { int quantidade; int disponivel; } cabecalho;
    struct registro { int tamanho; char status; char palavra[50]; } registro;

    // construtor: abre arquivo. Essa aplicacao deveria ler o arquivo se existente ou criar um novo.
    // Entretando recriaremos o arquivo a cada execucao ("wb+").
    MeuArquivo() {
        fd = fopen("dados.dat","wb+");
        cabecalho.quantidade = 0;
        cabecalho.disponivel = 0;
        atualizaCabecalho();
    }

    // Destrutor: fecha arquivo
    ~MeuArquivo() {
        fclose(fd);
    }

    // Insere uma nova palavra, consulta se há espaco disponível ou se deve inserir no final
    void inserePalavra(char *palavra) {
        palavra[strcspn(palavra, "\n")] = '\0';

        strcpy(registro.palavra, palavra);
        registro.tamanho = strlen(palavra);
        registro.status = 'P';  //status: presente

        if(cabecalho.disponivel > 0){
            int proximoDisponivel;

            fseek(fd, cabecalho.disponivel, SEEK_SET);

            //Apos um registro ser removido, o primeiro campo da struct conterá o offset do proximo registro disponível
            //Caso não tenha próximo, retorna 0 no cabecalho.disponivel e na próxima inserção esta função irá para o else.
            fread(&proximoDisponivel, sizeof(int), 1, fd);

            fseek(fd, cabecalho.disponivel, SEEK_SET);
            fwrite(&registro, sizeof(registro), 1, fd);
            cabecalho.disponivel = proximoDisponivel;
            fflush(fd);
        }
        else{
            fseek(fd, 0, SEEK_END);
            fwrite(&registro, sizeof(registro), 1, fd);
            fflush(fd);
        }
        cabecalho.quantidade++;
        atualizaCabecalho();

    }

    // Marca registro como removido, atualiza lista de disponíveis, incluindo o cabecalho
    void removePalavra(int offset) {
        fseek(fd, offset, SEEK_SET);
        fread(&registro, sizeof(registro), 1, fd);

        registro.tamanho = cabecalho.disponivel;
        registro.status = 'A'; //status: ausente
        fseek(fd, offset, SEEK_SET);
        fwrite(&registro, sizeof(registro), 1, fd);

        cabecalho.quantidade--;
        cabecalho.disponivel = offset;
        atualizaCabecalho();
    }

    // BuscaPalavra: retorno é o offset para o registro
    // Nao deve considerar registro removido
    int buscaPalavra(char *palavra) {
        fseek(fd, sizeof(cabecalho), SEEK_SET);
        int posicao = sizeof(cabecalho);
        while(fread(&registro, sizeof(registro), 1, fd)){
            if(strcmp(palavra, registro.palavra) == 0 && registro.status == 'P'){
                return posicao;
            }
            posicao += sizeof(registro);
        }
        // retornar -1 caso nao encontrar
        return -1;
    }

private:
    // descritor do arquivo é privado, apenas métodos da classe podem acessá-lo
    FILE *fd;

    void atualizaCabecalho(){
        fseek(fd, 0, SEEK_SET);
        fwrite(&cabecalho, sizeof(cabecalho), 1, fd);
        fflush(fd);
    }
};

int main(int argc, char** argv) {
    // abrindo arquivo dicionario.txt
    FILE *f = fopen("dicionario.txt","rt");

    // se não abriu
    if (f == NULL) {
        printf("Erro ao abrir arquivo.\n\n");
        return 0;
    }

    char *palavra = new char[50];

    // criando arquivo de dados
    MeuArquivo *arquivo = new MeuArquivo();

    while (fgets(palavra,50,f) != NULL) {
        arquivo->inserePalavra(palavra);
    }

    // fechar arquivo dicionario.txt
    fclose(f);

    printf("Arquivo criado.\n\n");

    char opcao;
    do {
        printf("\n\n1-Insere\n2-Remove\n3-Busca\n4-Sair\nOpcao:");
        opcao = getchar();
        if (opcao == '1') {
            printf("Palavra: ");
            scanf("%s",palavra);
            arquivo->inserePalavra(palavra);
        }
        else if (opcao == '2') {
            printf("Palavra: ");
            scanf("%s",palavra);
            int offset = arquivo->buscaPalavra(palavra);
            if (offset >= 0) {
                arquivo->removePalavra(offset);
                printf("Removido.\n\n");
            }
        }
        else if (opcao == '3') {
            printf("Palavra: ");
            scanf("%s",palavra);
            int offset = arquivo->buscaPalavra(palavra);
            if (offset >= 0)
                printf("Encontrou %s na posição %d\n\n",palavra,offset);
            else
                printf("Não encontrou %s\n\n",palavra);
        }
        if (opcao != '4') opcao = getchar();
    } while (opcao != '4');

    printf("\n\nIsso eh tudo, pessoal!\n\n");

    return (EXIT_SUCCESS);
}
