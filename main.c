/*
 * HORROR CLI - O ESPELHO DA CULPA
 * 
 * Jogo de terror psicologico baseado em texto
 * Desenvolvido por: Felipe Correa Soares
 * Disciplina: Algoritmos e Programacao de Computadores - IESB
 * 
 */

#define _DEFAULT_SOURCE  // Para habilitar usleep() - deve vir antes dos includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>   // Para time() usado em srand()

// Configuracao para cross-platform
#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_SEC(x) Sleep((x) * 1000)
    #define SLEEP_MS(x) Sleep(x)
#else
    #include <unistd.h>  // Isso inclui sleep() e usleep()
    #define SLEEP_SEC(x) sleep(x)
    #define SLEEP_MS(x) usleep((x) * 1000)
#endif

#define MAX_INPUT 100
#define MAX_DESCRIPTION 500
#define MAX_ITEMS 10

// Estrutura para itens
typedef struct {
    char nome[MAX_INPUT];
    char descricao[MAX_DESCRIPTION];
    int coletavel;
    int usado;
    int revelacao; // 0 = nenhuma, 1 = memoria, 2 = culpa, 3 = verdade
} Item;

// Estrutura para salas
typedef struct {
    char nome[MAX_INPUT];
    char descricao[MAX_DESCRIPTION];
    char descricao_curta[MAX_INPUT];
    char descricao_sanidade_baixa[MAX_DESCRIPTION]; // Descricao alternativa com sanidade baixa
    int conexoes[4];  // norte, leste, sul, oeste (-1 se nao houver conexao)
    Item* itens[MAX_ITEMS];
    int num_itens;
    int visitada;
    int revelada; // 0 = normal, 1 = revelada apos evento
} Sala;

// Estrutura para o estado do jogo
typedef struct {
    Sala salas[10];
    Item itens[30]; // Aumentado para mais itens
    Item* inventario[MAX_ITEMS];
    int num_inventario;
    char playerName[MAX_INPUT];
    int currentRoom;
    int sanity;
    int desejos_feitos;
    int gameRunning;
    int memorias_recuperadas; // Contador de memorias
    int escolha_final; // Para rastrear escolhas morais
    char ultimo_desejo[MAX_INPUT]; // Armazena o ultimo desejo feito
} GameState;

