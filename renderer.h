#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#define TOTAL_CARTAS_JOGO 20

typedef enum {
    ESTADO_ESCOLHER_CARTA,
    ESTADO_ESCOLHER_ALVO,
    ESTADO_MENU,
    ESTADO_VITORIA,
    ESTADO_DERROTA
} GameState;


typedef struct{
  int frame_width;
  int frame_height;
  int total_frames;
  

  int current_frame;
  float frama_delay;
  float frame_timer;
} Animacao;


typedef enum {
    TIPO_ATAQUE,
    TIPO_DEFESA,
    TIPO_ESPECIAL
} TipoCarta;

typedef struct {
    int id;             // Identificador único (0, 1, 2...)
    char nome[30];      // Nome para exibir (ex: "Soco Fraco")
    char descricao[30];
    TipoCarta tipo;     
    
    int custo;          // Quanto gasta de energia
    int valor_ataque;   // Se for defesa, isso é 0
    int valor_escudo;   // Se for ataque, isso é 0
    int efeito_especial;// Código do efeito (0 = nenhum)
    
    ALLEGRO_BITMAP* arte; // A imagem da carta
} Carta;

extern Carta BIBLIOTECA[TOTAL_CARTAS_JOGO];

typedef struct {
    TipoCarta tipo; // TIPO_ATAQUE ou TIPO_DEFESA
    int custo;      // Nível de força (baseado no custo das cartas do player)
} AcaoMob;

typedef struct {

    // Posição e Tamanho
    float x, y;       
    int posicao;   
    int width, height;   
    float scale;         

    // Atributos de Jogo
    float vida_atual;
    int vida_maxima;
    int escudo;

    AcaoMob ia_ciclo[3]; // Máximo de 3 ações por ciclo (regra do mob forte)
    int ia_total_passos; // Quantas ações tem nesse ciclo (1, 2 ou 3)
    int ia_passo_atual;  // Qual ação ele vai executar agora (0, 1 ou 2)

    // Imagem
    ALLEGRO_BITMAP *sprite; 
    Animacao anim;


} Entidade;

typedef struct {
    // Posição e Tamanho
    float x, y;       
    int posicao;   
    int width, height;   
    float scale;         

    // Atributos de Jogo
    float energia_max;
    int energia_atual;

    // Imagem
    ALLEGRO_BITMAP *sprites[4]; 
    

} Energia;



typedef struct {
  ALLEGRO_DISPLAY* display;
  ALLEGRO_BITMAP* display_buffer;

  ALLEGRO_FONT* font;

  Entidade player;
  Entidade mob_fraco_1;
  Entidade mob_medio;
  Entidade mob_forte;
  Entidade boss;

  Energia energia;

  ALLEGRO_BITMAP *mapa_00;
  ALLEGRO_BITMAP *mapa_01;
  //tela de vitria
  ALLEGRO_BITMAP *victory_screen;
  ALLEGRO_BITMAP *defeat_screen;
  ALLEGRO_BITMAP *card_art; 

  //Pilha de compra
  int deck[20];
  int deck_tamanho;    
  ALLEGRO_BITMAP* compra;

  // Pilha de descarte
  int descarte[20];
  int descarte_tamanho;
   ALLEGRO_BITMAP* descarte_img;

  int mao[5];   
  int mao_tamanho;      

  int indice_selecionado;

  ALLEGRO_BITMAP* seta_alvo; // A imagem da seta/mira
  GameState estado_jogo;     // 0 = carta, 1 = alvo
  int alvo_selecionado;      // 0 = fraco1, 1 = medio, 2 = forte, 3 = boss
  
  
  ALLEGRO_BITMAP *icone_intencao_ataque;
  ALLEGRO_BITMAP *icone_intencao_defesa;
  
  int round_atual;

  //menu principal
  int botao_selecionado; // jogar = 0, sair = 1;
  ALLEGRO_BITMAP* botao_menu;

  
  

} Renderer;


void IniciarNovaRodada(Renderer* renderer);

void IniciarRodadaBoss(Renderer* renderer);

void RenderVictory(Renderer* renderer);

void FillRenderer(Renderer* renderer);

void RenderMenu(Renderer* renderer);

ALLEGRO_BITMAP* CarregarArtePorCusto(TipoCarta tipo, int custo);

void Render(Renderer* renderer);

void ClearRenderer(Renderer* renderer);

void InicializarBibliotecaCartas(); 

void IniciarDeckCombate(Renderer* renderer);

void ComprarCarta(Renderer* renderer);

void RenderCard(const Renderer* renderer, int x, int y, int id_carta, int is_selected);

void RenderPlayerHand(Renderer* renderer);

void ExecutarTurnoInimigos(Renderer* renderer);

void IniciarTurnoJogador(Renderer* renderer);

void RenderDiscard(Renderer* renderer, int x, int y);

#endif