/*************************************************************
**                                                           *
*         Trabalho de Programação 2                          *
*         Programado em Linux 64 BITS                        *
*         Necessita das Bibliotecas:                         *
*         	- TIME.H                                      *
*         	- STRING.H                                    *
*         	- CURSES.H                                    *
*         Para compilar use:                                 *
*  gcc -o minefield minefield.c -lncurses                    *
*                                                            *
*                                                           **
*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <string.h>


/*************************************************************
**                                                           *
*         Definição de Macros e Constantes                   *
*                                                           **
*************************************************************/

#define STRUCT typedef struct

//controle do tabuleiro
#define MAX_BOMBS 10
#define MAX_W 10
#define MAX_H 10

//controle das casas
#define BOMB 9
#define TRUE 1
#define FALSE 0

//controle de cores
#define SCREEN 10
#define DISARM 11
#define D_BOMBS 12
#define W_DISARMS 13

//controle do jogo -  game_status
#define GAME_OVER game_status = 0
#define GAME_START game_status = 1
#define GAME_WIN explosion = 0
#define GAME_LOSE explosion = 1
#define HASH_CODE 4
#define START_SCREEN 3
#define INSTRUCTION 2
#define GAMING 1
#define QUIT 0
#define START_GAME 5
#define HIGHSCORES 6

//controle do tempo
#define GAMING_TIME 500
#define DEFAULT_TIME -1


/*************************************************************
**                                                           *
*           Declaração de Structs e Tipos                    *
*                                                           **
*************************************************************/

STRUCT reg_casa Casa;
STRUCT reg_score Score;

struct reg_casa{
	int value;
	unsigned int disarmed:1;
	unsigned int active:1;
};

struct reg_score{
	char nome[50];
	char hash_win[21];
	unsigned int game_time;
	
};


/*************************************************************
**                                                           *
*           Variáveis Globais                                *
*                                                           **
*************************************************************/

Casa tabuleiro[MAX_W][MAX_H];
int squares;
int disarms;
int x,y;
int game_status = START_SCREEN;
int explosion;
int time_start = 0, time_atual;
char IGO_str[21];
char hash[51];
Score list[10];
FILE *SCORES_file;


/*************************************************************
**                                                           *
*           Declaração de Funções                            *
*                                                           **
*************************************************************/

//funções de controle
void CTRL_start(); //abre as funções de sistema
int CTRL_end(); //termina um jogo
void CTRL_tela_inicial(); // controla o cursor da tela inicial
void CTRL_startgame(); // inicia um jogo
void CTRL_closegame(); //fecha as funções do sistema
void CTRL_start_colors(); //define as cores que o sistema vai usar
void CTRL_game_win(); // controla a vitoria de um jogo
Score CTRL_score_generator(); // gera uma struct para ser guardada no arquivo
void CTRL_scores(); // controla a tela de pontuação

//funções de inicialização do tabuleiro
void TAB_init_tab(); // inicia um tabuleiro zerado
void TAB_init_bombs(); // randomiza 10 bombas no tabuleiro
void TAB_init_squares(); // inicia as casas que não tem bombas, usando a TAB_bombcounter
Casa TAB_put_square(int value); // cria uma struct Casa, que vai para o tabuleiro
int  TAB_bombcounter(int x, int y); // conta o numero de bombas que existem nas casas visinhas

// funções de jogo
void PLAY_move(int key_pressed); // executa uma função dependendo da tecla apertada
void PLAY_open(int o, int p); // abre a casa de coordenada (o,p)
void PLAY_disarm(int o,int p); // desarma a casa de coordenada (o,p)
void PLAY_bigopen(int o, int p); // abre as casas vizinhas de coordenada (o,p)

