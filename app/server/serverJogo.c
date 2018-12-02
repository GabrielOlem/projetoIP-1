#include "server.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ACore.h"


//DEFINIÇÕES DAS CONSTANTES
// #define MSG_MAX_SIZE 350
// #define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define MAX_CLIENTS 6
#define QTD_TIMES 2
#define X_MAX 1280
#define Y_MAX 720
#define TEMPO_LIMITE 120
#define DIREITA 0x44            //D
#define ESQUERDA 0x41           //A
#define CIMA 0x57               //W
#define BAIXO 0x53              //S
#define VAZIO 0x30              //0
#define INVALIDO 0x49           //input.client_id
#define BANDEIRA_BLUE 0x42      //B  
#define BANDEIRA_RED 0x50       //P
#define CONGELAR 0x20           //SPACE
#define TRAP_TEAM_BLUE 0x54     //T
#define TRAP_TEAM_RED 0x56      //V
#define PLAYER_COM_TRAP 0x51    //Q
#define TRAP 0x4A               //J
#define X_FLAG_BLUE 1280
#define Y_FLAG_BLUE 0
#define X_FLAG_RED 0
#define Y_FLAG_RED 720
#define X_ENTRADA_1_BLUE 1275
#define Y_ENTRADA_1_BLUE 8
#define X_ENTRADA_2_BLUE 1273
#define Y_ENTRADA_2_BLUE 5
#define X_ENTRADA_3_BLUE 1270
#define Y_ENTRADA_3_BLUE 2
#define X_ENTRADA_1_RED 0
#define Y_ENTRADA_1_RED 719
#define X_ENTRADA_2_RED 5
#define Y_ENTRADA_2_RED 715
#define X_ENTRADA_3_RED 8
#define Y_ENTRADA_3_RED 710

//INICIALIZAÇÃO DAS VARIÁVEIS GLOBAIS
char mapa [50][51];
Player players[6];
int qntJogadores = 0;
PROTOCOLO_JOGO jogada_client, jogada_server, tempo;                 // Protocolo de envio a ser enviado para o cliente com as infos do jogo;
struct msg_ret_t input;
PROTOCOLO_JOGO jogada, updateServer;
PROTOCOLO_TESTE teste_envia, teste_lobby;
double tempoInicio, tempoAtual;
int i, j, fim = 0, tp=0; 

//INICIALIZAÇÃO DAS FUNÇÕES
void inicializaMapa();
void inicializaJogadores();
void runGame();
void runGameTest();


int main() {
    //CRIANDO MAPA 
    inicializaMapa();
        
    //INICIALIZAÇÃO DO SERVER
    serverInit(MAX_CLIENTS);
    puts("Server is running!!");
    
    //INICIALIZAÇÃO DOS JOGADORES
    inicializaJogadores();

    // puts("saiu");

    //JOGO
    runGameTest();
   
    return 0;
}

void imprimeMatriz(){
    for(i=0;i<50;i++){
        printf("%s\n", mapa[i]);
    }
}