int main() {
    // Inicializar o gerador de numeros aleatorios
    srand(time(NULL));
    
    int opcao_menu = 0;
    
    // Loop principal do menu
    while (opcao_menu != 4) {
        // Limpar tela
        system("clear || cls");
        printf("---------------------\n");
        printf("|   MENU DE JOGOS   |\n");
        printf("---------------------\n");
        printf("|                   |\n");
        printf("|                   |\n");
        printf("| 1 - O Espelho     |\n");
        printf("| 2 - Ver Score     |\n");
        printf("| 3 - Sobre         |\n");
        printf("| 4 - Sair          |\n");
        printf("|                   |\n");
        printf("|                   |\n");
        printf("---------------------\n");
        printf("Escolha uma opcao:\n> ");
        scanf("%i",&opcao_menu);
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        switch(opcao_menu) {
            case 1: {
                // === INÍCIO DO JOGO DE TERROR ===
                system("clear || cls");
                
                // Variáveis do jogo
                GameState game;
                char input[MAX_INPUT];
                
                // === TELA DE TÍTULO ===
                printf("\n\n\n");
                SLEEP_SEC(1);
                
                // Função displayWithDelay inline - ASCII Art do título
                char* titulo[] = {
                    "  _____ _____ ____  ____   ___  ____  ",
                    " |_   _| ____|  _ \\|  _ \\ / _ \\|  _ \\ ",
                    "   | | |  _| | |_) | |_) | | | | |_) |",
                    "   | | | |___|  _ <|  _ <| |_| |  _ < ",
                    "   |_| |_____|_| \\_\\_| \\_\\\\___/|_| \\_\\",
                    "",
                    "  ____  ____ ___ ____ ___  _     ___   ____ ___ ____ ___  ",
                    " |  _ \\/ ___|_ _/ ___/ _ \\| |   / _ \\ / ___|_ _/ ___/ _ \\ ",
                    " | |_) \\___ \\| | |  | | | | |  | | | | |  _ | | |  | | | |",
                    " |  __/ ___) | | |__| |_| | |__| |_| | |_| || | |__| |_| |",
                    " |_|   |____/___\\____\\___/|_____\\___/ \\____|___\\____\\___/ "
                };
                
                for(int linha = 0; linha < 11; linha++) {
                    if(linha == 5) {
                        printf("\n");
                        SLEEP_SEC(1);
                        continue;
                    }
                    
                    int i = 0;
                    int max_chars = 1000;
                    while (titulo[linha][i] != '\0' && i < max_chars) {
                        putchar(titulo[linha][i++]);
                        fflush(stdout);
                        SLEEP_MS(15);
                    }
                    printf("\n");
                }
                
                printf("\n\n");
                SLEEP_SEC(1);
                
                // Título do jogo
                char* subtitulo = "                  O  E S P E L H O  D A  C U L P A";
                char* linha_sep = "                 ===================================";
                
                int i = 0;
                while (subtitulo[i] != '\0' && i < 1000) {
                    putchar(subtitulo[i++]);
                    fflush(stdout);
                    SLEEP_MS(30);
                }
                printf("\n");
                
                i = 0;
                while (linha_sep[i] != '\0' && i < 1000) {
                    putchar(linha_sep[i++]);
                    fflush(stdout);
                    SLEEP_MS(30);
                }
                printf("\n\n\n");
                SLEEP_SEC(1);
                
                // Mensagem para começar
                char* msg_start = "Pressione ENTER para comecar...";
                i = 0;
                while (msg_start[i] != '\0' && i < 1000) {
                    putchar(msg_start[i++]);
                    fflush(stdout);
                    SLEEP_MS(30);
                }
                printf("\n");
                
                // Aguardar que o jogador pressione Enter
                getchar();
                
                // === INICIALIZAÇÃO DO JOGO (inline) ===
                
                // Inicializar estado do jogo
                game.currentRoom = 0;
                game.sanity = 100;
                game.desejos_feitos = 0;
                game.gameRunning = 1;
                game.num_inventario = 0;
                game.memorias_recuperadas = 0;
                game.escolha_final = 0;
                strcpy(game.ultimo_desejo, "");
                
                // === INICIALIZAÇÃO DAS SALAS ===
                
                // Quarto inicial
                strcpy(game.salas[0].nome, "Quarto Escuro");
                strcpy(game.salas[0].descricao, "Um quarto escuro e empoeirado. Ha uma porta trancada que parece levar a um corredor e uma janela embacada. Uma cama antiga esta no canto. Voce sente uma presenca estranha.");
                strcpy(game.salas[0].descricao_curta, "Quarto escuro e empoeirado com uma porta e uma janela.");
                strcpy(game.salas[0].descricao_sanidade_baixa, "O quarto pulsa com vida propria. As paredes respiram lentamente e sussurros incompreensiveis ecoam de todos os cantos. A cama range sozinha, como se alguem invisivel estivesse deitado nela.");
                game.salas[0].conexoes[0] = -1;  // norte
                game.salas[0].conexoes[1] = -1;  // leste
                game.salas[0].conexoes[2] = -1;  // sul
                game.salas[0].conexoes[3] = -1;  // oeste
                game.salas[0].num_itens = 0;
                game.salas[0].visitada = 0;
                game.salas[0].revelada = 0;
                
                // Corredor
                strcpy(game.salas[1].nome, "Corredor Sombrio");
                strcpy(game.salas[1].descricao, "Um corredor longo e sombrio. O papel de parede esta descascando. Ha uma porta ao norte que parece levar a uma sala de estar, e uma porta ao sul de onde voce veio. Voce nota uma escada descendo para o leste e outra subindo para o oeste.");
                strcpy(game.salas[1].descricao_curta, "Corredor sombrio com papel de parede descascando.");
                strcpy(game.salas[1].descricao_sanidade_baixa, "O corredor se estende infinitamente em ambas as direcoes. Pegadas sangrentas aparecem e desaparecem no chao. Voce ouve passos que ecoam os seus, mas sempre um segundo atrasados.");
                game.salas[1].conexoes[0] = 2;   // norte (sala de estar)
                game.salas[1].conexoes[1] = 3;   // leste (porão)
                game.salas[1].conexoes[2] = 0;   // sul (quarto inicial)
                game.salas[1].conexoes[3] = 4;   // oeste (sótão)
                game.salas[1].num_itens = 0;
                game.salas[1].visitada = 0;
                game.salas[1].revelada = 0;
                
                // Sala de estar
                strcpy(game.salas[2].nome, "Sala de Estar");
                strcpy(game.salas[2].descricao, "Uma sala de estar antiga com moveis cobertos de poeira. Ha uma lareira fria e uma poltrona velha. Uma mesa de centro de madeira esta no meio da sala. Um retrato antigo na parede parece te observar.");
                strcpy(game.salas[2].descricao_curta, "Sala de estar empoeirada com lareira fria.");
                strcpy(game.salas[2].descricao_sanidade_baixa, "Os moveis estao manchados de sangue seco. O retrato na parede agora mostra seu rosto contorcido em agonia. A lareira crepita com chamas negras que nao emitem calor, apenas frio mortal.");
                game.salas[2].conexoes[0] = -1;  // norte
                game.salas[2].conexoes[1] = -1;  // leste
                game.salas[2].conexoes[2] = 1;   // sul (corredor)
                game.salas[2].conexoes[3] = -1;  // oeste
                game.salas[2].num_itens = 0;
                game.salas[2].visitada = 0;
                game.salas[2].revelada = 0;
                
                // Porão
                strcpy(game.salas[3].nome, "Porao Umido");
                strcpy(game.salas[3].descricao, "Voce desce as escadas rangentes ate um porao escuro e umido. O cheiro de mofo e sufocante. Caixas antigas estao empilhadas nos cantos, e voce ouve o som de agua pingando em algum lugar. Uma porta de madeira apodrecida leva para o sul.");
                strcpy(game.salas[3].descricao_curta, "Porao umido com caixas antigas.");
                strcpy(game.salas[3].descricao_sanidade_baixa, "O porao esta inundado com sangue ate os tornozelos. Corpos em decomposicao flutuam entre as caixas. Voce reconhece alguns rostos - sao todos voce, em diferentes estagios de morte.");
                game.salas[3].conexoes[0] = -1;  // norte
                game.salas[3].conexoes[1] = -1;  // leste
                game.salas[3].conexoes[2] = 6;   // sul (jardim)
                game.salas[3].conexoes[3] = 1;   // oeste (volta pro corredor)
                game.salas[3].num_itens = 0;
                game.salas[3].visitada = 0;
                game.salas[3].revelada = 0;
                
                // Sótão
                strcpy(game.salas[4].nome, "Sotao Empoeirado");
                strcpy(game.salas[4].descricao, "O sotao e sufocante e cheio de teias de aranha. Brinquedos infantis quebrados estao espalhados pelo chao. Uma janela circular permite que um raio de luz fraca entre, iluminando particulas de poeira dancando no ar.");
                strcpy(game.salas[4].descricao_curta, "Sotao com brinquedos quebrados.");
                strcpy(game.salas[4].descricao_sanidade_baixa, "Os brinquedos se movem sozinhos, chorando lagrimas de sangue. Uma boneca sem olhos sussurra seu nome repetidamente. As paredes estao cobertas com desenhos infantis macabros mostrando cenas de morte.");
                game.salas[4].conexoes[0] = -1;  // norte
                game.salas[4].conexoes[1] = 1;   // leste (volta pro corredor)
                game.salas[4].conexoes[2] = -1;  // sul
                game.salas[4].conexoes[3] = -1;  // oeste
                game.salas[4].num_itens = 0;
                game.salas[4].visitada = 0;
                game.salas[4].revelada = 0;
                
                // Quarto do Espelho (inicialmente bloqueado, aparece após 2º desejo)
                strcpy(game.salas[5].nome, "Quarto do Espelho");
                strcpy(game.salas[5].descricao, "Um quarto pequeno dominado por um enorme espelho ornamentado. O reflexo nao esta certo - mostra uma versao sua, mas diferente. Velas negras queimam sem consumir a cera.");
                strcpy(game.salas[5].descricao_curta, "Quarto com espelho gigante.");
                strcpy(game.salas[5].descricao_sanidade_baixa, "Infinitos reflexos seus te encaram do espelho, cada um mostrando uma morte diferente. O espelho sangra pelas bordas. Seu reflexo se move independentemente, sorrindo maliciosamente.");
                game.salas[5].conexoes[0] = -1;  // norte
                game.salas[5].conexoes[1] = -1;  // leste
                game.salas[5].conexoes[2] = -1;  // sul
                game.salas[5].conexoes[3] = 2;   // oeste (sala de estar - será conectado depois)
                game.salas[5].num_itens = 0;
                game.salas[5].visitada = 0;
                game.salas[5].revelada = 0;
                
                // Jardim dos Arrependimentos (área externa, conectada ao porão)
                strcpy(game.salas[6].nome, "Jardim dos Arrependimentos");
                strcpy(game.salas[6].descricao, "Um jardim abandonado sob um ceu eternamente nublado. Flores mortas pendem de seus caules. Lapides improvisadas marcam pequenos montes de terra. O ar esta pesado com o cheiro de decomposicao.");
                strcpy(game.salas[6].descricao_curta, "Jardim morto com lapides.");
                strcpy(game.salas[6].descricao_sanidade_baixa, "As flores mortas tem rostos humanos contorcidos em gritos silenciosos. Das sepulturas, maos esqueleticas emergem tentando te agarrar. Voce ouve o choro de uma crianca vindo de baixo da terra.");
                game.salas[6].conexoes[0] = 3;   // norte (porão)
                game.salas[6].conexoes[1] = -1;  // leste
                game.salas[6].conexoes[2] = -1;  // sul
                game.salas[6].conexoes[3] = -1;  // oeste
                game.salas[6].num_itens = 0;
                game.salas[6].visitada = 0;
                game.salas[6].revelada = 0;
                
                // === INICIALIZAÇÃO DOS ITENS ===
                
                // Pata de Macaco
                strcpy(game.itens[0].nome, "pata de macaco");
                strcpy(game.itens[0].descricao, "Uma pata de macaco ressecada e enrugada. Voce sente um arrepio ao olha-la. Dizem que concede tres desejos, mas a um preco terrivel.");
                game.itens[0].coletavel = 1;
                game.itens[0].usado = 0;
                game.itens[0].revelacao = 0;
                
                // Chave
                strcpy(game.itens[1].nome, "chave antiga");
                strcpy(game.itens[1].descricao, "Uma chave de metal enferrujada. Parece que poderia abrir uma porta antiga.");
                game.itens[1].coletavel = 1;
                game.itens[1].usado = 0;
                game.itens[1].revelacao = 0;
                
                // Fotografia
                strcpy(game.itens[2].nome, "fotografia");
                strcpy(game.itens[2].descricao, "Uma fotografia amarelada. Mostra voce ao lado de alguem cujo rosto foi borrado. Voce nao se lembra de quando foi tirada.");
                game.itens[2].coletavel = 1;
                game.itens[2].usado = 0;
                game.itens[2].revelacao = 1; // memória
                
                // Diário rasgado
                strcpy(game.itens[3].nome, "diario rasgado");
                strcpy(game.itens[3].descricao, "Paginas rasgadas de um diario. A caligrafia e sua, mas voce nao se lembra de ter escrito: '...nao posso viver com o que fiz... a crianca... foi minha culpa... a pata me enganou...'");
                game.itens[3].coletavel = 1;
                game.itens[3].usado = 0;
                game.itens[3].revelacao = 2; // culpa
                
                // Carta manchada
                strcpy(game.itens[4].nome, "carta manchada");
                strcpy(game.itens[4].descricao, "Uma carta manchada de lágrimas e... sangue? 'Meu amor, não posso mais suportar. Desde que nossa filha... Você usou aquela maldita pata. Eu te perdoo, mas não posso continuar...'");
                game.itens[4].coletavel = 1;
                game.itens[4].usado = 0;
                game.itens[4].revelacao = 3; // verdade
                
                // Boneca quebrada
                strcpy(game.itens[5].nome, "boneca quebrada");
                strcpy(game.itens[5].descricao, "Uma boneca de porcelana com o rosto rachado. Está vestida com um vestido rosa desbotado. Algo nela desperta uma dor profunda em seu peito. 'Ana' está bordado no vestido.");
                game.itens[5].coletavel = 1;
                game.itens[5].usado = 0;
                game.itens[5].revelacao = 1; // memória
                
                // Anel manchado
                strcpy(game.itens[6].nome, "anel de casamento");
                snprintf(game.itens[6].descricao, MAX_DESCRIPTION, 
                         "Um anel de ouro manchado com o que parece ser sangue seco. A inscricao le: 'Para sempre juntos - M & %s'. Voce nao se lembra de ter sido casado.", 
                         "Jogador"); // Será atualizado com nome real depois
                game.itens[6].coletavel = 1;
                game.itens[6].usado = 0;
                game.itens[6].revelacao = 2; // culpa
                
                // Espelho de mão
                strcpy(game.itens[7].nome, "espelho de mao");
                strcpy(game.itens[7].descricao, "Um pequeno espelho de mão ornamentado. O reflexo mostra não apenas seu rosto, mas sombras de pessoas atrás de você que desaparecem quando você se vira.");
                game.itens[7].coletavel = 1;
                game.itens[7].usado = 0;
                game.itens[7].revelacao = 0;
                
                // Chave do espelho (aparece após o primeiro desejo)
                strcpy(game.itens[8].nome, "chave ornamentada");
                strcpy(game.itens[8].descricao, "Uma chave dourada com detalhes intrincados. Parece pulsar com uma energia estranha. Voce sente que ela abre algo importante.");
                game.itens[8].coletavel = 1;
                game.itens[8].usado = 0;
                game.itens[8].revelacao = 0;
                
                // === DISTRIBUIR ITENS NAS SALAS ===
                
                // Chave no quarto inicial
                game.salas[0].itens[0] = &game.itens[1];
                game.salas[0].num_itens = 1;
                
                // Pata de macaco e fotografia na sala de estar
                game.salas[2].itens[0] = &game.itens[0];
                game.salas[2].itens[1] = &game.itens[2];
                game.salas[2].num_itens = 2;
                
                // Diário e anel no porão
                game.salas[3].itens[0] = &game.itens[3];
                game.salas[3].itens[1] = &game.itens[6];
                game.salas[3].num_itens = 2;
                
                // Boneca no sótão
                game.salas[4].itens[0] = &game.itens[5];
                game.salas[4].num_itens = 1;
                
                // Espelho de mão no quarto do espelho
                game.salas[5].itens[0] = &game.itens[7];
                game.salas[5].num_itens = 1;
                
                // Carta no jardim
                game.salas[6].itens[0] = &game.itens[4];
                game.salas[6].num_itens = 1;
                
                // Limpar a tela
                system("clear || cls");
                
                // Solicitar nome do jogador
                char* msg_nome = "Por favor, insira seu nome: ";
                i = 0;
                while (msg_nome[i] != '\0' && i < 1000) {
                    putchar(msg_nome[i++]);
                    fflush(stdout);
                    SLEEP_MS(40);
                }
                
                // Ler nome do jogador
                do {
                    fgets(game.playerName, MAX_INPUT, stdin);
                    game.playerName[strcspn(game.playerName, "\n")] = 0;
                    
                    if (strlen(game.playerName) == 0) {
                        char* msg_valido = "Por favor, digite um nome valido: ";
                        i = 0;
                        while (msg_valido[i] != '\0' && i < 1000) {
                            putchar(msg_valido[i++]);
                            fflush(stdout);
                            SLEEP_MS(40);
                        }
                    }
                } while (strlen(game.playerName) == 0);
                
                // Atualizar descrição do anel com nome do jogador
                snprintf(game.itens[6].descricao, MAX_DESCRIPTION, 
                         "Um anel de ouro manchado com o que parece ser sangue seco. A inscricao le: 'Para sempre juntos - M & %s'. Voce nao se lembra de ter sido casado.", 
                         game.playerName);
                
                // Introdução do jogo
                printf("\n");
                char* intro_msgs[] = {
                    "=== O ESPELHO DA CULPA ===",
                    "Voce acorda em um quarto escuro...",
                    "A cabeca doi e voce nao consegue lembrar como chegou aqui...",
                    "Digite 'ajuda' para ver os comandos disponiveis."
                };
                
                for(int msg_idx = 0; msg_idx < 4; msg_idx++) {
                    i = 0;
                    int delay = (msg_idx == 0) ? 50 : (msg_idx == 1 ? 70 : (msg_idx == 2 ? 60 : 50));
                    while (intro_msgs[msg_idx][i] != '\0' && i < 1000) {
                        putchar(intro_msgs[msg_idx][i++]);
                        fflush(stdout);
                        SLEEP_MS(delay);
                    }
                    printf("\n");
                    if(msg_idx < 3) SLEEP_SEC(1);
                }
                printf("\n");
                
                // Forçar a inicialização da sala atual
                game.salas[game.currentRoom].visitada = 0;
                
                // === LOOP PRINCIPAL DO JOGO ===
                while(game.gameRunning) {
                    // === MOSTRAR SALA (inline) ===
                    Sala* sala_atual = &game.salas[game.currentRoom];
                    
                    // Escolher descrição baseada na sanidade
                    if (game.sanity < 30 && strlen(sala_atual->descricao_sanidade_baixa) > 0) {
                        printf("\n");
                        i = 0;
                        while (sala_atual->descricao_sanidade_baixa[i] != '\0' && i < 1000) {
                            putchar(sala_atual->descricao_sanidade_baixa[i++]);
                            fflush(stdout);
                            SLEEP_MS(50);
                        }
                        printf("\n");
                    } else if (!sala_atual->visitada) {
                        printf("\n");
                        i = 0;
                        while (sala_atual->descricao[i] != '\0' && i < 1000) {
                            putchar(sala_atual->descricao[i++]);
                            fflush(stdout);
                            SLEEP_MS(40);
                        }
                        printf("\n");
                        sala_atual->visitada = 1;
                    } else {
                        printf("\n");
                        i = 0;
                        while (sala_atual->descricao_curta[i] != '\0' && i < 1000) {
                            putchar(sala_atual->descricao_curta[i++]);
                            fflush(stdout);
                            SLEEP_MS(30);
                        }
                        printf("\n");
                    }
                    
                    // Mostrar itens na sala
                    if (sala_atual->num_itens > 0) {
                        printf("Voce ve: ");
                        for (int item_idx = 0; item_idx < sala_atual->num_itens; item_idx++) {
                            SLEEP_MS(300);
                            printf("%s", sala_atual->itens[item_idx]->nome);
                            if (item_idx < sala_atual->num_itens - 1) {
                                printf(", ");
                            }
                        }
                        printf("\n");
                    }
                    
                    // Mostrar saídas
                    printf("Saidas: ");
                    int tem_saida = 0;
                    if (sala_atual->conexoes[0] != -1) {
                        printf("norte");
                        tem_saida = 1;
                    }
                    if (sala_atual->conexoes[1] != -1) {
                        if (tem_saida) printf(", ");
                        printf("leste");
                        tem_saida = 1;
                    }
                    if (sala_atual->conexoes[2] != -1) {
                        if (tem_saida) printf(", ");
                        printf("sul");
                        tem_saida = 1;
                    }
                    if (sala_atual->conexoes[3] != -1) {
                        if (tem_saida) printf(", ");
                        printf("oeste");
                        tem_saida = 1;
                    }
                    if (!tem_saida) {
                        printf("nenhuma");
                    }
                    printf("\n");
                    
                    // === SISTEMA DE SANIDADE (5 níveis) ===
                    if (game.sanity >= 80) {
                        // Nível 1: Sanidade alta (80-100%) - Apenas pequenos sinais
                        if (rand() % 10 == 0) { // 10% de chance
                            SLEEP_MS(300);
                            char* msg_leve = "Voce sente um leve arrepio...";
                            i = 0;
                            while (msg_leve[i] != '\0' && i < 1000) {
                                putchar(msg_leve[i++]);
                                fflush(stdout);
                                SLEEP_MS(40);
                            }
                            printf("\n");
                        }
                    } 
                    else if (game.sanity >= 60) {
                        // Nível 2: Sanidade média-alta (60-79%)
                        SLEEP_MS(500);
                        char* msg_desconforto = "Voce sente um desconforto crescente...";
                        i = 0;
                        while (msg_desconforto[i] != '\0' && i < 1000) {
                            putchar(msg_desconforto[i++]);
                            fflush(stdout);
                            SLEEP_MS(50);
                        }
                        printf("\n");
                        
                        if (rand() % 5 == 0) { // 20% de chance
                            SLEEP_MS(300);
                            int evento = rand() % 3;
                            char* eventos_nivel2[] = {
                                "Por um momento, voce acha que viu algo se mover no canto do olho.",
                                "Um odor putrido passa rapidamente por suas narinas.",
                                "Voce ouve um sussurro distante... era seu nome?"
                            };
                            i = 0;
                            while (eventos_nivel2[evento][i] != '\0' && i < 1000) {
                                putchar(eventos_nivel2[evento][i++]);
                                fflush(stdout);
                                SLEEP_MS(60);
                            }
                            printf("\n");
                        }
                    }
                    else if (game.sanity >= 40) {
                        // Nível 3: Sanidade média (40-59%) - Alucinações leves
                        SLEEP_MS(700);
                        char* msg_sombras = "As sombras dancam nos cantos de sua visao...";
                        i = 0;
                        while (msg_sombras[i] != '\0' && i < 1000) {
                            putchar(msg_sombras[i++]);
                            fflush(stdout);
                            SLEEP_MS(60);
                        }
                        printf("\n");
                        
                        if (rand() % 3 == 0) { // 33% de chance
                            SLEEP_MS(500);
                            int evento = rand() % 4;
                            char* eventos_nivel3[] = {
                                "Pegadas molhadas aparecem no chao, levando a lugar nenhum.",
                                "Voce ouve o choro abafado de uma crianca... Ana?",
                                "As paredes parecem respirar lentamente.",
                                "Tres batidas... sempre tres..."
                            };
                            
                            if(evento == 3) {
                                printf("\n");
                                for(int toc = 0; toc < 3; toc++) {
                                    printf("*toc* ");
                                    fflush(stdout);
                                    SLEEP_MS(600);
                                }
                                printf("\n");
                            }
                            
                            i = 0;
                            while (eventos_nivel3[evento][i] != '\0' && i < 1000) {
                                putchar(eventos_nivel3[evento][i++]);
                                fflush(stdout);
                                SLEEP_MS(70);
                            }
                            printf("\n");
                            game.sanity -= 2; // Perda adicional
                        }
                    }
                    else if (game.sanity >= 20) {
                        // Nível 4: Sanidade baixa (20-39%) - Alucinações intensas
                        SLEEP_MS(1000);
                        char* msg_realidade = "A realidade se desmancha ao seu redor...";
                        i = 0;
                        while (msg_realidade[i] != '\0' && i < 1000) {
                            putchar(msg_realidade[i++]);
                            fflush(stdout);
                            SLEEP_MS(70);
                        }
                        printf("\n");
                        
                        if (rand() % 2 == 0) { // 50% de chance
                            SLEEP_MS(700);
                            int evento = rand() % 5;
                            char* eventos_nivel4[] = {
                                "SANGUE! Sangue escorre pelas paredes formando palavras: 'CULPADO'",
                                "Voce ve uma figura no canto - e voce mesmo, enforcado e apodrecendo.",
                                "... por que voce me matou?",
                                "Maos invisiveis agarram seus tornozelos. Voce tropeca.",
                                "Seu reflexo aparece em uma parede sem espelho, sorrindo maliciosamente."
                            };
                            
                            if(evento == 2) {
                                printf("\n\"");
                                for(int name_idx = 0; game.playerName[name_idx] != '\0'; name_idx++) {
                                    putchar(game.playerName[name_idx]);
                                    fflush(stdout);
                                    SLEEP_MS(150);
                                }
                                printf("... por que voce me matou?\" - sussurra uma voz infantil.\n");
                            } else {
                                i = 0;
                                while (eventos_nivel4[evento][i] != '\0' && i < 1000) {
                                    putchar(eventos_nivel4[evento][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(80);
                                }
                                printf("\n");
                            }
                            game.sanity -= 3;
                        }
                    }
                    else {
                        // Nível 5: Sanidade crítica (0-19%) - Caos total
                        SLEEP_MS(1500);
                        char* msg_loucura = "A LOUCURA TE CONSOME!";
                        i = 0;
                        while (msg_loucura[i] != '\0' && i < 1000) {
                            putchar(msg_loucura[i++]);
                            fflush(stdout);
                            SLEEP_MS(100);
                        }
                        printf("\n");
                        
                        SLEEP_MS(1000);
                        int evento = rand() % 6;
                        char* eventos_criticos[] = {
                            "ELES ESTÃO AQUI! TODOS ELES! OS MORTOS TE CERCAM!\nAna segura sua boneca quebrada. 'Por que, papai?'",
                            "Voce sente suas maos molhadas. Ao olhar, estao cobertas de sangue fresco.\nO sangue nunca sai. NUNCA SAI!",
                            "As paredes gritam seu nome em unissono. O som e ensurdecedor.",
                            "Voce se ve no espelho, mas nao e voce. E o monstro que voce se tornou.\nO monstro sorri e sussurra: 'Tres desejos, tres mortes.'",
                            "O chao se torna um mar de maos agarrando, puxando voce para baixo.\nSao as maos de todos que voce amou e destruiu.",
                            "A pata de macaco aparece em sua mao, mesmo que voce nao a tenha.\nEla se contorce e sussurra: 'Faca outro desejo...'"
                        };
                        
                        if(evento == 2) {
                            i = 0;
                            while (eventos_criticos[evento][i] != '\0' && i < 1000) {
                                putchar(eventos_criticos[evento][i++]);
                                fflush(stdout);
                                SLEEP_MS(90);
                            }
                            printf("\n");
                            for(int grito = 0; grito < 5; grito++) {
                                printf("%s! ", game.playerName);
                                fflush(stdout);
                                SLEEP_MS(400);
                            }
                            printf("\n");
                        } else {
                            i = 0;
                            while (eventos_criticos[evento][i] != '\0' && i < 1000) {
                                putchar(eventos_criticos[evento][i++]);
                                fflush(stdout);
                                SLEEP_MS(90);
                            }
                            printf("\n");
                        }
                        game.sanity -= 5;
                    }
                    
                    // Mostrar barra de sanidade
                    printf("\n");
                    printf("Sanidade: [");
                    
                    int sanidade_visual = game.sanity / 10;
                    
                    if (game.sanity >= 70) {
                        for (int bar = 0; bar < sanidade_visual; bar++) printf("#");
                        for (int bar = sanidade_visual; bar < 10; bar++) printf("-");
                    } 
                    else if (game.sanity >= 30) {
                        for (int bar = 0; bar < sanidade_visual; bar++) printf("=");
                        for (int bar = sanidade_visual; bar < 10; bar++) printf("-");
                    }
                    else {
                        for (int bar = 0; bar < sanidade_visual; bar++) printf(".");
                        for (int bar = sanidade_visual; bar < 10; bar++) printf("-");
                    }
                    printf("] %d%%\n", game.sanity);
                    
                    // Ler comando do jogador
                    printf("\n> ");
                    fflush(stdout);
                    
                    if (fgets(input, MAX_INPUT, stdin) == NULL) {
                        break; // EOF ou erro
                    }
                    
                    // Limpar o \n do final da string
                    input[strcspn(input, "\n")] = 0;
                    
                    // Ignorar comandos vazios
                    if (strlen(input) == 0) {
                        continue;
                    }
                    
                    // Converter para minúsculas
                    for(int conv_idx = 0; input[conv_idx]; conv_idx++) {
                        input[conv_idx] = tolower(input[conv_idx]);
                    }
                    
                    // === PROCESSAR COMANDO (inline) ===
                    char cmd[MAX_INPUT];
                    char arg[MAX_INPUT];
                    
                    // Extrair comando e argumento
                    arg[0] = '\0';
                    sscanf(input, "%s %[^\n]", cmd, arg);
                    
                    // Verificar se é o final do jogo (após o terceiro desejo)
                    if (game.desejos_feitos == 3) {
                        if (strcmp(cmd, "abrir") == 0 && 
                            (strcmp(arg, "porta") == 0 || strcmp(arg, "a porta") == 0)) {
                            // === FINAL ABRIR PORTA (inline) ===
                            system("clear || cls");
                            printf("\n\n");
                            SLEEP_SEC(2);
                            
                            char* msgs_abrir[] = {
                                "Você caminha lentamente em direção à porta principal...",
                                "As batidas continuam ritmadas e insistentes...",
                                "Sua mão trêmula alcança a maçaneta..."
                            };
                            
                            for(int msg_idx = 0; msg_idx < 3; msg_idx++) {
                                i = 0;
                                while (msgs_abrir[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msgs_abrir[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(50 + msg_idx * 10);
                                }
                                printf("\n");
                                SLEEP_SEC(msg_idx == 2 ? 2 : 1);
                            }
                            
                            // Som de porta abrindo
                            printf("\n*CREEEEEK*\n");
                            SLEEP_SEC(2);
                            
                            char* msg_abrindo = "A porta se abre lentamente, revelando...";
                            i = 0;
                            while (msg_abrindo[i] != '\0' && i < 1000) {
                                putchar(msg_abrindo[i++]);
                                fflush(stdout);
                                SLEEP_MS(70);
                            }
                            printf("\n");
                            SLEEP_SEC(3);
                            
                            // Final do jogo
                            system("clear || cls");
                            printf("\n\n");
                            
                            char* final_msgs[] = {
                                "Você mesmo está parado do outro lado da porta.",
                                "Mas seu reflexo está distorcido, seu rosto contorcido em uma expressão de dor e culpa.",
                                "Seu outro eu sussurra: \"O que você fez não pode ser desfeito...\"",
                                "Você sente seu corpo congelar enquanto seu reflexo avança em sua direção.",
                                "A última coisa que você vê é seu próprio rosto, consumido pela culpa..."
                            };
                            
                            for(int final_idx = 0; final_idx < 5; final_idx++) {
                                i = 0;
                                while (final_msgs[final_idx][i] != '\0' && i < 1000) {
                                    putchar(final_msgs[final_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(final_idx == 0 ? 100 : (60 + final_idx * 5));
                                }
                                printf("\n");
                                SLEEP_SEC(2);
                            }
                            
                            // Mensagem final
                            system("clear || cls");
                            printf("\n\n\n");
                            SLEEP_SEC(1);
                            
                            char* fim_abrir = "                 ===== F I M =====";
                            i = 0;
                            while (fim_abrir[i] != '\0' && i < 1000) {
                                putchar(fim_abrir[i++]);
                                fflush(stdout);
                                SLEEP_MS(100);
                            }
                            printf("\n");
                            SLEEP_SEC(1);
                            
                            char* msg_final_abrir[] = {
                                "Você foi consumido pelo seu reflexo no espelho da culpa.",
                                "Alguns desejos têm um preço alto demais."
                            };
                            
                            for(int msg_idx = 0; msg_idx < 2; msg_idx++) {
                                i = 0;
                                while (msg_final_abrir[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msg_final_abrir[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(60);
                                }
                                printf("\n");
                                SLEEP_SEC(2);
                            }
                            
                            game.gameRunning = 0;
                            continue;
                        }
                        else if (strcmp(cmd, "esconder") == 0) {
                            // === FINAL ESCONDER (inline) ===
                            system("clear || cls");
                            printf("\n\n");
                            SLEEP_SEC(2);
                            
                            char* msgs_esconder[] = {
                                "Você corre pelo corredor procurando um lugar para se esconder...",
                                "As batidas na porta ficam mais fortes e mais rápidas...",
                                "Você se esconde em um armário, tremendo..."
                            };
                            
                            for(int msg_idx = 0; msg_idx < 3; msg_idx++) {
                                i = 0;
                                while (msgs_esconder[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msgs_esconder[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(50 + msg_idx * 10);
                                }
                                printf("\n");
                                SLEEP_SEC(msg_idx == 2 ? 2 : 1);
                            }
                            
                            // Som de porta quebrando
                            printf("\n*CRASH!*\n");
                            SLEEP_SEC(2);
                            
                            char* msg_arrombada = "A porta principal é arrombada violentamente!";
                            i = 0;
                            while (msg_arrombada[i] != '\0' && i < 1000) {
                                putchar(msg_arrombada[i++]);
                                fflush(stdout);
                                SLEEP_MS(50);
                            }
                            printf("\n");
                            SLEEP_SEC(2);
                            
                            char* msg_passos = "Você ouve passos lentos se arrastando pelo corredor...";
                            i = 0;
                            while (msg_passos[i] != '\0' && i < 1000) {
                                putchar(msg_passos[i++]);
                                fflush(stdout);
                                SLEEP_MS(70);
                            }
                            printf("\n");
                            SLEEP_SEC(2);
                            
                            // Passos se aproximando
                            char* sons_passos[] = {"*thump*", " *thump*", " *thump*\n"};
                            for(int passo = 0; passo < 3; passo++) {
                                printf("%s", sons_passos[passo]);
                                fflush(stdout);
                                SLEEP_SEC(1);
                            }
                            SLEEP_SEC(1);
                            
                            char* msg_parada = "Os passos param em frente ao armário onde você está...";
                            i = 0;
                            while (msg_parada[i] != '\0' && i < 1000) {
                                putchar(msg_parada[i++]);
                                fflush(stdout);
                                SLEEP_MS(70);
                            }
                            printf("\n");
                            SLEEP_SEC(3);
                            
                            // Porta do armário abrindo
                            printf("\n*CRACK!*\n");
                            SLEEP_SEC(1);
                            
                            // Final do jogo
                            system("clear || cls");
                            printf("\n\n");
                            SLEEP_SEC(1);
                            
                            char* final_esconder[] = {
                                "A porta do armário se abre lentamente...",
                                "Do outro lado está você mesmo, mas desfigurado e com os olhos vazios.",
                                "\"Não se pode fugir do próprio reflexo...\" ele sussurra antes de agarrar seu pulso.",
                                "Você sente sua consciência se fragmentando enquanto é puxado para a escuridão..."
                            };
                            
                            for(int final_idx = 0; final_idx < 4; final_idx++) {
                                i = 0;
                                while (final_esconder[final_idx][i] != '\0' && i < 1000) {
                                    putchar(final_esconder[final_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(final_idx == 0 ? 100 : 60);
                                }
                                printf("\n");
                                SLEEP_SEC(final_idx == 3 ? 3 : 2);
                            }
                            
                            // Mensagem final
                            system("clear || cls");
                            printf("\n\n\n");
                            SLEEP_SEC(1);
                            
                            char* fim_esconder = "                 ===== F I M =====";
                            i = 0;
                            while (fim_esconder[i] != '\0' && i < 1000) {
                                putchar(fim_esconder[i++]);
                                fflush(stdout);
                                SLEEP_MS(100);
                            }
                            printf("\n");
                            SLEEP_SEC(1);
                            
                            game.gameRunning = 0;
                            continue;
                        }
                        else if (strcmp(cmd, "aceitar") == 0 && game.memorias_recuperadas >= 3) {
                            // === FINAL ACEITAR - REDENÇÃO (inline) ===
                            system("clear || cls");
                            printf("\n\n");
                            SLEEP_SEC(2);
                            
                            char* msgs_aceitar[] = {
                                "Você fecha os olhos e respira fundo...",
                                "Toda a culpa, toda a dor, todos os arrependimentos...",
                                "Você finalmente aceita.",
                                "\"Eu fiz isso. Eu usei a pata. Eu causei tudo isso.\"",
                                "\"Ana... minha pequena Ana... me perdoe.\""
                            };
                            
                            int delays_aceitar[] = {60, 50, 70, 60, 80};
                            int sleeps_aceitar[] = {1, 1, 2, 1, 2};
                            
                            for(int msg_idx = 0; msg_idx < 5; msg_idx++) {
                                i = 0;
                                while (msgs_aceitar[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msgs_aceitar[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(delays_aceitar[msg_idx]);
                                }
                                printf("\n");
                                SLEEP_SEC(sleeps_aceitar[msg_idx]);
                            }
                            
                            // As batidas param
                            char* msg_silencio = "Subitamente, as batidas na porta param.";
                            i = 0;
                            while (msg_silencio[i] != '\0' && i < 1000) {
                                putchar(msg_silencio[i++]);
                                fflush(stdout);
                                SLEEP_MS(60);
                            }
                            printf("\n");
                            SLEEP_SEC(2);
                            
                            char* msg_ensurdecedor = "O silêncio é ensurdecedor.";
                            i = 0;
                            while (msg_ensurdecedor[i] != '\0' && i < 1000) {
                                putchar(msg_ensurdecedor[i++]);
                                fflush(stdout);
                                SLEEP_MS(70);
                            }
                            printf("\n");
                            SLEEP_SEC(3);
                            
                            // Voz suave
                            char* msgs_voz[] = {
                                "Então, você ouve uma voz suave, quase um sussurro:",
                                "\"Papai?\"",
                                "Não é a voz distorcida e terrível que você temia.",
                                "É a voz dela. A verdadeira voz de Ana.",
                                "\"Eu já te perdoei, papai. Eu nunca te culpei.\"",
                                "\"Você precisa se perdoar também.\""
                            };
                            
                            int delays_voz[] = {60, 100, 70, 70, 80, 90};
                            int sleeps_voz[] = {1, 2, 2, 2, 2, 3};
                            
                            for(int msg_idx = 0; msg_idx < 6; msg_idx++) {
                                i = 0;
                                while (msgs_voz[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msgs_voz[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(delays_voz[msg_idx]);
                                }
                                printf("\n");
                                SLEEP_SEC(sleeps_voz[msg_idx]);
                            }
                            
                            // Transformação do ambiente
                            char* msgs_transformacao[] = {
                                "A casa começa a se iluminar lentamente...",
                                "As paredes escuras se dissolvem como névoa...",
                                "E você se encontra em pé no jardim, mas não o jardim morto.",
                                "É o jardim como era antes. Vivo. Florido. Belo."
                            };
                            
                            for(int msg_idx = 0; msg_idx < 4; msg_idx++) {
                                i = 0;
                                while (msgs_transformacao[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msgs_transformacao[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(msg_idx >= 2 ? 70 : 60);
                                }
                                printf("\n");
                                SLEEP_SEC(msg_idx >= 2 ? 2 : 1);
                            }
                            
                            // Reunião final
                            char* msgs_reuniao[] = {
                                "Ana está lá, sorrindo. Não como um fantasma, mas como uma memória de paz.",
                                "Sua esposa está ao lado dela, também sorrindo.",
                                "\"Está na hora de descansar\", elas dizem juntas."
                            };
                            
                            for(int msg_idx = 0; msg_idx < 3; msg_idx++) {
                                i = 0;
                                while (msgs_reuniao[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msgs_reuniao[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(70 + msg_idx * 10);
                                }
                                printf("\n");
                                SLEEP_SEC(msg_idx == 2 ? 3 : (1 + msg_idx));
                            }
                            
                            // Final de redenção
                            system("clear || cls");
                            printf("\n\n\n");
                            SLEEP_SEC(1);
                            
                            char* fim_redencao[] = {
                                "                 ===== F I M =====",
                                "                FINAL: REDENÇÃO"
                            };
                            
                            for(int msg_idx = 0; msg_idx < 2; msg_idx++) {
                                i = 0;
                                while (fim_redencao[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(fim_redencao[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(100);
                                }
                                printf("\n");
                                SLEEP_SEC(msg_idx == 0 ? 1 : 2);
                            }
                            
                            char* msgs_final_redencao[] = {
                                "Você finalmente encontrou a paz.",
                                "O ciclo foi quebrado.",
                                "A culpa foi perdoada."
                            };
                            
                            for(int msg_idx = 0; msg_idx < 3; msg_idx++) {
                                i = 0;
                                while (msgs_final_redencao[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msgs_final_redencao[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(70);
                                }
                                printf("\n");
                            }
                            SLEEP_SEC(3);
                            
                            char* msgs_epilogo[] = {
                                "Às vezes, o caminho mais difícil é aceitar o que fizemos...",
                                "...e nos permitir ser perdoados."
                            };
                            
                            for(int msg_idx = 0; msg_idx < 2; msg_idx++) {
                                i = 0;
                                while (msgs_epilogo[msg_idx][i] != '\0' && i < 1000) {
                                    putchar(msgs_epilogo[msg_idx][i++]);
                                    fflush(stdout);
                                    SLEEP_MS(80 + msg_idx * 10);
                                }
                                printf("\n");
                            }
                            SLEEP_SEC(3);
                            
                            game.gameRunning = 0;
                            continue;
                        }
                    }
                    
                    // === PROCESSAR COMANDOS NORMAIS ===
                    if (strcmp(cmd, "ajuda") == 0) {
                        printf("\nComandos disponiveis:\n");
                        printf("- ajuda: mostra esta ajuda\n");
                        printf("- ir [direcao]: move para a direcao especificada (norte, sul, leste, oeste)\n");
                        printf("- examinar [item/sala]: examina um item ou a sala atual\n");
                        printf("- pegar [item]: pega um item da sala\n");
                        printf("- usar [item]: usa um item do inventario\n");
                        printf("- inventario: mostra os itens no inventario\n");
                        printf("- desejar [desejo]: faz um desejo usando a pata de macaco\n");
                        printf("- sair: sai do jogo\n");
                        
                        // Se estiver no final do jogo, mostrar comandos adicionais
                        if (game.desejos_feitos == 3) {
                            printf("\nComandos especiais disponíveis:\n");
                            printf("- abrir porta: abre a porta para ver quem esta batendo\n");
                            printf("- esconder: tenta se esconder da presença do outro lado da porta\n");
                            if (game.memorias_recuperadas >= 3) {
                                printf("- aceitar: aceitar seu destino e buscar redenção\n");
                            }
                        }
                    }
                    else if (strcmp(cmd, "pegar") == 0) {
                        // === COMANDO PEGAR (inline) ===
                        if(strlen(arg) == 0) {
                            printf("Pegar o quê?\n");
                            continue;
                        }
                        
                        Sala* sala_atual_pegar = &game.salas[game.currentRoom];
                        Item* item_pegar = NULL;
                        int index_pegar = -1;
                        
                        // Procurar o item na sala
                        for (int pegar_idx = 0; pegar_idx < sala_atual_pegar->num_itens; pegar_idx++) {
                            if (strcmp(sala_atual_pegar->itens[pegar_idx]->nome, arg) == 0 ||
                                strstr(sala_atual_pegar->itens[pegar_idx]->nome, arg) == sala_atual_pegar->itens[pegar_idx]->nome) {
                                item_pegar = sala_atual_pegar->itens[pegar_idx];
                                index_pegar = pegar_idx;
                                break;
                            }
                        }
                        
                        if (item_pegar != NULL && item_pegar->coletavel) {
                            if (game.num_inventario >= MAX_ITEMS) {
                                printf("Seu inventario esta cheio.\n");
                                continue;
                            }
                            
                            // Adicionar ao inventário
                            game.inventario[game.num_inventario++] = item_pegar;
                            
                            // Remover da sala
                            for (int rem_idx = index_pegar; rem_idx < sala_atual_pegar->num_itens - 1; rem_idx++) {
                                sala_atual_pegar->itens[rem_idx] = sala_atual_pegar->itens[rem_idx + 1];
                            }
                            sala_atual_pegar->num_itens--;
                            
                            printf("Você pegou: %s\n", item_pegar->nome);
                            
                            // Efeitos especiais para certos itens
                            if (strcmp(item_pegar->nome, "pata de macaco") == 0) {
                                SLEEP_SEC(1);
                                printf("A pata se contorce levemente em sua mão...\n");
                                game.sanity -= 5;
                            }
                        } else if (item_pegar != NULL && !item_pegar->coletavel) {
                            printf("Você não pode pegar isso.\n");
                        } else {
                            printf("Não encontrei esse item aqui.\n");
                        }
                    }
                    else if (strcmp(cmd, "usar") == 0) {
                        // === COMANDO USAR (inline) ===
                        if(strlen(arg) == 0) {
                            printf("Usar o quê?\n");
                            continue;
                        }
                        
                        Item* item_usar = NULL;
                        for (int usar_idx = 0; usar_idx < game.num_inventario; usar_idx++) {
                            if (strcmp(game.inventario[usar_idx]->nome, arg) == 0 ||
                                strstr(game.inventario[usar_idx]->nome, arg) == game.inventario[usar_idx]->nome) {
                                item_usar = game.inventario[usar_idx];
                                break;
                            }
                        }
                        
                        if (item_usar == NULL) {
                            printf("Você não possui esse item.\n");
                            continue;
                        }
                        
                        // Lógica específica para cada item
                        if (strcmp(item_usar->nome, "chave antiga") == 0) {
                            if (game.currentRoom == 0) {
                                if (!item_usar->usado) {
                                    printf("Você usa a chave na porta...\n");
                                    SLEEP_SEC(1);
                                    printf("*CLICK*\n");
                                    SLEEP_SEC(1);
                                    printf("A porta se abre revelando um corredor sombrio.\n");
                                    printf("\n[IMPORTANTE: Agora você pode ir para LESTE usando o comando 'ir leste']\n");
                                    game.salas[0].conexoes[1] = 1; // Abrir conexão leste para o corredor
                                    item_usar->usado = 1;
                                } else {
                                    printf("A porta já está aberta.\n");
                                }
                            } else {
                                printf("Não há onde usar a chave aqui.\n");
                            }
                        }
                        else if (strcmp(item_usar->nome, "chave ornamentada") == 0) {
                            if (game.currentRoom == 2 && game.salas[5].revelada) {
                                if (!item_usar->usado) {
                                    printf("Você insere a chave ornamentada na porta misteriosa...\n");
                                    SLEEP_SEC(1);
                                    printf("A chave gira sozinha, como se tivesse vida própria!\n");
                                    SLEEP_SEC(1);
                                    printf("A porta se abre lentamente, revelando o Quarto do Espelho.\n");
                                    game.salas[2].conexoes[1] = 5; // Conectar ao quarto do espelho (leste)
                                    item_usar->usado = 1;
                                } else {
                                    printf("A porta já está aberta.\n");
                                }
                            } else {
                                printf("A chave vibra levemente, mas não há onde usá-la aqui.\n");
                            }
                        }
                        else if (strcmp(item_usar->nome, "espelho de mao") == 0) {
                            printf("Você olha no espelho...\n");
                            SLEEP_SEC(1);
                            
                            if (game.sanity >= 70) {
                                printf("Seu reflexo parece cansado e confuso.\n");
                            } else if (game.sanity >= 40) {
                                printf("Seu reflexo tem olhos vazios e uma expressão perturbadora.\n");
                                printf("Por um momento, ele parece sorrir enquanto você franze a testa.\n");
                                game.sanity -= 5;
                            } else {
                                printf("SEU REFLEXO NÃO É VOCÊ!\n");
                                SLEEP_SEC(1);
                                printf("É algo MUITO pior...\n");
                                printf("Ele bate no vidro do lado de dentro, tentando sair!\n");
                                game.sanity -= 10;
                            }
                        }
                        else if (strcmp(item_usar->nome, "pata de macaco") == 0) {
                            printf("Para fazer um desejo, use o comando 'desejar [seu desejo]'.\n");
                        }
                        else {
                            printf("Você não pode usar isso agora.\n");
                        }
                    }
                    else if (strcmp(cmd, "desejar") == 0) {
                        // === COMANDO DESEJAR (inline) ===
                        // Verificar se o jogador tem a pata de macaco
                        Item* pata = NULL;
                        for (int pata_idx = 0; pata_idx < game.num_inventario; pata_idx++) {
                            if (strcmp(game.inventario[pata_idx]->nome, "pata de macaco") == 0) {
                                pata = game.inventario[pata_idx];
                                break;
                            }
                        }
                        
                        if (pata == NULL) {
                            printf("Você não possui a pata de macaco.\n");
                            continue;
                        }
                        
                        if (game.desejos_feitos >= 3) {
                            printf("A pata de macaco já concedeu três desejos e não tem mais poder.\n");
                            continue;
                        }
                        
                        if (strlen(arg) == 0) {
                            printf("Você precisa especificar um desejo.\n");
                            continue;
                        }
                        
                        // Armazenar o desejo
                        strcpy(game.ultimo_desejo, arg);
                        
                        // Fazer o desejo com suspense
                        game.desejos_feitos++;
                        
                        printf("\nVocê segura a pata de macaco firmemente...\n");
                        SLEEP_SEC(1);
                        printf("Você deseja: \"%s\"\n", arg);
                        SLEEP_SEC(1);
                        printf("A pata se CONTORCE violentamente em sua mão como uma cobra!\n");
                        SLEEP_SEC(1);
                        
                        // Simples análise do desejo
                        int efeito_desejo = 1; // 1=neutro, 2=altruísta, 3=egoísta
                        if (strstr(arg, "lembrar") || strstr(arg, "memoria") || strstr(arg, "verdade")) {
                            efeito_desejo = 2;
                        } else if (strstr(arg, "trazer") || strstr(arg, "voltar") || strstr(arg, "vida")) {
                            efeito_desejo = 3;
                        }
                        
                        if (game.desejos_feitos == 1) {
                            printf("Por um momento, nada parece acontecer...\n");
                            SLEEP_SEC(2);
                            
                            if(efeito_desejo == 2) {
                                printf("Memórias fragmentadas invadem sua mente!\n");
                                game.memorias_recuperadas++;
                            } else {
                                printf("Sussurros ecoam pelas paredes: 'Cuidado com o que deseja...'\n");
                            }
                            
                            game.sanity -= 10;
                            
                            // Revelar o quarto do espelho
                            if (game.salas[5].revelada == 0) {
                                SLEEP_SEC(2);
                                printf("\nVocê ouve um ESTALO vindo da sala de estar...\n");
                                printf("Uma porta que você nunca notou antes apareceu na parede leste!\n");
                                game.salas[2].conexoes[1] = 5;
                                game.salas[5].revelada = 1;
                                // Adicionar chave ornamentada ao corredor
                                game.salas[1].itens[game.salas[1].num_itens++] = &game.itens[8];
                                printf("Algo tilinta no chão do corredor...\n");
                            }
                        }
                        else if (game.desejos_feitos == 2) {
                            printf("Uma sensação gélida percorre seu corpo...\n");
                            SLEEP_SEC(1);
                            printf("Você ouve uma risada infantil... mas está distorcida, errada.\n");
                            printf("'Papai... por que você fez isso comigo?'\n");
                            SLEEP_SEC(1);
                            printf("A voz vem de todos os lugares ao mesmo tempo.\n");
                            game.sanity -= 20;
                            game.memorias_recuperadas++;
                        }
                        else if (game.desejos_feitos == 3) {
                            printf("A escuridão te envolve como um manto...\n");
                            SLEEP_SEC(1);
                            printf("Agora você entende. Esta casa... este loop... é sua punição.\n");
                            printf("A pergunta é: desta vez, você escolherá diferente?\n");
                            SLEEP_SEC(2);
                            
                            // Efeito de batidas na porta
                            printf("\n*toc* *TOC* *TOC!*\n");
                            SLEEP_SEC(1);
                            printf("Batidas fortes na porta principal!\n");
                            printf("Uma voz familiar chama seu nome do outro lado...\n");
                            printf("\"%s... Abra a porta...\"\n", game.playerName);
                            
                            game.sanity -= 30;
                            printf("\nO que você fará?\n");
                            printf("Digite 'abrir porta' para enfrentar seu destino ou 'esconder' para fugir.\n");
                            if (game.memorias_recuperadas >= 3) {
                                printf("Ou digite 'aceitar' para aceitar seu destino.\n");
                            }
                        }
                    }
                    else if (strcmp(cmd, "ir") == 0 || strcmp(cmd, "mover") == 0 || strcmp(cmd, "andar") == 0) {
                        // === COMANDO IR (movimento) ===
                        if(strlen(arg) == 0) {
                            printf("Ir para onde? Use: norte, sul, leste ou oeste.\n");
                            continue;
                        }
                        
                        int direcao = -1;
                        if (strcmp(arg, "norte") == 0) {
                            direcao = 0;
                        } else if (strcmp(arg, "leste") == 0) {
                            direcao = 1;
                        } else if (strcmp(arg, "sul") == 0) {
                            direcao = 2;
                        } else if (strcmp(arg, "oeste") == 0) {
                            direcao = 3;
                        } else {
                            printf("Direção inválida. Use: norte, sul, leste ou oeste.\n");
                            continue;
                        }
                        
                        int nova_sala = game.salas[game.currentRoom].conexoes[direcao];
                        if (nova_sala != -1) {
                            game.currentRoom = nova_sala;
                            printf("Você se move para %s...\n", arg);
                            SLEEP_SEC(1);
                            
                            // Efeitos especiais ao entrar em certas salas
                            if (nova_sala == 5 && game.salas[5].visitada == 0) {
                                // Primeira vez no quarto do espelho
                                printf("Ao entrar no quarto, você sente uma presença esmagadora...\n");
                                game.sanity -= 10;
                            } else if (nova_sala == 6 && game.salas[6].visitada == 0) {
                                // Primeira vez no jardim
                                printf("O ar pesado do jardim morto te sufoca...\n");
                                game.sanity -= 5;
                            }
                        } else {
                            printf("Não há passagem nessa direção.\n");
                        }
                    }
                    else if (strcmp(cmd, "examinar") == 0) {
                        // === COMANDO EXAMINAR ===
                        if(strlen(arg) == 0 || strcmp(arg, "sala") == 0) {
                            // Examinar a sala atual
                            Sala* sala_exam = &game.salas[game.currentRoom];
                            printf("Você examina %s cuidadosamente...\n", sala_exam->nome);
                            SLEEP_SEC(1);
                            i = 0;
                            while (sala_exam->descricao[i] != '\0' && i < 1000) {
                                putchar(sala_exam->descricao[i++]);
                                fflush(stdout);
                                SLEEP_MS(30);
                            }
                            printf("\n");
                        } else {
                            // Examinar um item específico
                            Item* item_exam = NULL;
                            
                            // Procurar no inventário
                            for (int exam_idx = 0; exam_idx < game.num_inventario; exam_idx++) {
                                if (strcmp(game.inventario[exam_idx]->nome, arg) == 0 ||
                                    strstr(game.inventario[exam_idx]->nome, arg) == game.inventario[exam_idx]->nome) {
                                    item_exam = game.inventario[exam_idx];
                                    break;
                                }
                            }
                            
                            // Se não achou no inventário, procurar na sala
                            if (item_exam == NULL) {
                                Sala* sala_atual_exam = &game.salas[game.currentRoom];
                                for (int exam_idx = 0; exam_idx < sala_atual_exam->num_itens; exam_idx++) {
                                    if (strcmp(sala_atual_exam->itens[exam_idx]->nome, arg) == 0 ||
                                        strstr(sala_atual_exam->itens[exam_idx]->nome, arg) == sala_atual_exam->itens[exam_idx]->nome) {
                                        item_exam = sala_atual_exam->itens[exam_idx];
                                        break;
                                    }
                                }
                            }
                            
                            if (item_exam != NULL) {
                                printf("Você examina %s...\n", item_exam->nome);
                                SLEEP_SEC(1);
                                i = 0;
                                while (item_exam->descricao[i] != '\0' && i < 1000) {
                                    putchar(item_exam->descricao[i++]);
                                    fflush(stdout);
                                    SLEEP_MS(40);
                                }
                                printf("\n");
                                
                                // Revelar memórias se aplicável
                                if (item_exam->revelacao > 0 && item_exam->revelacao <= 3) {
                                    game.memorias_recuperadas++;
                                    if (item_exam->revelacao == 1) {
                                        printf("\nUma memória retorna...\n");
                                        game.sanity -= 5;
                                    } else if (item_exam->revelacao == 2) {
                                        printf("\nA culpa te consome...\n");
                                        game.sanity -= 10;
                                    } else if (item_exam->revelacao == 3) {
                                        printf("\nA verdade terrível é revelada...\n");
                                        game.sanity -= 15;
                                    }
                                }
                            } else {
                                printf("Você não vê isso aqui.\n");
                            }
                        }
                    }
                    else if (strcmp(cmd, "inventario") == 0) {
                        // === COMANDO INVENTÁRIO (inline) ===
                        if (game.num_inventario == 0) {
                            char* msg_vazio = "Seu inventário está vazio.";
                            i = 0;
                            while (msg_vazio[i] != '\0' && i < 1000) {
                                putchar(msg_vazio[i++]);
                                fflush(stdout);
                                SLEEP_MS(40);
                            }
                            printf("\n");
                        } else {
                            char* titulo_inv = "Inventário:";
                            i = 0;
                            while (titulo_inv[i] != '\0' && i < 1000) {
                                putchar(titulo_inv[i++]);
                                fflush(stdout);
                                SLEEP_MS(40);
                            }
                            printf("\n");
                            for (int inv_idx = 0; inv_idx < game.num_inventario; inv_idx++) {
                                SLEEP_MS(200);
                                printf("- %s\n", game.inventario[inv_idx]->nome);
                            }
                        }
                    }
                    else if (strcmp(cmd, "sair") == 0) {
                        // === COMANDO SAIR (inline) ===
                        char* msg_certeza = "Tem certeza que quer sair? (s/n): ";
                        i = 0;
                        while (msg_certeza[i] != '\0' && i < 1000) {
                            putchar(msg_certeza[i++]);
                            fflush(stdout);
                            SLEEP_MS(40);
                        }
                        char confirm;
                        scanf(" %c", &confirm);
                        if (confirm == 's' || confirm == 'S') {
                            game.gameRunning = 0;
                        }
                        // Limpar buffer
                        int c_clear;
                        while ((c_clear = getchar()) != '\n' && c_clear != EOF);
                    }
                    else {
                        // === COMANDO INVÁLIDO ===
                        char* msg_invalido = "Comando inválido. Digite 'ajuda' para ver os comandos disponíveis.";
                        i = 0;
                        while (msg_invalido[i] != '\0' && i < 1000) {
                            putchar(msg_invalido[i++]);
                            fflush(stdout);
                            SLEEP_MS(40);
                        }
                        printf("\n");
                    }
                    
                    // === VERIFICAR PERDA DE SANIDADE ===
                    if(game.sanity <= 0) {
                        system("clear || cls");
                        printf("\n\n");
                        SLEEP_SEC(1);
                        
                        char* msgs_loucura[] = {
                            "Sua visão começa a ficar turva...",
                            "Sombras dançam ao seu redor, cada vez mais próximas...",
                            "Você tenta lutar contra a escuridão que invade sua mente...",
                            "Mas é forte demais..."
                        };
                        
                        int delays_loucura[] = {70, 60, 60, 100};
                        int sleeps_loucura[] = {1, 1, 1, 2};
                        
                        for(int msg_idx = 0; msg_idx < 4; msg_idx++) {
                            i = 0;
                            while (msgs_loucura[msg_idx][i] != '\0' && i < 1000) {
                                putchar(msgs_loucura[msg_idx][i++]);
                                fflush(stdout);
                                SLEEP_MS(delays_loucura[msg_idx]);
                            }
                            printf("\n");
                            SLEEP_SEC(sleeps_loucura[msg_idx]);
                        }
                        
                        system("clear || cls");
                        printf("\n\n\n");
                        SLEEP_SEC(1);
                        
                        char* loucura_final = "SUA MENTE SUCUMBE À LOUCURA...";
                        i = 0;
                        while (loucura_final[i] != '\0' && i < 1000) {
                            putchar(loucura_final[i++]);
                            fflush(stdout);
                            SLEEP_MS(150);
                        }
                        printf("\n");
                        SLEEP_SEC(3);
                        
                        char* fim_loucura = "                 ===== F I M =====";
                        i = 0;
                        while (fim_loucura[i] != '\0' && i < 1000) {
                            putchar(fim_loucura[i++]);
                            fflush(stdout);
                            SLEEP_MS(100);
                        }
                        printf("\n");
                        SLEEP_SEC(1);
                        
                        char* msg_final_loucura = "Você perdeu o controle da sua sanidade.";
                        i = 0;
                        while (msg_final_loucura[i] != '\0' && i < 1000) {
                            putchar(msg_final_loucura[i++]);
                            fflush(stdout);
                            SLEEP_MS(60);
                        }
                        printf("\n");
                        SLEEP_SEC(2);
                        
                        game.gameRunning = 0;
                    }
                } // Fim do loop principal do jogo
                
                char* msg_obrigado = "Obrigado por jogar O ESPELHO DA CULPA.";
                i = 0;
                while (msg_obrigado[i] != '\0' && i < 1000) {
                    putchar(msg_obrigado[i++]);
                    fflush(stdout);
                    SLEEP_MS(50);
                }
                printf("\nPressione ENTER para voltar ao menu principal...");
                getchar();
                
                break; // Fim do case 1 (jogo)
            }
            case 2: {
                // === MENU VER SCORE ===
                system("clear || cls");
                printf("Score do Jogo\n\n");
                printf("1 - Jogador A - Sobreviveu com 75%% de sanidade\n");
                printf("2 - Jogador B - Sobreviveu com 45%% de sanidade\n");
                printf("3 - Jogador C - Não sobreviveu (perdeu toda sanidade)\n");
                printf("4 - Jogador D - Morreu pelo reflexo no espelho\n");
                printf("5 - Jogador E - Foi consumido pelas sombras\n");
                printf("\nPressione ENTER para continuar...");
                int c_score;
                while ((c_score = getchar()) != '\n' && c_score != EOF);
                break;
            }
            case 3: {
                // === MENU SOBRE ===
                system("clear || cls");
                printf("Sobre o jogo\n\n");
                printf("Desenvolvido em C\n");
                printf("Um jogo de terror psicologico baseado em texto\n");
                printf("Versao 1.3 - O Espelho da Culpa - VERSAO ACADEMICA\n");
                printf("\nPressione ENTER para continuar...");
                int c_sobre;
                while ((c_sobre = getchar()) != '\n' && c_sobre != EOF);
                break;
            }
            case 4: {
                // === SAIR DO PROGRAMA ===
                system("clear || cls");
                printf("Ate logo!\n");
                break;
            }
            default: {
                // === OPÇÃO INVÁLIDA ===
                printf("Opção inválida!\n");
                printf("Pressione ENTER para continuar...");
                int c_default;
                while ((c_default = getchar()) != '\n' && c_default != EOF);
                break;
            }
        } // Fim do switch
    } // Fim do loop do menu
    
    return 0;
} // Fim da função main