//funções gráficas
void GRAPH_tela_inicial(); // exibe a tela inicial
void GRAPH_move(int i, int j); // move o cursor no tabuleiro do jogo
void GRAPH_print_cell(Casa quadro); // imprime a casa do jogo
void GRAPH_print_field(); // imprime um campo 
void GRAPH_disarm(int action); // imprime um desarme ou rearme 
void GRAPH_print_scores(); // imprime os dados do jogo no lado do tabuleiro
void GRAPH_print_instructions(int instr); // imprime as instruções no lado do tabuleiro
void GRAPH_print_color(int color); // imprime alguma coisa usando as cores defininas na CTRL_start_colors();
void GRAPH_game_end(); // imprime a tela do final do jogo
void GRAPH_game_win(); // imprime a mensagem de vitoria
void GRAPH_game_lose(); // imprime a mensagem de derrota
void GRAPH_timer(int at); // escreve o timer na tela
void GRAPH_instructions(); // imprime as instruções da tela inicial
void GRAPH_print_score(Score s); // imprime uma pontuação

//funções de tempo
void TIMER_start(); // inicia o contador de tempo
int TIMER_atual(); // retorna o tempo decorrido do inicio da partida
void TIMER_reset(); // reseta o contador de tempo

//funcoes de hashcode
void IGO_generate(); // gera o hashcode base
int IGO_search(char a); // retorna o valor do caractere 'a'
int IGO_getx(char a, int pos); // retorna o valor x da coordenada de a
int IGO_gety(char a, int pos); // retorna o valor y da coordenada de a
void IGO_generatecode(); // gera o hashcode a partir de um tabuleiro
int IGO_generatecoords(char *code); // monta um tabuleiro a partir de um hashcode

/*************************************************************
**                                                           *
*           Programa Principal                               *
*                                                           **
*************************************************************/

int main (){
	int key;
	int file_size;
	CTRL_start();
	do{
	switch(game_status){
		case START_SCREEN: // tela inicial
			CTRL_tela_inicial();
			break;
		case INSTRUCTION: // tela de instuções
			GRAPH_instructions();
			getch();
			game_status = START_SCREEN;
			break;
		case START_GAME: // inicio de jogo normal
			CTRL_startgame();
			TAB_init_tab();
			clear();
			game_status = GAMING;
			break;
		case GAMING: // jogo correndo
			GRAPH_print_field();
			while(game_status == GAMING){
				PLAY_move(getch());
			}
			game_status = CTRL_end();
			break;
		case HASH_CODE: // inicio de jogo com hash
			CTRL_startgame();
			GRAPH_print_field();
			move(3,60);
			printw("                                    ");
			move(3,61);
			echo();
			scanw("%s",IGO_str);
			noecho();
			IGO_generatecoords(IGO_str);
			clear();
			game_status = GAMING;
			break; 
		case HIGHSCORES: // tela de pontuação
			CTRL_scores();
			game_status = START_SCREEN;
			break;	
		}
	}while(game_status != QUIT);
	CTRL_closegame();
	return 0;
}


/*************************************************************
**                                                           *
*         Implementação das Funções                          *
*                                                           **
*************************************************************/

void CTRL_startgame(){
	explosion = FALSE;
	disarms = 0;
	x = 5;
	y = 5;
	clear();
}

void CTRL_tela_inicial(){
	int i,k=32,j = 37;
	clear();
	GRAPH_tela_inicial();
	game_status = START_GAME;
	move(32, 37);
	do{
		i = getch();
		switch(i){ // setas do teclado
			case 65:
				if (k != 32) k-=2;
				break;
			case 66:
				if (k != 36) k+=2;
				if (k == 36) j = 37;
				break;
			case 67:
				if (j != 58 && k != 36) j+=21;
				break;
			case 68:
				if (j != 37) j-=21;
				break;
		}
		switch(k){
			case 32:
				if (j == 58) game_status = HASH_CODE;
				if (j == 37) game_status = START_GAME;
				break;
			case 34:
				if (j == 58) game_status = HIGHSCORES;
				if (j == 37) game_status = INSTRUCTION;
				break;
			case 36: 
				game_status = QUIT;
				break;
		}
		move(k,j);
	}while(i != 10);
	clear();
}

void CTRL_closegame(){
    endwin(); 	//	Sempre que finalizarmos um programa com a biblioteca curses, 
    exit(0);   //	devemos executar este comando.   
}

void CTRL_start(){
	initscr(); // inicia a tela separada do terminal
	noecho(); // bloqueia a escrita na tela
	start_color(); // inicia as cores da biblioteca curses.h
	use_default_colors(); // permite o uso das cores do terminal padrão 
	CTRL_start_colors(); // inicia as cores que serão utilizadas
	cbreak(); // permite ao sistema interromper a espera de uma entrada de teclado
	IGO_generate(); // gera a hash base

}

