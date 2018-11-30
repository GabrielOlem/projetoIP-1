#include "server.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ACore.h"


//DEFINIÇÕES DAS CONSTANTES
#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define MAX_CLIENTS 6
#define QTD_TIMES 2
#define MAX_CLIENTS_TIMES 5
#define X_MAX 1280
#define Y_MAX 720
#define N_ARMADILHAS 20
#define TEMPO_LIMITE 120
#define DIREITA 0x44            //D
#define ESQUERDA 0x41           //A
#define CIMA 0x57               //W
#define BAIXO 0x53              //S
#define VAZIO 0x30              //0
#define INVALIDO 0x49           //I
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


int main() {
    char client_names[MAX_CLIENTS][LOGIN_MAX_SIZE];
    char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];

    Player players[10];

    char helmetChoice[] = ""; 
    int jogadores = 0, capacete, notReady = 1;
    double tempoInicio, tempoAtual;
    int fim = 0,comecou = 1;
    int time_1 = 0,time_2 = 0;              // Quantidade de indivíduos nas equipes
    int armadilhas_1,armadilhas_2;          // Quantidade de armadilha por pessoa
    char mapa [X_MAX][Y_MAX];
    int xAnterior,yAnterior;
    int scoreBlue = 0, scoreRed = 0;
    int i, j;
    int qntJogadoresProntos = 0;


    PROTOCOLO_JOGO jogada, jogada_server, tempo;                 // Protocolo de envio a ser enviado para o cliente com as infos do jogo;
    
    struct msg_ret_t input;

    //CRIANDO MAPA 
    for(i = 0; i < X_MAX; i++){
        for(j = 0; j < Y_MAX; j++){
            mapa[i][j] = VAZIO;
        }
    }
    //
    
    //INICIALIZAÇÃO DO SERVER
    serverInit(MAX_CLIENTS);
    puts("Server is running!!");
    
    
    //INICIALIZAÇÃO DOS JOGADORES
    while (notReady) {
        PROTOCOLO_INICIAL msg_client, msg_inicial_server;
        int id = acceptConnection();
        input = recvMsg((PROTOCOLO_INICIAL *) &msg_client);
        if (id != NO_CONNECTION) {
            // Recebe o nick, capacete e id das novas conexões
            if(msg_client.tipo == INICIAL ){
                if(jogadores < MAX_CLIENTS){
                    strcpy(players[id].name, msg_client.jogador.name);     // Salva o nick
                    players[id].helmet = msg_client.jogador.helmet;        // Salva o capacete
                    players[id].id = id;                                   // Salva o id         
                
                    printf("%s connected id = %d\n", players[id].name, id);
                
                    // Inicializando os demais atributos do player atual
                    if(jogadores%2 == 0){
                        players[id].team = 1;
                        players[id].position.x = 0;
                        players[id].position.y = 0;
                        time_1++;
                    }
                    else{
                        players[id].team = 2;
                        players[id].position.x = X_MAX;
                        players[id].position.y = Y_MAX;
                        time_2++;
                    }
                    
                    players[id].comBandeira = 0;
                    players[id].ready = 0;
                    players[id].estacongelado = 0;
                    players[id].congelou = 0;
                    players[id].congelamentos = 4;
                    players[id].armadilhas = 3;

                    // Atualizando o player do client após as inicializações dos atributos
                    msg_inicial_server.tipo = INICIAL;
                    msg_inicial_server.jogador = players[id];                
                    sendMsgToClient((PROTOCOLO_INICIAL *) &msg_inicial_server, sizeof(PROTOCOLO_INICIAL), id);

                    jogadores++;                                       
                }
                else{
                    printf("Numero max de clientes conectados ja foi atingido!\n");
                    printf("%s disconnected id = %d\n", players[id].name, id);
                    disconnectClient(id);
                } 
            }
        }
        else{
            if(msg_client.tipo==ENDGAME){
                printf("%s disconnected id = %d\n", players[msg_client.jogador.id].name, msg_client.jogador.id);
                disconnectClient(msg_client.jogador.id);
                msg_client.tipo = -1;
            }
            if(msg_client.tipo == COMECOU){
                qntJogadoresProntos++;
                players[msg_client.jogador.id] = msg_client.jogador;

                if(msg_client.jogador.id == 0){
                    // if(qntJogadoresProntos >= 4){
                        jogada_server.qntJogadores = jogadores;
                        jogada_server.tipo = COMECOU;
                        notReady = 0;
                    // }    
                }
                else{
                    jogada_server.tipo = WAITING;
                    jogada_server.qntJogadores = qntJogadoresProntos;
                }

                for(int i = 0; i < jogadores; i++){
                    jogada_server.todosJogadores[i] = players[i];
                }
                broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));    
            }
        }
    }


    //JOGO
    tempoInicio = al_get_time();
    while( (al_get_time() - tempoInicio < TEMPO_LIMITE) && !fim){
        input = recvMsg((PROTOCOLO_JOGO *) &jogada);
        for(i = 0; i < jogadores; i++){ 
            if((players[i].id == input.client_id)){ 
                // Atualiza o player do server com o player enviado pelo client
                players[i] = jogada.todosJogadores[0];
                // Verifica se o jogador não está congelado
                if(!players[i].estacongelado){
                    // Verifica se o jogador andou para cima
                    if(jogada.tipo == ANDAR_CIMA){
                        jogada_server.tipo = ANDAR_CIMA; 
                        if(players[i].position.y != 0){          // Verifica se não é o fim do mapa
                            if(mapa[players[i].position.x][players[i].position.y-4] == VAZIO || mapa[players[i].position.x][players[i].position.y-4] == TRAP_TEAM_BLUE 
                            || mapa[players[i].position.x][players[i].position.y-4] == TRAP_TEAM_RED ||  mapa[players[i].position.x][players[i].position.y-4] == BANDEIRA_BLUE 
                            || mapa[players[i].position.x][players[i].position.y-4] == BANDEIRA_RED ){
                                 //considerando 0 um espaco livre
                                if(mapa[players[i].position.x][players[i].position.y] == (char)((players[i].id + 97) + 10)){ // se o mapa = indicador do id do player + indicador da trap
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] = VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[i].position.x;
                                jogada_server.yAnterior = players[i].position.y;
                                jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];

                                players[i].position.y -= 4;
                                mapa[players[i].position.x][players[i].position.y] = (char)(players[i].id + 97); // mapa = indicador do id do player

                                if(players[i].team == 1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED){
                                        players[i].estacongelado = 1;
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));
                                    }
                                    else if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE){
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10);
                                    }
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE){
                                        players[i].estacongelado = 1;
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));
                                    }
                                    else if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED){
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10);
                                    }
                                }  
                                
                            }
                        } 
                    }
                    // Verifica se o jogador andou para baixo
                    else if(jogada.tipo == ANDAR_BAIXO){
                        if(players[i].position.y != Y_MAX){
                            jogada_server.tipo = ANDAR_BAIXO;
                            if(mapa[players[i].position.x][players[i].position.y+4] == VAZIO || mapa[players[i].position.x][players[i].position.y+4] == TRAP_TEAM_BLUE
                            || mapa[players[i].position.x][players[i].position.y+4] == TRAP_TEAM_RED || mapa[players[i].position.x][players[i].position.y+4] == BANDEIRA_BLUE
                            || mapa[players[i].position.x][players[i].position.y+4] == BANDEIRA_RED){
                                
                              if(mapa[players[i].position.x][players[i].position.y] == (char)((players[i].id + 97) + 10)){ // se o mapa = indicador do id do player + indicador da trap
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] = VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[i].position.x;
                                jogada_server.yAnterior = players[i].position.y;
                                jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];

                                players[i].position.y += 4;
                                mapa[players[i].position.x][players[i].position.y] = (char)(players[i].id + 97); // mapa = indicador do id do player

                                if(players[i].team == 1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED){
                                        players[i].estacongelado = 1;
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));
                                    }
                                    else if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE){
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10);
                                    }
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE){
                                        players[i].estacongelado = 1;
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));
                                    }
                                    else if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED){
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10);
                                    }
                                }  
                            }  
                        }
                    }
                    // Verifica se o jogador andou para direita
                    else if(jogada.tipo == ANDAR_DIREITA){
                        if(players[i].position.x != X_MAX){
                            if(mapa[players[i].position.x+4][players[i].position.y] == VAZIO || mapa[players[i].position.x+4][players[i].position.y] == TRAP_TEAM_BLUE 
                            || mapa[players[i].position.x+4][players[i].position.y] == TRAP_TEAM_RED || mapa[players[i].position.x+4][players[i].position.y] == BANDEIRA_BLUE 
                            || mapa[players[i].position.x+4][players[i].position.y] == BANDEIRA_RED){
                                
                              if(mapa[players[i].position.x][players[i].position.y] == (char)((players[i].id + 97) + 10)){ // se o mapa = indicador do id do player + indicador da trap
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] = VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[i].position.x;
                                jogada_server.yAnterior = players[i].position.y;
                                jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];

                                players[i].position.x += 4;
                                mapa[players[i].position.x][players[i].position.y] = (char)(players[i].id + 97); // mapa = indicador do id do player

                                if(players[i].team == 1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED){
                                        players[i].estacongelado = 1;
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));
                                    }
                                    else if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE){
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10);
                                    }
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE){
                                        players[i].estacongelado = 1;
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));
                                    }
                                    else if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED){
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10);
                                    }
                                }  
                            }  
                        }
                    }
                    // Verifica se o jogador andou para a esuqerda
                    else if(jogada.tipo == ANDAR_ESQUERDA){
                        if(players[i].position.x != 0){
                            if(mapa[players[i].position.x-4][players[i].position.y] == VAZIO || mapa[players[i].position.x-4][players[i].position.y] == TRAP_TEAM_BLUE 
                            || mapa[players[i].position.x-4][players[i].position.y] == TRAP_TEAM_RED || mapa[players[i].position.x-4][players[i].position.y] == BANDEIRA_BLUE 
                            || mapa[players[i].position.x-4][players[i].position.y] == BANDEIRA_RED){
                                
                              if(mapa[players[i].position.x][players[i].position.y] == (char)((players[i].id + 97) + 10)){ // se o mapa = indicador do id do player + indicador da trap
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] = VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[i].position.x;
                                jogada_server.yAnterior = players[i].position.y;
                                jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];

                                players[i].position.x -= 4;
                                mapa[players[i].position.x][players[i].position.y] = (char)(players[i].id + 97); // mapa = indicador do id do player

                                if(players[i].team == 1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED){
                                        players[i].estacongelado = 1;
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));
                                    }
                                    else if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE){
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10);
                                    }
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE){
                                        players[i].estacongelado = 1;
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));
                                    }
                                    else if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED){
                                        mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10);
                                    }
                                }  
                            }  
                        }
                    }
                    // Verifica se o jogador botou alguma armadilha
                    else if(jogada.tipo == BOTARTRAPS){ 
                        // Verifica se tem armadilhas para botar
                        if(players[i].armadilhas > 0){                                                          
                            jogada_server.tipo = BOTARTRAPS;                                                    
                            // Se for do time azul
                            if(players[i].team == 1){                                                           
                                if(mapa[players[i].position.x][players[i].position.y] != TRAP_TEAM_BLUE){
                                    mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10); // mapa = indicador do id do player + indicador da trap
                                    players[i].armadilhas--;
                                }
                                else{
                                    char msg[] = "armadilha ja existente";
                                    mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));    // mapa = indicador do id do player
                                    sendMsgToClient((char *) msg, sizeof(msg) + 1, players[i].id);
                                }
                            } 
                            // Se for do time vermelho   
                            else{                                                                               
                                if(mapa[players[i].position.x][players[i].position.y] != TRAP_TEAM_RED){
                                    mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97) + 10); // mapa = indicador do id do player + indicador da trap
                                    players[i].armadilhas--;
                                }
                                else{
                                    char msg[] = "armadilha ja existente";
                                    mapa[players[i].position.x][players[i].position.y] = (char)((players[i].id + 97));    // mapa = indicador do id do player
                                    sendMsgToClient((char *) msg, sizeof(msg) + 1, players[i].id);
                                }
                            }
                            jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];
                            jogada_server.xAnterior = players[i].position.x;
                            jogada_server.yAnterior = players[i].position.y;
                        }
                    }
                    // Verifica se o jogador congelou/descongelou alguem 
                    else if(jogada.tipo == CONGELAR){
                        int posi = i + 97;
                        int flag = 1;
                        
                        if(players[i].congelamentos > 0){
                            // Verifica se tem algum player ao redor do player atual e salva a posição dele caso tenha algo
                            if(mapa[players[i].position.x+1][players[i].position.y] >= 97 
                            && mapa[players[i].position.x+1][players[i].position.y] <= 106)
                                posi = (int) mapa[players[i].position.x+1][players[i].position.y];   
                            else if(mapa[players[i].position.x-1][players[i].position.y] >= 97 
                            && mapa[players[i].position.x-1][players[i].position.y] <= 106)
                                posi = (int) mapa[players[i].position.x-1][players[i].position.y];
                            else if(mapa[players[i].position.x][players[i].position.y+1] >= 97 
                            && mapa[players[i].position.x][players[i].position.y+1] <= 106)
                                posi = (int) mapa[players[i].position.x][players[i].position.y+1];
                            else if(mapa[players[i].position.x][players[i].position.y-1] >= 97 
                            && mapa[players[i].position.x][players[i].position.y-1] <= 106)
                                posi = (int) mapa[players[i].position.x][players[i].position.y-1];
                    
                            // Se o player encontrado foi do time adversário, ele é congelado
                            if(players[posi - 97].team != players[i].team){
                                players[posi - 97].estacongelado = 1;
                                
                                //jogada_server.todosJogadores = players[posi - 97]; 
                                jogada_server.tipo = CONGELA;
                                //broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));
                                
                                flag = 0;
                            }
                        
                            if(flag){
                                posi = i + 97;

                                // Verifica se tem algum player congelado ao redor do player atual e salva a posição dele caso tenha algo
                                if(mapa[players[i].position.x+1][players[i].position.y] >= 107 
                                && mapa[players[i].position.x+1][players[i].position.y] <= 116)
                                    posi = (int) mapa[players[i].position.x+1][players[i].position.y];   
                                else if(mapa[players[i].position.x-1][players[i].position.y] >= 107 
                                && mapa[players[i].position.x-1][players[i].position.y] <= 116)
                                    posi = (int) mapa[players[i].position.x-1][players[i].position.y];
                                else if(mapa[players[i].position.x][players[i].position.y+1] >= 107 
                                && mapa[players[i].position.x][players[i].position.y+1] <= 116)
                                    posi = (int) mapa[players[i].position.x][players[i].position.y+1];
                                else if(mapa[players[i].position.x][players[i].position.y-1] >= 107 
                                && mapa[players[i].position.x][players[i].position.y-1] <= 116)
                                    posi = (int) mapa[players[i].position.x][players[i].position.y-1];

                                // Se o player encontrado foi do mesmo time, ele é descongelado
                                if((players[posi - 97].team == players[i].team) && (posi != (i + 97))){
                                    players[posi - 97].estacongelado = 0;
                                    //jogada_server.todosJogadores = players[posi - 97]; 
                                    jogada_server.tipo = DESCONGELA;
                                    //broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));      
                                }
                            }
                            players[i].congelou = 1;
                            players[i].congelamentos--;   
                        }
                                              
                    }

                    // Se ele chegou na bandeira vermelha e ele é do time azul ou Se ele chegou na bandeira azul e ele é do time vermelho = ele tá quase ganhando!
                    if((players[i].position.x == X_FLAG_RED && players[i].position.y == Y_FLAG_RED && players[i].team == 1) || (players[i].position.x == X_FLAG_BLUE && players[i].position.y == Y_FLAG_BLUE && players[i].team == 2)){
                        if(mapa[players[i].position.x][players[i].position.y] == BANDEIRA_RED){
                            players[i].comBandeira = 1;
                            mapa[players[i].position.x][players[i].position.y] = (char)(players[i].id + 97);
                        }
                            
                    }
                    // Se ele chegou na base vermelha, ele é do time vermelho e está com a bandeira azul = ele ganhou uma partida!
                    else if(((players[i].position.x == X_ENTRADA_1_RED && players[i].position.y == Y_ENTRADA_1_RED) || (players[i].position.x == X_ENTRADA_2_RED && players[i].position.y == Y_ENTRADA_2_RED) || (players[i].position.x == X_ENTRADA_3_RED && players[i].position.y == Y_ENTRADA_3_RED)) && (players[i].team == 2) && (players[i].comBandeira == 1)){
                        PROTOCOLO_JOGO msg_final;
                        strcpy(msg_final.mensagem, "Time Vermelho Ganhou!");
                        msg_final.tipo = ENDGAME;
                        broadcast((PROTOCOLO_JOGO *) &msg_final, sizeof(PROTOCOLO_JOGO));
                        fim = 1;
                    }
                    // Se ele chegou na base azul com a bandeira vermelha e ele é do time azul = ele ganhou uma partida!
                    else if(((players[i].position.x == X_ENTRADA_1_BLUE && players[i].position.y == Y_ENTRADA_1_BLUE) || (players[i].position.x == X_ENTRADA_2_BLUE && players[i].position.y == Y_ENTRADA_2_BLUE) || (players[i].position.x == X_ENTRADA_3_BLUE && players[i].position.y == Y_ENTRADA_3_BLUE)) && (players[i].team == 1) && (players[i].comBandeira == 1)){
                        PROTOCOLO_JOGO msg_final;
                        strcpy(msg_final.mensagem, "Time Azul Ganhou!");
                        msg_final.tipo = ENDGAME;
                        broadcast((PROTOCOLO_JOGO *) &msg_final, sizeof(PROTOCOLO_JOGO));
                        fim = 1;
                    }
                    
                    if(fim != 1){
                        for(int i = 0; i < jogadores; i++){
                            jogada_server.todosJogadores[i] = players[i];
                        }
                        
                        jogada_server.tipo = GAME;
                        broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));
                    }
                }
            }
        }
        tempoAtual = al_get_time() - tempoInicio;
        tempo.tipo = TEMPO;
        sprintf(tempo.mensagem, "%lf", tempoAtual);
        broadcast((PROTOCOLO_JOGO *) &tempo, sizeof(PROTOCOLO_JOGO)); 
    }

    return 0;
}