void inicializaMapa(){
    
    strcpy(mapa[0],"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[1],"NNNNNNNNNNNNNNNNNNNXXXXNNNNXXXXXXXXXXXXXNNNNNXNNNN");
    strcpy(mapa[2],"NNNNNNNNNNNNNNNNNNNXXXXNNNNXXXXXXXXXXXXXNNNNNXNNNN");
    strcpy(mapa[3],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXNNNNSSSSSN");
    strcpy(mapa[4],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXSNNZSSSSSN");
    strcpy(mapa[5],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXSSNZZSSSSN");
    strcpy(mapa[6],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXSSNZZSSSSN");
    strcpy(mapa[7],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXSSZZSNNN");
    strcpy(mapa[8],"NXXXXXXXXXXXXXXXXXXXXSSSSSSSSXXXXXXXXXXXXXSSNZZSSN");
    strcpy(mapa[9],"NXXXXXXXXXXXXXXXXXXXXSSSSSSSSXXXXXXXXXXXXXSSNNZNSN");
    strcpy(mapa[10],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXSNNNNN");
    strcpy(mapa[11],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXSNNNNN");
    strcpy(mapa[12],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[13],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[14],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[15],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[16],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[17],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[18],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[19],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[20],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[21],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[22],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[23],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[24],"NXXXXXXXXXXXVVVVVXXVVSSSSSSSSXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[25],"NXXXXXXXXXXXVVVVVXXVVSSSSSSSSXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[26],"NXXXXXXXXXXXVVVVVXXVVSSSSSSSSXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[27],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[28],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[29],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[30],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[31],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[32],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[33],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[34],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[35],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[36],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[37],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[38],"NNNNSXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[39],"NNNNSXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[40],"NNZNNSXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[41],"NSSZNNSSXXXXVVVVVXXVVVSSSSSSXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[42],"NNSZZSSSXXXXXXXXXXXXXXSSSSSSXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[43],"NNSSZZZNXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[44],"NSSSSZZNSSXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[45],"NSSSSSZZNSSXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[46],"NSSSSSZZNNSXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[47],"NNNSSNNNNNSXXXXXXXXXXXXNNNNXXXXNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[48],"NNNNSNNNNNSXXXXXXXXXXXXNNNNXXXXNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[49],"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");

//   for(i = 0; i < 48; i++){
//       for(j = 0; j < 24; j++){
//           printf("%c",mapa[i][j]);
//       }
//       printf("\n");
//   }
//   printf("\n");
}


void inicializaJogadores(){
    int notReady = 1;
    int qntJogadoresProntos = 0;
    struct msg_ret_t input;

    while (notReady) {
        PROTOCOLO_INICIAL msg_client, msg_inicial_server;
        int id = acceptConnection();
        input = recvMsg((PROTOCOLO_INICIAL *) &msg_client);
        if(input.status != NO_MESSAGE){
            if (id != NO_CONNECTION) {
                    // Recebe o nick, capacete e id das novas conexões
                    if(msg_client.tipo == INICIAL ){
                        ++qntJogadores;
                        
                        if(qntJogadores < MAX_CLIENTS){
                            strcpy(players[id].name, msg_client.jogador.name);     // Salva o nick
                            players[id].helmet = msg_client.jogador.helmet;        // Salva o capacete
                            players[id].id = id;                                   // Salva o id         
                        
                            printf("%s connected id = %d\n", players[id].name, id);
                        
                            // Inicializando os demais atributos do player atual
                            if(qntJogadores%2 == 0){
                                players[id].team = 1;
                                players[id].position.x = 30;
                                players[id].position.y = 30;
                                players[id].posicaoPrint.x = players[id].position.x * 19;
                                players[id].posicaoPrint.y = players[id].position.y * 19;
                            }
                            else{
                                players[id].team = 2;
                                players[id].position.x = 20;
                                players[id].position.y = 20;
                                players[id].posicaoPrint.x = players[id].position.x * 19;
                                players[id].posicaoPrint.y = players[id].position.y * 19;
                            }                    
                            players[id].comBandeira = 0;
                            players[id].ready = 0;
                            players[id].estaCongelado = 0;
                            players[id].congelou = 0;
                            players[id].congelamentos = 4;
                            players[id].armadilhas = 3;

                            // Atualizando o player do client após as inicializações dos atributos
                            msg_inicial_server.tipo = INICIAL;
                            msg_inicial_server.jogador = players[id];                
                            sendMsgToClient((PROTOCOLO_INICIAL *) &msg_inicial_server, sizeof(PROTOCOLO_INICIAL), id);

                            //qntJogadores++;                                       
                        }
                        else{
                            printf("Numero max de clientes conectados ja foi atingido!\n");
                            printf("%s disconnected id = %d\n", players[id].name, id);
                            disconnectClient(id);
                            msg_client.tipo = -1;
                        } 
                    }
                }
                else{
                    //puts("loopou");
                    if(msg_client.tipo == ENDGAME){
                        printf("%s disconnected id = %d\n", players[msg_client.jogador.id].name, msg_client.jogador.id);
                        disconnectClient(msg_client.jogador.id);
                        msg_client.tipo = -1;
                    }
                    if(msg_client.tipo == COMECOU){
                        //puts("comecou?");
                        players[input.client_id] = msg_client.jogador;

                        if(players[input.client_id].ready == 0){
                            qntJogadoresProntos++;
                            //printf("id = %d",input.client_id);
                            players[input.client_id].ready++;
                        }
                    
                        if(input.client_id == 0){
                            // if(qntJogadoresProntos >= 4){
                                //puts("fooi");
                                //printf("qnt = %d", qntJogadores);
                                teste_lobby.qntJogadores = qntJogadores;
                                teste_lobby.tipo = 'l';
                                if(players[0].ready == 1){
                                    notReady = 0;
                                   // puts("break");
                                    //break;
                                }
                                for(int i = 0; i < qntJogadores; i++){
                                    teste_lobby.todosJogadores[i] = players[i];
                                    //printf("%d - %d, %d", i, jogada_server.todosJogadores[i].position.x,jogada_server.todosJogadores[i].position.y );
                                 }
                                broadcast(&teste_lobby, sizeof(PROTOCOLO_TESTE)); 
                      
                            // }    
                        }
                        else{
                            players[input.client_id].ready = 1;
                            //puts("waiting");
                            teste_lobby.tipo = 'w';
                            for(int i = 0; i < qntJogadores; i++){
                                teste_lobby.todosJogadores[i] = players[i];
                            //printf("%d - %d, %d", i, jogada_server.todosJogadores[i].position.x,jogada_server.todosJogadores[i].position.y );
                            }
                            broadcast(&teste_lobby, sizeof(PROTOCOLO_TESTE)); 
                        }

                           
                    }
                }
        }
  
    }
}



void runGameTest(){

    for(i = 0; i < qntJogadores; i++){
        //puts("oi");
        if(players[i].ready == 0){
            //printf("id disconnect %d", players[i].id);
            disconnectClient(i);
        }
    }
   // printf("qnt = %d", qntJogadores);
    while(!fim){
        input = recvMsg((PROTOCOLO_JOGO *) &jogada);
        // printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
       // players[input.client_id] = jogada.todosJogadores[input.client_id];
        // for(int i = 0; i < qntJogadores; i++){
        //     players[i] = jogada.todosJogadores[i];
        //    // printf("id = %d team = %d", players[i].id, players[i].team);
        // }
        // printf("x = %d, Y = %d", jogada.todosJogadores[input.client_id].position.x, jogada.todosJogadores[input.client_id].position.y);

        if(input.status != NO_MESSAGE){
            // printf("X = %d, Y = %d,", jogada.todosJogadores[input.client_id].position.x, jogada.todosJogadores[input.client_id].position.y);
            // printf(" Mc = %c",  mapa[jogada.todosJogadores[input.client_id].position.x -1] [ jogada.todosJogadores[input.client_id].position.y]);
            // printf(" Mb = %c",  mapa[jogada.todosJogadores[input.client_id].position.x +1] [ jogada.todosJogadores[input.client_id].position.y]);
            // printf(" Me = %c",  mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y -1]);
            // printf(" Md = %c\n",  mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y + 1]);

             for(int i = 0; i < qntJogadores; i++){
                players[i] = jogada.todosJogadores[i];
                // printf("posicao print::: x: %d, y:%d, posicao:: x: %d, y:%d\n", jogada.todosJogadores[i].posicaoPrint.x, jogada.todosJogadores[i].posicaoPrint.y, jogada.todosJogadores[i].position.x, jogada.todosJogadores[i].position.y);
                // printf("%d congelado? %d\n", i, players[i].estaCongelado);
                // printf("id = %d team = %d", players[i].id, players[i].team);
            }
            // printf("x = %d, Y = %d", players[input.client_id].position.x, players[input.client_id].position.y);
            if(jogada.tipo == ANDAR_CIMA && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'N' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                // imprimeMatriz();
                if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] == 'R'  || jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] == 'B'){
                    players[input.client_id].estaCongelado=1;
                    teste_envia.acao = 'c';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                }else{
                    
                    teste_envia.acao = 'c';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                } 
            }
            else if(jogada.tipo == ANDAR_BAIXO && mapa[jogada.todosJogadores[input.client_id].position.x + 1] [ jogada.todosJogadores[input.client_id].position.y] != 'N' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x +1] [ jogada.todosJogadores[input.client_id].position.y] == 'R' || jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x + 1] [ jogada.todosJogadores[input.client_id].position.y] == 'B'){
                    players[input.client_id].estaCongelado=1;
                    teste_envia.acao = 'b';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                }else{
                    
                    teste_envia.acao = 'b';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                } 
            }
            else if(jogada.tipo == ANDAR_ESQUERDA && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y - 1] != 'N' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y-1] == 'R' || jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y - 1] != 'B'){
                    players[input.client_id].estaCongelado=1;
                    teste_envia.acao = 'e';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                }else{
                    
                    teste_envia.acao = 'e';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                } 
            }
            else if(jogada.tipo == ANDAR_DIREITA && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y + 1] != 'N' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y+1] == 'R' || jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y + 1] == 'B'){
                    players[input.client_id].estaCongelado=1;
                    teste_envia.acao = 'd';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                }else{
                    
                    teste_envia.acao = 'd';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                } 
            }
            else if(jogada.tipo == BOTARTRAPS && mapa[jogada.todosJogadores[input.client_id].position.x] [jogada.todosJogadores[input.client_id].position.y] != 'S' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                if(mapa[jogada.todosJogadores[input.client_id].position.x][ jogada.todosJogadores[input.client_id].position.y] != 'Z'){
                    if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y] != 'B' && jogada.todosJogadores[input.client_id].armadilhas > 0){
                        // puts("traps");
                        for(int i = 0; i < qntJogadores; i++){
                            teste_envia.todosJogadores[i] = players[i];
                        }
                        teste_envia.acao = 't';
                        teste_envia.tipo = 'G';
                        teste_envia.id_acao = input.client_id;
                        teste_envia.traps[tp].posiT.x= jogada.todosJogadores[input.client_id].position.x;
                        teste_envia.traps[tp].posiT.y= jogada.todosJogadores[input.client_id].position.y;
                        teste_envia.traps[tp].team = 1;
                        teste_envia.tp++;
                        // printf("%d %d\n", teste_envia.traps[tp].posiT.x, teste_envia.traps[tp].posiT.y);
                        tp++;
                        mapa[jogada.todosJogadores[input.client_id].position.x][jogada.todosJogadores[input.client_id].position.y] = 'B';
                    // printf("acao = %c, id = %d", teste_envia.acao, teste_envia.id_acao);
                        broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                    }else if(jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y] != 'R' && jogada.todosJogadores[input.client_id].armadilhas > 0){
                        // puts("traps");
                        for(int i = 0; i < qntJogadores; i++){
                            teste_envia.todosJogadores[i] = players[i];
                        }
                        teste_envia.acao = 't';
                        teste_envia.tipo = 'G';
                        teste_envia.id_acao = input.client_id;
                        teste_envia.traps[tp].posiT.x = jogada.todosJogadores[input.client_id].position.x;
                        teste_envia.traps[tp].posiT.y = jogada.todosJogadores[input.client_id].position.y;
                        teste_envia.traps[tp].team = 2;
                        teste_envia.tp++;
                        // printf("%d %d\n", teste_envia.traps[tp].posiT.x, teste_envia.traps[tp].posiT.y);
                        tp++;
                        mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y] = 'R';
                    // printf("acao = %c, id = %d", teste_envia.acao, teste_envia.id_acao);
                        broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                    }
                    // printf("%s \n", mapa[jogada.todosJogadores[input.client_id].position.x]);
                }
            }
            // else if(jogada.tipo == CONGELA){
            //     teste_envia.acao = 'f';
            //     teste_envia.tipo = 'G';
            //     for(int i = 0; i < qntJogadores; i++);
            // }
            else if(jogada.tipo == ENDGAME){
                disconnectClient(input.client_id);
                printf("%s disconnected id = %d\n", players[input.client_id].name, input.client_id);
            }
            else{
                teste_envia.acao = 'n';

            //     teste_envia.acao = 'n';
            //     teste_envia.tipo = 'G';
            //     teste_envia.id_acao = input.client_id;
            //     for(int i = 0; i < qntJogadores; i++){
            //         teste_envia.todosJogadores[i] = players[i];
            //     }
            //    // printf("acao = %c, id = %d", teste_envia.acao, teste_envia.id_acao);
            //     broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));          
            }
        }
    }
}