int CTRL_end(){
	int i,k = START_GAME;
	TIMER_reset();
	if(!explosion){
		CTRL_game_win();
		GRAPH_game_end();
		GRAPH_game_win();
	}
	else{
		GRAPH_game_end();
		GRAPH_game_lose();
	}
	move(14,53);
	do{
		i = getch();
		if(i == 68) {
			move(14,53);
			k = START_GAME;
		}
		if(i == 67){
			move(14,65);
			k = START_SCREEN;
		}
	}while(i != 10);
	
	return k;
}

void CTRL_start_colors(){
	int i;
	// init_pair(Nome da cor, cor do texto, cor do fundo);
	// -1 é a cor padrão do terminal
	init_pair(BOMB,COLOR_BLACK,COLOR_RED); 
	init_pair(SCREEN,COLOR_BLACK,COLOR_WHITE);
	init_pair(DISARM,COLOR_RED,COLOR_WHITE);
	init_pair(D_BOMBS,COLOR_BLACK,COLOR_YELLOW);
	init_pair(W_DISARMS,COLOR_WHITE,COLOR_RED);
	for(i=1;i<8;i++){
		init_pair(i,i,-1);
	}
	init_pair(8,COLOR_BLACK,-1);
}

void CTRL_game_win(){
	int i,j,pos = 10;
	Score k = CTRL_score_generator();
	SCORES_file = fopen("scores.mfs","ab+"); // abre o arquivo
	fread(&list[0], sizeof(struct reg_score), 10, SCORES_file); // lê as 10 posições e as coloca num vetor
	fclose(SCORES_file); // fecha o arquivo
	for(i=0;i<10;i++){ // confere o lugar em que o jogador ficou
		if(list[i].game_time == 0 || list[i].game_time > k.game_time){
		pos = i;
		break;
		}
	}
	if (pos != 10){ // se o jogador ficou antes do 10º lugar, ele entrará no quadro
		move(9,47);
		printw("* * * * * * * * * * * * * * *");
		for(i=10;i<19;i++){
			move(i,47);
			printw("*                           *");
		}
		move(19,47);
		printw("* * * * * * * * * * * * * * *");
		move(12, 55);
		printw("Parabéns,");
		move(13, 49);	
		printw("voce entrou na pontuacao");
		move(14, 49);	
		printw("Digite o seu nome:");
		move(15,55);
		echo();
		scanw("%s",k.nome);
		strcpy(k.hash_win,IGO_str);
		noecho();
		for(j = 9; j>pos; j--){ // joga todos, a partir da posição conseguida pelo jogador, uma posição para tras
			list[j] = list[j-1];
		}
		list[pos] = k;
	}
	SCORES_file = fopen("scores.mfs","wb+"); // abre e apaga o arquivo, e grava os 10 registros novos
	rewind(SCORES_file);
	fwrite(&list[0], sizeof(struct reg_score), 10, SCORES_file);
	fclose(SCORES_file);
}

Score CTRL_score_generator(){
	Score k;
	k.game_time = time_atual;
	return k;
}

void CTRL_scores(){
	int i;
	move(3, 10);
	printw("Highscores:");

	SCORES_file = fopen("scores.mfs","ab+");
	fread(&list[0], sizeof(struct reg_score), 10, SCORES_file);
	fclose(SCORES_file);
	
	for(i=0;i<10;i++){
		move ((2*i)+5,12);
		printw("%d: ",i+1);
		if(list[i].game_time != 0) 
		GRAPH_print_score(list[i]);
	}
	move ((2*i)+5,8);
	
	printw("Pressione qualquer tecla para voltar a tela inicial.");
	getch();
}

void TAB_init_tab(){
	int i,j;
	for(i=0 ; i < MAX_H ; i++){
		for(j=0 ; j < MAX_W ; j++){
			tabuleiro[i][j].value = 0; // valor da casa = 0
			tabuleiro[i][j].active = FALSE; // casa não ativa
			tabuleiro[i][j].disarmed = FALSE; // casa não desarmada
		}
	}
	TAB_init_bombs(); 
	TAB_init_squares();
	squares = (MAX_H * MAX_W) - MAX_BOMBS; 
}

void TAB_init_bombs(){
	int i,x,y;
	int coords[2][10];
	srand( (unsigned)time(NULL) );
	for(i=0 ; i < MAX_BOMBS ; i++){
		x = rand()%10; //sorteia 2 numeros entre 0 e 10
		y = rand()%10;
		coords[0][i] = x;
		coords[1][i] = y;		
		if (tabuleiro[x][y].value == BOMB) i--;
		tabuleiro[x][y] = TAB_put_square(BOMB);
	}
	IGO_generatecode(coords); // gera o codigo da partida
}

void TAB_init_squares(){
	int i,j;
	for(i=0 ; i < MAX_H ; i++){
		for(j=0 ; j < MAX_W ; j++){
			if(tabuleiro[i][j].value != BOMB)
			tabuleiro[i][j] = TAB_put_square(TAB_bomb_counter(i,j));
		}
	}
}

Casa TAB_put_square(int value){
	Casa x;
	x.value = value;
	x.active = FALSE;
	x.disarmed = FALSE;
	return x;
}

int TAB_bomb_counter(int o, int p){
	int count = 0, n,i,j;
	for(n=0; n<9;n++){
		i= o + n%3 - 1;
		j= p + n/3 - 1;
		if(i != -1 && i != MAX_H && j != -1 && j != MAX_W){
			if (tabuleiro[i][j].value == BOMB) count++;
		}
	}
	return count;
}

int TAB_disarm_counter(int o, int p){
	int count = 0, n,i,j;
	for(n=0; n<9;n++){
		i= o + n%3 - 1;
		j= p + n/3 - 1;
		if(i != -1 && i != MAX_H && j != -1 && j != MAX_W){
			if (tabuleiro[i][j].disarmed == TRUE) count++;
		}
	}
	return count;
}

void PLAY_move(int key_pressed){
	switch(key_pressed){
	case 65:
		if( y == 0) break;
		GRAPH_move(--y,x);
		break;
	case 66:
		if( y == (MAX_H-1)) break;
		GRAPH_move(++y,x);
		break;
	case 67:
		if( x == (MAX_W-1)) break;
		GRAPH_move(y,++x);
		break;
	case 68:
		if( x == 0) break;
		GRAPH_move(y,--x);
		break;
	case 99: //c
	case 10: //enter
		PLAY_open(x,y);
		GRAPH_move(y,x);
		break;
	case 100: //d
		PLAY_disarm(x,y);
		GRAPH_move(y,x);
		break;
	case 102: //f
		PLAY_bigopen(x,y);
		GRAPH_move(y,x);
		break;
	case 105: //i
		GRAPH_print_instructions(TRUE);
		GRAPH_move(y,x);
		break;
	case 104: //h
		//char igo_hash[20];
		break;
	default:
		break;
	}
	switch(key_pressed){
/*	case 65:
	case 66:
	case 67:
	case 68:*/
	case 99: //c
	case 10: //enter
	case 100: //d
	case 102: //f
		if(time_start == 0) TIMER_start();
	default:
		break;
	}
	GRAPH_timer(TIMER_atual());
	GRAPH_print_scores();
	GRAPH_move(y,x);
}

void PLAY_open(int o,int p){
	int i,j,n;
	if (tabuleiro[o][p].active == TRUE || tabuleiro[o][p].disarmed == TRUE)  // não abre a casa já aberta ou desarmada
		return;
		
	GRAPH_move(p,o);
	tabuleiro[o][p].active = TRUE;
	GRAPH_print_cell(tabuleiro[o][p]);
	if(tabuleiro[o][p].value == BOMB){
		GAME_OVER;
		GAME_LOSE;
		return;
	}
	squares--;
	if (squares == 0){
		GAME_OVER;
		GAME_WIN;
		return;
	}
	if(tabuleiro[o][p].value == FALSE){ // se é uma casa vazia, ela deve abrir todas as casas vizinhas
		for(n=0; n<9;n++){
			i= o + n%3 - 1;
			j= p + n/3 - 1;
			if((i != -1 && i != 10 && j != -1 && j != 10) && 
				(tabuleiro[i][j].active == FALSE)){
				PLAY_open(i,j);
			}
		}
	} 
}


void PLAY_disarm(int o,int p){
	if(tabuleiro[o][p].active == TRUE) // se a casa esta aberta, a função não faz nada
		return;
	GRAPH_move(p,o);
	if(tabuleiro[o][p].disarmed){ // se a casa esta desarmada, ela é rearmada
		tabuleiro[o][p].disarmed = FALSE;
		disarms--;
	}
	else{ // se a casa está armada, ela é desarmada
		tabuleiro[o][p].disarmed = TRUE;
		disarms++;
	}
	GRAPH_disarm(tabuleiro[o][p].disarmed);	// imprime a casa armada ou desarmada
	return;
}

void PLAY_bigopen(int o, int p){
	int i,j,n;
	if(tabuleiro[o][p].active == FALSE) return;
	if(tabuleiro[o][p].value == TAB_disarm_counter(o,p)){
		for(n=0; n<9;n++){
			i= o + n%3 - 1;
			j= p + n/3 - 1;
			if((i != -1 && i != 10 && j != -1 && j != 10) && 
				(tabuleiro[i][j].active == FALSE)){
				PLAY_open(i,j);
			}
		}
	}
}

//funções gráficas
void GRAPH_tela_inicial(){
	int o,p,i,j,n;
	int title[14][28] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,12,12,12,0,12,12,12,0,12,12,12,12,12,0,12,12,12,0,12,12,12,0,0,0},
		{0,0,0,0,12,0,0,0,12,0,12,0,12,0,12,0,12,0,12,0,12,0,12,0,12,0,0,0},
		{0,0,0,0,12,0,0,0,12,12,12,0,12,0,12,0,12,0,12,12,12,0,12,0,12,0,0,0},
		{0,0,0,0,12,12,12,0,12,0,12,0,12,0,12,0,12,0,12,0,0,0,12,12,12,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,12,12,12,12,12,0,9,0,12,12,0,12,0,12,12,12,0,12,12,0,0,12,12,12,0,0},
		{0,0,12,0,12,0,12,0,12,0,12,0,12,12,0,12,0,12,0,12,0,12,0,12,0,12,0,0},
		{0,0,12,0,12,0,12,0,12,0,12,0,12,12,0,12,12,12,0,12,0,12,0,12,0,12,0,0},
		{0,0,12,0,12,0,12,0,12,0,12,0,0,12,0,12,0,12,0,12,12,0,0,12,12,12,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
	for (o = 0; o < 14; o++){
		for (p = 0; p < 28; p++){
			for(n=0; n<9;n++){
				if(title[o][p] != BOMB && title[o][p] != D_BOMBS){
					i= o + n%3 - 1;
					j= p + n/3 - 1;
					if(i != -1 && i != 14 && j != -1 && j != 28){
						if (title[i][j] == BOMB || title[i][j] == D_BOMBS) title[o][p]++;
					}	
				}
			}

		}
	}
	for (o = 0; o < 14; o++){
		for (p = 0; p < 28; p++){
			if(title[o][p] == 0) title[o][p] = 10;
			GRAPH_move(o,p);
			GRAPH_print_color(title[o][p]);
		}
	}
	move(32, 36);
	printw("( ) Novo Jogo        ( ) Novo Jogo com Hashcode");
	move(34, 36);
	printw("( ) Instrucoes       ( ) Highscores");
	move(36, 36);
	printw("( ) Sair");
}

void GRAPH_move(int i, int j){
	move((i*2)+3, (j*4)+5); // move o cursor de acordo com o tabuleiro
}

void GRAPH_print_cell(Casa quadro){
	GRAPH_print_color(quadro.value);
}

void GRAPH_print_field(){ // imprime o tabuleiro do jogo na tela
	int i,j;
	for(i=0 ; i < 10 ; i++){
		for(j=0 ; j < 10 ; j++){
			GRAPH_move(i,j);
			GRAPH_print_color(SCREEN);
		}
	}
	GRAPH_print_scores();
	GRAPH_print_instructions(FALSE);
	move(7,50);
	printw("Tempo : 0: 0");
	GRAPH_move(y,x);
		
}

void GRAPH_disarm(int action){ 
	enum {rearm, disarm};
	if (action == rearm){
		GRAPH_print_color(SCREEN);
		return;
	}
	if (action == disarm){
		GRAPH_print_color(DISARM);
		return;
	}
}

void GRAPH_print_scores(){
	move(3,50);
	printw("Hash code: %s",IGO_str);
	move(4,50);
	printw("Pontos:");
	move(5,50);
	printw("Casas abertas: %2d/%2d   ",squares,(MAX_H*MAX_W)-MAX_BOMBS);
	move(6,50);
	printw("Casas desarmadas: %d/10",disarms);
	
}

void GRAPH_print_instructions(int instr){
	if(instr == FALSE){
		move(9,50);
		printw("Aperte (i) para ver");	
		move(10,50);
		printw("  as instruções");	
	}
	else {
		move(9,50);
		printw("Instruções               ");
		move(10,50);
		printw("(c) - Escolher a casa    ");
		move(11,50);
		printw("(d) - Desarmar a casa    ");
		move(12,50);
		printw("(f) - Abre as casas em ");
		move(13,50);
		printw("  volta quando possível");
		
	}
}

void GRAPH_print_color(int color){
	attron(COLOR_PAIR(color));
  	attron(A_BOLD);
  	switch(color){
  		case BOMB:
  		case D_BOMBS:
  			printw("\b * ");
  			break;
		case DISARM:
		case W_DISARMS:
			printw("\b P ");
			break;
		case SCREEN:
		case 0:
			printw("\b   ");
			break;
		default:
			printw("\b %d ",color);
			GRAPH_move(y,x);
			break;
  	}	
  	attroff(A_BOLD);
	attroff(COLOR_PAIR(color));
}

void GRAPH_game_end(){
	int i;
	move(9,47);
	printw("* * * * * * * * * * * * * * *");
	for(i=10;i<19;i++){
		move(i,47);
		printw("*                           *");
	}
	move(19,47);
	printw("* * * * * * * * * * * * * * *");
	move(13, 49);
	printw(" Deseja jogar novamente?");
	move(14, 49);
	printw("   ( ) Sim     ( )Nao");
	move(14,53);
	
}

void GRAPH_game_win(){
	move(10, 49);
	printw("       Voce Venceu!");
}

void GRAPH_game_lose(){
	int i,j;
	for (i= 0;i<10;i++){
		for (j= 0;j<10;j++){
			if(tabuleiro[i][j].value == BOMB && tabuleiro[i][j].active == FALSE && tabuleiro[i][j].disarmed == FALSE){
				GRAPH_move(j,i);
				GRAPH_print_color(D_BOMBS);
			}
			if(tabuleiro[i][j].value != BOMB && tabuleiro[i][j].disarmed == TRUE){
				GRAPH_move(j,i);
				GRAPH_print_color(W_DISARMS);
			}
		}
	}
	move(10, 49);
	printw("       Voce Perdeu!");	
}

void GRAPH_timer(int at){ // escreve o timer na tela, depois volta para a posição que o cursor estava
	int min, seg;
	min = at/60;
	seg = at%60;
	move(7,50);
	fflush(stdout);
	printw("Tempo :%2d:%2d",min,seg); 
	GRAPH_move(y,x);
	refresh();
}

void GRAPH_instructions(){
	int i;
	GRAPH_move(0,0);
	printw("Instrucoes:");
	GRAPH_move(1,0);
	printw("Comandos:");
	GRAPH_move(2,1);
	printw("Use as SETAS do teclado para mover o cursor;");
	GRAPH_move(3,1);
	printw("Aperte (C) ou ENTER para escolher a casa onde o cursor está;");
	GRAPH_move(4,1);
	printw("Aperte (D) para desarmar ou rearmar a casa atual - a casa não poderá ser aberta enquanto desarmada;");
	GRAPH_move(5,1);
	printw("Aperte (F) para abrir todas as casas em volta da casa atual - e necessaria que as casas em volta sejam desarmadas;");
	GRAPH_move(6,0);
	printw("Casas:");
	GRAPH_move(7,1);
	GRAPH_print_color(SCREEN);
	GRAPH_move(7,2);
	printw("Casas que não foram abertas;");
	for(i = 0;i<9;i++){
		GRAPH_move(8,i);
		GRAPH_print_color(i);
	}
	GRAPH_move(8,i);
	printw("Casas sem bombas (O numero das casas e correspondente ao numero de bombas nas casas vizinhas);");
	GRAPH_move(9,1);
	GRAPH_print_color(BOMB);
	GRAPH_move(9,2);
	printw("Bomba que foi explodida pelo Jogador;");
	GRAPH_move(10,1);
	GRAPH_print_color(D_BOMBS);
	GRAPH_move(10,2);
	printw("Bombas que não foram desarmadas;");
	GRAPH_move(11,1);
	GRAPH_print_color(DISARM);
	GRAPH_move(11,2);
	printw("Casa desarmada;");
	GRAPH_move(12,1);
	GRAPH_print_color(W_DISARMS);
	GRAPH_move(12,2);
	printw("Casa que foi desarmada mas nao era uma bomba;");
	GRAPH_move(14,1);
	printw("Pressione qualquer tecla para voltar a tela inicial.");
}

void GRAPH_print_score(Score s){
	printw("%s - %d:%d min - %s",s.nome,s.game_time/60,s.game_time%60,s.hash_win);
	
}

// funções do timer
void TIMER_start(){
	time_start = time(NULL); // pega o tempo em seg do sistema
	timeout(GAMING_TIME); // define o tempo que o sistema esperará a entrada do teclado
}

int TIMER_atual(){
	if(time_start == 0) return 0; 
	time_atual = time(NULL)	- time_start; // faz a conta hora inicial - hora atual = tempo gasto
	return  time_atual;
	
}

void TIMER_reset(){
	timeout(DEFAULT_TIME); //não terá mais tempo máximo para entrar com algum dado
	time_start = 0; // reseta o timer
}

//funções de hashcode

void IGO_generate(){ 
	int i,k = 0,o,p;
	char starting[51];
	for(i =65; i<90;i++){
		starting[k] = (char) i;
		k++;
	}
	for(i =97; i<122;i++){
		starting[k] = (char) i;
		k++;
	}
	for(i = 0; i< 50;i++){
		p = rand()%k;
		hash[i] = starting[p];
		
		for(o = p;o< k;o++){
			starting[o] = starting[o+1];
		} 
		k--;
	}
	hash[50] = '\0';
}

int IGO_search(char a){
	int i;
	for(i=0;i<52;i++){
		if(hash[i] == a) return i; 
	}	
}

int IGO_getx(char a, int pos){
	return (2*IGO_search(a)+pos%2)/10;
}

int IGO_gety(char a, int pos){
	return (2*IGO_search(a)+pos%2)%10;
}

char IGO_getchar(int x, int y){
	int i;
	i =(x*10 + y - (y%2))/2;
	return hash[i];
}

int IGO_generatecoords(char *code){
	int i = 0,j,o,p;
	int k = 0;
	int IGO_coords[2][10];
	for(o=0 ; o < MAX_H ; o++){
		for(p=0 ; p < MAX_W ; p++){
			tabuleiro[o][p].value = 0;
			tabuleiro[o][p].active = FALSE;
			tabuleiro[o][p].disarmed = FALSE;
		}
	}
	while (code[i] != '\0'){
		if(code[i] != 'z'){
			o = IGO_getx(code[i],i);
			p = IGO_gety(code[i],i);
			k++;
			tabuleiro[o][p].value = BOMB;
		}
		i++;
	}
	TAB_init_squares();
	squares = (MAX_H * MAX_W) - MAX_BOMBS;
	return k;
}

void IGO_generatecode(int IGO_coords[2][10]){
	int i,o=-2,p = -1,x,y;
	char key;
	for(i = 0;i<20;i++){
		IGO_str[i] = 'z';
	}
	IGO_str[20] = '\0'; 
	for(i = 0;i<10;i++){
		key = IGO_getchar(IGO_coords[0][i],IGO_coords[1][i]);
		if(IGO_coords[1][i]%2 == 0){
			o+=2;
			IGO_str[o] = key;	
		}
		else {
			p+=2;
			IGO_str[p] = key;
		}
	}
	if(p>o) IGO_str[p+1] = '\0';
	else IGO_str[o+1] = '\0';
}

