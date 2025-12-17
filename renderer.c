#include "renderer.h"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "utils.h"

Carta BIBLIOTECA[TOTAL_CARTAS_JOGO];

void DrawScaledText(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y,
                    float xscale, float yscale, int alignment,
                    const char* text) {
  ALLEGRO_TRANSFORM transform;
  al_identity_transform(&transform);
  
  // Aplica o zoom (escala)
  al_scale_transform(&transform, xscale, yscale);
  al_use_transform(&transform);

  
  al_draw_text(font, color, x / xscale, y / yscale, alignment, text);
  
  al_identity_transform(&transform);
  al_use_transform(&transform);
}

void DrawCenteredScaledText(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x,
                            float y, float xscale, float yscale, char* text) {
  DrawScaledText(font, color, x, y, xscale, yscale, ALLEGRO_ALIGN_CENTRE, text);
}
// Função auxiliar para garantir que o mob tenha pelo menos 1 ataque
void ValidarPeloMenosUmAtaque(Entidade* mob) {
    int tem_ataque = 0;
    for(int i = 0; i < mob->ia_total_passos; i++) {
        if (mob->ia_ciclo[i].tipo == TIPO_ATAQUE) tem_ataque = 1;
    }
    // Se não tiver ataque, força a primeira ação a ser ataque
    if (!tem_ataque) {
        mob->ia_ciclo[0].tipo = TIPO_ATAQUE;
    }
}

void ConfigurarIA_Fraco(Entidade* mob) {
    // Regra: 1 ou 2 ações
    mob->ia_total_passos = (rand() % 2) + 1; 
    mob->ia_passo_atual = 0;

    for (int i = 0; i < mob->ia_total_passos; i++) {
        // Regra: Tipo Ataque ou Defesa (50% chance)
        mob->ia_ciclo[i].tipo = (rand() % 2) == 0 ? TIPO_ATAQUE : TIPO_DEFESA;
        
        // Regra: Custo 0 ou 1
        mob->ia_ciclo[i].custo = rand() % 2; 
    }
    
    ValidarPeloMenosUmAtaque(mob);
}

void ConfigurarIA_Medio(Entidade* mob) {
    // Regra: 1 ou 2 ações
    mob->ia_total_passos = (rand() % 2) + 1; 
    mob->ia_passo_atual = 0;

    for (int i = 0; i < mob->ia_total_passos; i++) {
        // Regra: Tipo Ataque ou Defesa (50% chance)
        mob->ia_ciclo[i].tipo = (rand() % 2) == 0 ? TIPO_ATAQUE : TIPO_DEFESA;
        
        // Regra: Custo 0 ou 1
        mob->ia_ciclo[i].custo = rand() % 2 + 1; 
    }
    
    ValidarPeloMenosUmAtaque(mob);
}

void ConfigurarIA_Forte(Entidade* mob) {
    // Regra: 2 ou 3 ações
    mob->ia_total_passos = (rand() % 2) + 2; 
    mob->ia_passo_atual = 0;

    int contagem_nivel_1 = 0;

    for (int i = 0; i < mob->ia_total_passos; i++) {
        mob->ia_ciclo[i].tipo = (rand() % 2) == 0 ? TIPO_ATAQUE : TIPO_DEFESA;

        // Regra: Custo 1, 2 ou 3
        int custo_sorteado = (rand() % 3) + 1; // 1 a 3

        // Regra: Máximo uma ação de nível 1
        if (custo_sorteado == 1) {
            if (contagem_nivel_1 > 0) {
                // Se já tem uma de nível 1, força ser 2 ou 3
                custo_sorteado = (rand() % 2) + 2; 
            } else {
                contagem_nivel_1++;
            }
        }
        mob->ia_ciclo[i].custo = custo_sorteado;
    }

    ValidarPeloMenosUmAtaque(mob);
}

void ConfigurarIA_BOSS(Entidade* mob) {
    // bixo tem tudo
    mob->ia_total_passos = 3; 
    mob->ia_passo_atual = 0;

    int contagem_nivel_1 = 0;

       for (int i = 0; i < mob->ia_total_passos; i++) {
        // 50% chance de Ataque ou Defesa
        mob->ia_ciclo[i].tipo = (rand() % 2) == 0 ? TIPO_ATAQUE : TIPO_DEFESA;

        // REGRA 2: Nível (Custo) entre 2 e 3
        // rand() % 2 gera 0 ou 1. 
        // Somando 2, o resultado final será sempre 2 ou 3.
        mob->ia_ciclo[i].custo = (rand() % 2) + 2; 
        }

        ValidarPeloMenosUmAtaque(mob);

    }
    

void FillRenderer(Renderer* renderer) {
  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

  renderer->display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  must_init(renderer->display, "display");

  renderer->display_buffer =
      al_create_bitmap(DISPLAY_BUFFER_WIDTH, DISPLAY_BUFFER_HEIGHT);
  must_init(renderer->display_buffer, "display buffer");

  renderer->font = al_create_builtin_font();
  must_init(renderer->font, "font");
  
   

  //mapa
  renderer->mapa_00 = al_load_bitmap("sprites/FUNDO_01_02.png");
  renderer->mapa_01 = al_load_bitmap("sprites/FUNDO_01_01.png");
  renderer->victory_screen = al_load_bitmap("sprites/TELA_VITORIA.png");
  renderer->defeat_screen = al_load_bitmap("sprites/TELA_DERROTA.png");

  //energia
  renderer->energia.energia_max = 3;
  renderer->energia.energia_atual = 3;
  renderer->energia.sprites[0] = al_load_bitmap("sprites/ENERGIA_0.png");
  renderer->energia.sprites[1] = al_load_bitmap("sprites/ENERGIA_1.png");
  renderer->energia.sprites[2] = al_load_bitmap("sprites/ENERGIA_2.png");
  renderer->energia.sprites[3] = al_load_bitmap("sprites/ENERGIA_3.png");
  renderer->energia.scale = ENERGIA_SCALE;
  renderer->energia.x = ENERGIA_X;
  renderer->energia.y = ENERGIA_Y;
  if(renderer->energia.sprites[3]) {
      renderer->energia.width = al_get_bitmap_width(renderer->energia.sprites[3]);
      renderer->energia.height = al_get_bitmap_height(renderer->energia.sprites[3]);
  }

  
  //player
  renderer->player.sprite = al_load_bitmap("sprites/PERSONAGEM_POSE_LUTA_01.png");
  must_init(renderer->player.sprite, "sprite player");
  renderer->player.x = PLAYER_BEGIN_X;
  renderer->player.y = PLAYER_BEGIN_Y;
  renderer->player.scale = PLAYER_SCALE;
  renderer->player.width = al_get_bitmap_width(renderer->player.sprite); // Pega tamanho real
  renderer->player.height = al_get_bitmap_height(renderer->player.sprite);
  renderer->player.vida_maxima = 100;
  renderer->player.vida_atual = 100;
  renderer->player.escudo = 0;
  
  //animacao player
  renderer->player.anim.current_frame = 0;
  renderer->player.anim.frame_timer = 0;
  renderer->player.anim.frame_width = 500;
  renderer->player.anim.frame_height = 500;
  renderer->player.anim.total_frames = 1;

 
    // --- CONFIGURAÇÃO DA ENERGIA ---
  renderer->energia.energia_max = 3;
  renderer->energia.energia_atual = 3; 

  // Carregando as sprites (Array de 4 posições conforme alteração anterior)
  renderer->energia.sprites[0] = al_load_bitmap("sprites/ENERGIA_0.png");
  renderer->energia.sprites[1] = al_load_bitmap("sprites/ENERGIA_1.png");
  renderer->energia.sprites[2] = al_load_bitmap("sprites/ENERGIA_2.png");
  renderer->energia.sprites[3] = al_load_bitmap("sprites/ENERGIA_3.png");
  must_init(renderer->energia.sprites[3], "sprite energia");



  //Pilhas de compra e descarte
renderer->descarte_img = al_load_bitmap("sprites/PILHA_DESCARTE.png");
must_init(renderer->descarte_img, "sprite deck de descarte");
renderer->compra = al_load_bitmap("sprites/PILHA_COMPRA.png");
must_init(renderer->compra, "sprite deck de compra");

  

  // Pega o tamanho original da imagem
  renderer->energia.width = al_get_bitmap_width(renderer->energia.sprites[3]);
  renderer->energia.height = al_get_bitmap_height(renderer->energia.sprites[3]);

  // AQUI VOCÊ CONTROLA O TAMANHO DA XÍCARA
  renderer->energia.scale = ENERGIA_SCALE; // Ex: 0.5 é metade do tamanho, 2.0 é o dobro.
  
  renderer->energia.x = ENERGIA_X;
  renderer->energia.y = ENERGIA_Y;

  renderer->seta_alvo = al_load_bitmap("sprites/SELECAO_MOB.png"); 
  must_init(renderer->seta_alvo, "sprite selecao mob");

   //icones de intenção
    renderer->icone_intencao_ataque = al_load_bitmap("sprites/ATAQUE.png");
  must_init(renderer->icone_intencao_ataque, "icone intencao ataque");

  renderer->icone_intencao_defesa = al_load_bitmap("sprites/DEFESA.png");
  must_init(renderer->icone_intencao_defesa, "icone intencao defesa");


  renderer->botao_selecionado = 0;
  renderer->estado_jogo = ESTADO_MENU;
   
  }


void RenderBackground(Renderer* renderer, int begin_x, int begin_y, int width, int height, float scale){
  
    al_draw_scaled_bitmap(renderer->mapa_01, 0, 0, width, height, begin_x, begin_y, width, height*scale, 0);
}
//muda o plano de fundo

void RenderDeck(Renderer* renderer, int x_left, int y_top) {
  if (renderer->deck_tamanho > 0 && renderer->compra != NULL) {
      
      
      float w_final = DECK_WIDTH * DECK_SCALE;
      float h_final = DECK_HEIGHT * DECK_SCALE;

      // 2. Desenha a imagem do deck
      al_draw_scaled_bitmap(
          renderer->compra,
          0, 0, 
          al_get_bitmap_width(renderer->compra), al_get_bitmap_height(renderer->compra),
          x_left, y_top, 
          w_final, h_final, 
          0
      );

      
      float centro_x = x_left;
      float centro_y = y_top + h_final - 25;

      al_draw_filled_circle(centro_x, centro_y, 25, al_map_rgb(20, 20, 20)); // Fundo Preto
      al_draw_circle(centro_x, centro_y, 25, al_map_rgb(255, 215, 0), 2);    // Borda Dourada

      
      char text[10];
      sprintf(text, "%d", renderer->deck_tamanho);
      
     
      DrawCenteredScaledText(renderer->font, al_map_rgb(255, 255, 255), 
                             centro_x + 1, centro_y - 10,
                             2.5, 2.5, text);
  }
}

void RenderDiscard(Renderer* renderer, int x, int y) {
    if (renderer->descarte_tamanho > 0 && renderer->descarte_img != NULL) {
        
        float w_final = DECK_WIDTH * DISCARD_SCALE;
        float h_final = DECK_HEIGHT * DISCARD_SCALE;

        al_draw_scaled_bitmap(
            renderer->descarte_img,
            0, 0, 
            al_get_bitmap_width(renderer->descarte_img), al_get_bitmap_height(renderer->descarte_img),
            x, y, 
            w_final, h_final, 
            0
        );

        // Calcula o centro
        float centro_x = x + 125;
        float centro_y = y + h_final - 25;

        // Badge estilizada
        al_draw_filled_circle(centro_x, centro_y, 25, al_map_rgb(20, 20, 20)); // Fundo Preto
        al_draw_circle(centro_x, centro_y, 25, al_map_rgb(160, 160, 160), 2);  // Borda Cinza/Prata

        // Número
        char text[10];
        sprintf(text, "%d", renderer->descarte_tamanho);
        DrawCenteredScaledText(renderer->font, al_map_rgb(255, 255, 255), 
                               centro_x + 1, centro_y - 10, 
                               2.5, 2.5, text);
    }
}

void RenderHealthBar(float x_begin, float x_largura, float y_down_left,
                     ALLEGRO_FONT* font, float vida_atual, float vida_maxima)              
                     {
  float mid_y = y_down_left - (HEALTH_BAR_HEIGHT * 0.78);
  
  float porcentagem = vida_atual/vida_maxima;
  al_draw_filled_rounded_rectangle(
      x_begin - HEALTH_BAR_BACKGROUND_EXTRA,
      y_down_left - HEALTH_BAR_BACKGROUND_EXTRA,
      x_begin + (x_largura + HEALTH_BAR_BACKGROUND_EXTRA),
      y_down_left - HEALTH_BAR_HEIGHT + HEALTH_BAR_BACKGROUND_EXTRA,
      HEALTH_BAR_RX, HEALTH_BAR_RY, al_map_rgb(124, 6, 0));
  char text[100] = "";
  sprintf(text, "%.0f/%.0f", vida_atual, vida_maxima);

  al_draw_filled_rounded_rectangle(
      x_begin - HEALTH_BAR_BACKGROUND_EXTRA,
      y_down_left - HEALTH_BAR_BACKGROUND_EXTRA,
      x_begin + ((x_largura * porcentagem) + HEALTH_BAR_BACKGROUND_EXTRA),
      y_down_left - HEALTH_BAR_HEIGHT + HEALTH_BAR_BACKGROUND_EXTRA,
      HEALTH_BAR_RX, HEALTH_BAR_RY, al_map_rgb(179, 0, 4));
  
  float x_scale = 1.0, y_scale = 1.0;
  DrawScaledText(font, al_map_rgb(0, 0, 0), (x_begin + x_largura/2.0) / x_scale,
                 mid_y / y_scale, x_scale, y_scale, ALLEGRO_ALIGN_CENTRE, text);
  //Mexe na barra de vida, cor, texto               
}

void RenderShieldBar(float x_begin, float x_end, float y_down_left,
                     ALLEGRO_FONT* font, int escudo) {
  float mid_y = y_down_left - (SHIELD_BAR_HEIGHT * 0.78);

  al_draw_filled_rounded_rectangle(
      x_begin - SHIELD_BAR_BACKGROUND_EXTRA,
      y_down_left - SHIELD_BAR_BACKGROUND_EXTRA,
      (x_end + SHIELD_BAR_BACKGROUND_EXTRA),
      y_down_left - SHIELD_BAR_HEIGHT + SHIELD_BAR_BACKGROUND_EXTRA,
      SHIELD_BAR_RX, SHIELD_BAR_RY, al_map_rgb(200, 254, 248));
  char text[100] = "";
  sprintf(text, "%d", escudo);
  float x_scale = 1.0, y_scale = 1.0;
  DrawScaledText(font, al_map_rgb(0, 0, 0), (x_begin + x_end) / 2.0 / x_scale,
                 mid_y / y_scale, x_scale, y_scale, ALLEGRO_ALIGN_CENTRE, text);
  //Mexe na barra de escudo, cor, texto               
}

// Função genérica para desenhar qualquer entidade (Player ou Inimigo)
void RenderEntity(const Renderer* renderer, const Entidade *ent) {
    
  if (ent == NULL || ent->sprite == NULL || ent->vida_atual <= 0) {
        return;
    }

    // Desenha o sprite
    al_draw_scaled_bitmap(
        ent->sprite, 
        ent->anim.current_frame * ent->anim.frame_width, 
        0, 
        ent->width, ent->height, 
        ent->x, ent->y, 
        ent->width * ent->scale, ent->height * ent->scale, 
        0
    );

    // Calcula posições das barras
    float largura_final = ent->width * ent->scale;
    float altura_final = ent->height * ent->scale;
    float x_end_vida = ent->x + largura_final;
    float x_end_escudo = ent->x + largura_final;

    float health_bar_y = ent->y + altura_final + 20;
       
    RenderHealthBar(ent->x, largura_final, health_bar_y, renderer->font, ent->vida_atual, ent->vida_maxima);
    
    if (ent->escudo > 0) {
        float shield_bar_y = health_bar_y + 20;
        RenderShieldBar(ent->x, x_end_escudo, shield_bar_y, renderer->font, ent->escudo);
    }
}

void RenderCard(const Renderer* renderer, int x, int y, int id_carta, int is_selected) {
  Carta c = BIBLIOTECA[id_carta]; 

    int draw_y = is_selected ? y - 100 : y; // Sobe se selecionada
    
    // Desenha o fundo da carta
    ALLEGRO_COLOR cor_borda = is_selected ? al_map_rgb(255, 215, 0) : al_map_rgb(0,0,0);
    
     al_draw_filled_rounded_rectangle(x, draw_y, x+CARD_WIDTH, draw_y+CARD_HEIGHT, 5, 5, al_map_rgb(255,255,255));
    

    if (c.arte != NULL) {
        al_draw_scaled_bitmap(c.arte, 
            0, 0, al_get_bitmap_width(c.arte), al_get_bitmap_height(c.arte),
            x, draw_y, CARD_WIDTH, CARD_HEIGHT, 0 // Estica a arte para o tamanho da carta
        );
    }

    al_draw_rounded_rectangle(x - 2, draw_y - 2, x+CARD_WIDTH + 2, draw_y+CARD_HEIGHT + 2, 5, 5, cor_borda, is_selected ? 5 : 5);

   
    // Mostra Dano/Escudo
    char texto_custo[10];
    if(c.valor_ataque > 0){
    sprintf(texto_custo, "%d Dano", c.valor_ataque);
    } else if(c.valor_escudo > 0){
        sprintf(texto_custo, "%d Escudo", c.valor_escudo);
    } else sprintf(texto_custo, "Especial", NULL);
    DrawScaledText(renderer->font, al_map_rgb(27, 22, 22), x + 20, draw_y + 150, 1.0, 1.0, ALLEGRO_ALIGN_LEFT, texto_custo);
}

int CalcularEfeitoPorCusto(int custo) {
    if (custo == 0) return (rand() % 5) + 1;        // 1 a 5
    if (custo == 1) return (rand() % 6) + 5;        // 5 a 10
    if (custo == 2) return (rand() % 6) + 10;       // 10 a 15
    if (custo == 3) return (rand() % 16) + 15;      // 15 a 30
    return 0;
}

ALLEGRO_BITMAP* CarregarArtePorCusto(TipoCarta tipo, int custo) {
    char caminho[100];

    if (tipo == TIPO_ATAQUE) {
        if (custo == 0) {
            // Arte para ataques rápidos/fracos
            sprintf(caminho, "sprites/CARTA_ATAQUE_00.png"); 
        } 
        else if (custo == 1) {
            // Arte para ataques comuns
            sprintf(caminho, "sprites/CARTA_ATAQUE_01.png"); 
        } 
        else if (custo == 2) {
            // Arte para ataques fortes
            sprintf(caminho, "sprites/CARTA_ATAQUE_02.png"); 
        }
        else{
            // Arte para ataques muito fortes
            sprintf(caminho, "sprites/CARTA_ATAQUE_03.png"); 
    } 
    }
    else if (tipo == TIPO_DEFESA) {
       if (custo == 0) {
            // Arte para ataques rápidos/fracos
            sprintf(caminho, "sprites/CARTA_DEFESA_03.png"); 
        } 
        else if (custo == 1) {
            // Arte para ataques comuns
            sprintf(caminho, "sprites/CARTA_DEFESA_01.png"); // Mude para _02 se tiver
        } 
        else if (custo == 2) {
            // Arte para ataques fortes
            sprintf(caminho, "sprites/CARTA_DEFESA_02.png"); 
        }
        else{
            // Arte para ataques muito fortes
            sprintf(caminho, "sprites/CARTA_DEFESA_03.png"); 
    } 
      }
    else {
        sprintf(caminho, "sprites/CARTA_ESPECIAL_00.png");
    }

    ALLEGRO_BITMAP* bmp = al_load_bitmap(caminho);
    if (!bmp) {
        printf("AVISO: Nao foi possivel carregar arte: %s. Usando padrao.\n", caminho);
        // Fallback para não crashar o jogo
        return al_load_bitmap("sprites/CARTA_ATAQUE_01.png"); 
    }
    return bmp;
}

void CriarCartaProcedural(int id, TipoCarta tipo, int custo) {
    BIBLIOTECA[id].id = id;
    BIBLIOTECA[id].tipo = tipo;
    BIBLIOTECA[id].custo = custo;
    
    int valor = CalcularEfeitoPorCusto(custo);

    
    if (tipo == TIPO_ATAQUE) {
        
        if (custo == 0) sprintf(BIBLIOTECA[id].nome, "Ataque de Papelada");
        else if (custo == 1) sprintf(BIBLIOTECA[id].nome, "Pe na Mesa");
        else if (custo == 2) sprintf(BIBLIOTECA[id].nome, "Cafe Quente");
        else sprintf(BIBLIOTECA[id].nome, "Ataque de Monitor");

        sprintf(BIBLIOTECA[id].descricao, "%d Dano", valor);
        BIBLIOTECA[id].valor_ataque = valor;
        BIBLIOTECA[id].valor_escudo = 0;
    } 
    else if (tipo == TIPO_DEFESA) {
       if (custo == 0) sprintf(BIBLIOTECA[id].nome, "Arquivo Blindado");
        else if (custo == 1) sprintf(BIBLIOTECA[id].nome, "Morcegada Estrategica");
        else if (custo == 2) sprintf(BIBLIOTECA[id].nome, "Barreira de Mesa");
        else sprintf(BIBLIOTECA[id].nome, "Codigo Trabalhista");

        sprintf(BIBLIOTECA[id].descricao, "%d Escudo", valor);
        BIBLIOTECA[id].valor_ataque = 0;
        BIBLIOTECA[id].valor_escudo = valor;
    }

    // --- AQUI APLICAMOS A ARTE ESPECÍFICA ---
    BIBLIOTECA[id].arte = CarregarArtePorCusto(tipo, custo);
}

void InicializarBibliotecaCartas() {
    
    memset(BIBLIOTECA, 0, sizeof(BIBLIOTECA));

    int id_atual = 0;

   
    
    // Obrigatórias
    CriarCartaProcedural(id_atual++, TIPO_ATAQUE, 0);
    CriarCartaProcedural(id_atual++, TIPO_ATAQUE, 1);
    CriarCartaProcedural(id_atual++, TIPO_ATAQUE, 1);
    CriarCartaProcedural(id_atual++, TIPO_ATAQUE, 1);
    CriarCartaProcedural(id_atual++, TIPO_ATAQUE, 2);
    CriarCartaProcedural(id_atual++, TIPO_ATAQUE, 3);

  
    for(int i = 0; i < 4; i++) {
        int custo_sorteado = rand() % 4; // 0 a 3
        CriarCartaProcedural(id_atual++, TIPO_ATAQUE, custo_sorteado);
    }

   

    // Obrigatórias
    CriarCartaProcedural(id_atual++, TIPO_DEFESA, 0);
    CriarCartaProcedural(id_atual++, TIPO_DEFESA, 1);
    CriarCartaProcedural(id_atual++, TIPO_DEFESA, 1);
    CriarCartaProcedural(id_atual++, TIPO_DEFESA, 1);
    CriarCartaProcedural(id_atual++, TIPO_DEFESA, 2);
    CriarCartaProcedural(id_atual++, TIPO_DEFESA, 3);

    
    for(int i = 0; i < 2; i++) {
        int custo_sorteado = rand() % 3; 
        CriarCartaProcedural(id_atual++, TIPO_DEFESA, custo_sorteado);
    }

   
    
    BIBLIOTECA[id_atual].id = id_atual;
    sprintf(BIBLIOTECA[id_atual].nome, "Troca de Turno");
    sprintf(BIBLIOTECA[id_atual].descricao, "Troca a Mao");
    BIBLIOTECA[id_atual].tipo = TIPO_ESPECIAL;
    BIBLIOTECA[id_atual].custo = 0; 
    BIBLIOTECA[id_atual].valor_ataque = 0;
    BIBLIOTECA[id_atual].valor_escudo = 0; 
    BIBLIOTECA[id_atual].arte = CarregarArtePorCusto(TIPO_ESPECIAL, 0); 
    id_atual++;

    BIBLIOTECA[id_atual].id = id_atual;
    sprintf(BIBLIOTECA[id_atual].nome, "Reuniao de Emergencia");
    sprintf(BIBLIOTECA[id_atual].descricao, "Troca a Mao");
    BIBLIOTECA[id_atual].tipo = TIPO_ESPECIAL;
    BIBLIOTECA[id_atual].custo = 0;
    BIBLIOTECA[id_atual].valor_ataque = 0;
    BIBLIOTECA[id_atual].valor_escudo = 0;
    BIBLIOTECA[id_atual].arte = CarregarArtePorCusto(TIPO_ESPECIAL, 0);
    id_atual++;
}

void ReciclarDescarte(Renderer* renderer) {
    printf("Baralho vazio! Embaralhando pilha de descarte...\n");

    
    for (int i = 0; i < renderer->descarte_tamanho; i++) {
        renderer->deck[i] = renderer->descarte[i];
    }
    
    
    renderer->deck_tamanho = renderer->descarte_tamanho;
    
    // Zera o descarte
    renderer->descarte_tamanho = 0;

    // Embaralha o novo deck
    ShuffleArray(renderer->deck, renderer->deck_tamanho);
}

void IniciarDeckCombate(Renderer* renderer) {
    renderer->deck_tamanho = 0;
    renderer->mao_tamanho = 0;
    renderer->indice_selecionado = 0;

    renderer->descarte_tamanho = 0;

    
    for(int i = 0; i < TOTAL_CARTAS_JOGO; i++) {
        renderer->deck[renderer->deck_tamanho++] = i; 
    }
  
    // Embaralha
    ShuffleArray(renderer->deck, renderer->deck_tamanho);
    
    // Compra 5 cartas iniciais
    for(int i=0; i<5; i++) ComprarCarta(renderer);
}

void ComprarCarta(Renderer* renderer) {
    // Se o deck estiver vazio, tenta reciclar o descarte
    if (renderer->deck_tamanho == 0) {
        if (renderer->descarte_tamanho > 0) {
            ReciclarDescarte(renderer);
        } else {
            printf("Nao ha mais cartas nem no deck nem no descarte!\n");
            return; // Não tem o que fazer
        }
    }

    // Lógica padrão de compra (agora segura)
    if (renderer->deck_tamanho > 0 && renderer->mao_tamanho < 5) {
        int id_carta = renderer->deck[renderer->deck_tamanho - 1];
        renderer->deck_tamanho--;

        renderer->mao[renderer->mao_tamanho] = id_carta;
        renderer->mao_tamanho++;
    }
}

void RenderPlayerHand(Renderer* renderer) {
  int espacamento = 100; 
    int inicio_x = HAND_BEGIN_X;

    for (int i = 0; i < renderer->mao_tamanho; i++) {
        int id_carta = renderer->mao[i];
        int selecionada = (i == renderer->indice_selecionado);

        
        RenderCard(renderer, inicio_x + (i * espacamento), HAND_BEGIN_Y, id_carta, selecionada);
    }
}

void ExecutarTurnoInimigos(Renderer* renderer) {
    printf("\n--- TURNO DOS INIMIGOS ---\n");

    Entidade* mobs[5] = {&renderer->mob_fraco_1, &renderer->mob_medio, &renderer->mob_forte, &renderer->boss };

    for(int i = 0; i < 4; i++) {
        Entidade* mob = mobs[i];

        
        if (mob->sprite != NULL && mob->vida_atual > 0) {
            
           
            mob->escudo = 0;

            
            int passo = mob->ia_passo_atual;
            AcaoMob acao = mob->ia_ciclo[passo];

            
            int valor = CalcularEfeitoPorCusto(acao.custo);

         
            if (acao.tipo == TIPO_ATAQUE) {
                printf("Inimigo %d usou ATAQUE (Nvl %d) -> %d Dano!\n", i, acao.custo, valor);

                
                if (renderer->player.escudo > 0) {
                    int sobra = valor - renderer->player.escudo;
                    renderer->player.escudo -= valor;
                    
                    if (renderer->player.escudo < 0) {
                        renderer->player.escudo = 0;
                        valor = sobra; 
                    } else {
                        valor = 0; 
                    }
                }
                if (valor > 0) {
                    renderer->player.vida_atual -= valor;
                    if (renderer->player.vida_atual < 0) renderer->player.vida_atual = 0;
                }

            } 

            else if (acao.tipo == TIPO_DEFESA) {
                printf("Inimigo %d usou DEFESA (Nvl %d) -> %d Escudo!\n", i, acao.custo, valor);
                mob->escudo += valor;
            }

            // 4. Avança o ciclo para o próximo turno
            mob->ia_passo_atual++;
            // Se chegou no fim, volta para o começo (cíclico)
            if (mob->ia_passo_atual >= mob->ia_total_passos) {
                mob->ia_passo_atual = 0;
            }
        }
    }
    printf("\n");
}

void RenderVictory(Renderer* renderer) {
    
    al_clear_to_color(al_map_rgb(0, 0, 0)); 

   
    if (renderer->victory_screen != NULL) {
        al_draw_scaled_bitmap(
            renderer->victory_screen,
            0, 0,                                
            al_get_bitmap_width(renderer->victory_screen), 
            al_get_bitmap_height(renderer->victory_screen),
            0, 0,                                
            DISPLAY_BUFFER_WIDTH,                
            DISPLAY_BUFFER_HEIGHT,               
            0                                    
        );
    }
}

void EndingScreen(Renderer* renderer){
    printf("---- Você venceu o CLT ----");
    printf("---- Curta seus próximos meses de auxilio desemprego! ----");



}

void RenderDefeat(Renderer* renderer) {
   
    al_clear_to_color(al_map_rgb(40, 0, 0)); 

   
    if (renderer->defeat_screen != NULL) {
        al_draw_scaled_bitmap(
            renderer->defeat_screen,
            0, 0,
            al_get_bitmap_width(renderer->defeat_screen), 
            al_get_bitmap_height(renderer->defeat_screen),
            0, 0,
            DISPLAY_BUFFER_WIDTH,
            DISPLAY_BUFFER_HEIGHT,
            0
        );
    }
}

void RenderMenu(Renderer* renderer) {
   
    al_clear_to_color(al_map_rgb(0,0,0));
   
    al_draw_scaled_bitmap(renderer->mapa_00,
                        0, -20,
                        BACKGROUND_WIDTH, BACKGROUND_HEIGHT,
                        0, 0,
                        BACKGROUND_WIDTH, BACKGROUND_HEIGHT * BACKGROUND_SCALE,
                        0
                    );

   
    int selecionado_jogar = (renderer->botao_selecionado == 0);
    float y_jogar = selecionado_jogar ? BACKGROUND_HEIGHT/2 - 15 : BACKGROUND_HEIGHT/2;
  
    ALLEGRO_COLOR cor_jogar_borda = selecionado_jogar ? al_map_rgb(253, 186, 53) : al_map_rgb(80, 16, 29);
    
    
    
    al_draw_filled_rounded_rectangle(BACKGROUND_WIDTH/2 - 192, y_jogar - 12,
                            BACKGROUND_WIDTH/2 - 8, y_jogar + 32,
                            10, 10,
                        cor_jogar_borda);

    ALLEGRO_COLOR cor_jogar_borda_frente =  al_map_rgb(80, 16, 29);

    al_draw_filled_rounded_rectangle(BACKGROUND_WIDTH/2 - 190, y_jogar - 10,
                            BACKGROUND_WIDTH/2 - 10, y_jogar + 30,
                            10, 10,
                        cor_jogar_borda_frente);


    
    ALLEGRO_COLOR cor_jogar = selecionado_jogar ? al_map_rgb(255, 215, 0) : al_map_rgb(150, 150, 150);
    
    
    DrawCenteredScaledText(renderer->font, cor_jogar, 
                           BACKGROUND_WIDTH/2 - 100, y_jogar, 
                           3.0, 3.0, "JOGAR");

    
                           

    // --- BOTÃO SAIR ---
    int selecionado_sair = (renderer->botao_selecionado == 1);
    
    ALLEGRO_COLOR cor_sair = selecionado_sair ? al_map_rgb(255, 215, 0) : al_map_rgb(150, 150, 150);
    
    float y_sair = selecionado_sair ? BACKGROUND_HEIGHT/2 - 15 : BACKGROUND_HEIGHT/2;

    ALLEGRO_COLOR cor_sair_borda_tras = selecionado_jogar ?  al_map_rgb(80, 16, 29) : al_map_rgb(253, 186, 53);

    al_draw_filled_rounded_rectangle(BACKGROUND_WIDTH/2 + 8, y_sair - 12,
                            BACKGROUND_WIDTH/2 + 192, y_sair + 32,
                            10, 10,
                        cor_sair_borda_tras);


    
    ALLEGRO_COLOR cor_sair_borda_frente =  al_map_rgb(80, 16, 29);

    al_draw_filled_rounded_rectangle(BACKGROUND_WIDTH/2 + 10, y_sair - 10,
                            BACKGROUND_WIDTH/2 + 190, y_sair + 30,
                            10, 10,
                        cor_sair_borda_frente);


    DrawCenteredScaledText(renderer->font, cor_sair, 
                           BACKGROUND_WIDTH/2 + 100, y_sair, 
                           3.0, 3.0, "SAIR"); // Escala fixa em 3.0


    
}

void IniciarTurnoJogador(Renderer* renderer) {
    printf("--- SEU TURNO ---\n");
    printf("Sua vida e %d\n", renderer->player.vida_atual);

    
    renderer->energia.energia_atual = renderer->energia.energia_max;

    
    renderer->player.escudo = 0; 
    
    for(int i = 0; i < renderer->mao_tamanho; i++) {
        renderer->descarte[renderer->descarte_tamanho++] = renderer->mao[i];
    }

    // 3. Descarta a mão atual inteira
    renderer->mao_tamanho = 0;
    renderer->indice_selecionado = 0;
    
    //Compra 5 cartas novas
    // Se o deck acabar, precisariamos de logica para reembaralhar o descarte, 
    // mas por enquanto vamos assumir que o deck é grande ou infinito na logica atual.
    for(int i = 0; i < 5; i++) {
        // Se deck acabar, reinicia ele (lógica simples de emergência)
        if (renderer->deck_tamanho == 0) {
            IniciarDeckCombate(renderer); 
        }
        ComprarCarta(renderer);
    }
    


    // 5. Garante estado de escolha de carta
    renderer->estado_jogo = ESTADO_ESCOLHER_CARTA;
}

void RenderEnergy(Renderer* renderer, const Energia *energia) {
  int indice = renderer->energia.energia_atual;
  if (indice < 0) indice = 0;
  if (indice > 3) indice = 3;

  if (renderer->energia.sprites[indice] != NULL) {
      al_draw_scaled_bitmap(
          renderer->energia.sprites[indice],
          0, 0,
          renderer->energia.width, renderer->energia.height, 
          renderer->energia.x, renderer->energia.y,          
          renderer->energia.width * renderer->energia.scale, 
          renderer->energia.height * renderer->energia.scale,
          0
      );
      char texto_energia[20]; 
      
      sprintf(texto_energia, "%d/%d", renderer->energia.energia_atual, (int)renderer->energia.energia_max);
      
      DrawCenteredScaledText(
          renderer->font,            
          al_map_rgb(0, 0, 0), 
          (renderer->energia.x + renderer->energia.width)/2 + 10,                  
          renderer->energia.y - 5,                     
          1.5, 1.5,                  
          texto_energia              
      );
  }
}

void IniciarNovaRodada(Renderer* renderer) {
    if (renderer->mob_fraco_1.sprite) { al_destroy_bitmap(renderer->mob_fraco_1.sprite); renderer->mob_fraco_1.sprite = NULL; }
    if (renderer->mob_forte.sprite) { al_destroy_bitmap(renderer->mob_forte.sprite); renderer->mob_forte.sprite = NULL; }
    if (renderer->mob_medio.sprite) { al_destroy_bitmap(renderer->mob_medio.sprite); renderer->mob_medio.sprite = NULL; }
    
    if (renderer->boss.sprite) { al_destroy_bitmap(renderer->boss.sprite); renderer->boss.sprite = NULL; }
    renderer->boss.vida_atual = 0; 
   

    renderer->round_atual++;
    printf("--- INICIANDO ROUND %d ---\n", renderer->round_atual);

    int spawn_forte = (rand() % 100) < 5; 

    // MOB FORTE
    if (spawn_forte) {
        renderer->mob_forte.sprite = al_load_bitmap("sprites/STRONG_MOB_STAY.png");
        must_init(renderer->mob_forte.sprite, "sprite mob forte");

        renderer->mob_forte.posicao = rand() % 2;
        
        if (renderer->mob_forte.posicao == 0) { renderer->mob_forte.x = MOB1_BEGIN_X; renderer->mob_forte.y = MOB1_BEGIN_Y; } 
        else { renderer->mob_forte.x = MOB2_BEGIN_X; renderer->mob_forte.y = MOB2_BEGIN_Y; }

        renderer->mob_forte.width = al_get_bitmap_width(renderer->mob_forte.sprite);
        renderer->mob_forte.height = al_get_bitmap_height(renderer->mob_forte.sprite);
        renderer->mob_forte.scale = MOB2_SCALE;
        
        // 
        int vida_forte = (rand() % 61) + 40; 
        renderer->mob_forte.vida_maxima = vida_forte;
        renderer->mob_forte.vida_atual = vida_forte;
        renderer->mob_forte.escudo = 0;

        ConfigurarIA_Forte(&renderer->mob_forte);
        
        // Zera animação(não fiz)
        renderer->mob_forte.anim.current_frame = 0; renderer->mob_forte.anim.frame_timer = 0;
        renderer->mob_forte.anim.frame_width = 500; renderer->mob_forte.anim.frame_height = 500; renderer->mob_forte.anim.total_frames = 1;
    } else {
        renderer->mob_forte.sprite = NULL;
        renderer->mob_forte.vida_atual = 0;
    }

    // MOB FRACO 1
    renderer->mob_fraco_1.sprite = al_load_bitmap("sprites/WEAK_MOB_STAY_01.png");
    must_init(renderer->mob_fraco_1.sprite, "sprite mob fraco 1");

    if (spawn_forte) renderer->mob_fraco_1.posicao = (renderer->mob_forte.posicao == 0) ? 1 : 0;
    else renderer->mob_fraco_1.posicao = 0;

    if (renderer->mob_fraco_1.posicao == 0) { renderer->mob_fraco_1.x = MOB1_BEGIN_X; renderer->mob_fraco_1.y = MOB1_BEGIN_Y; } 
    else { renderer->mob_fraco_1.x = MOB2_BEGIN_X; renderer->mob_fraco_1.y = MOB2_BEGIN_Y; }

    renderer->mob_fraco_1.width = al_get_bitmap_width(renderer->mob_fraco_1.sprite);
    renderer->mob_fraco_1.height = al_get_bitmap_height(renderer->mob_fraco_1.sprite);
    renderer->mob_fraco_1.scale = MOB1_SCALE;

  
    int vida_fraco1 = (rand() % 21) + 10;
    renderer->mob_fraco_1.vida_maxima = vida_fraco1;
    renderer->mob_fraco_1.vida_atual = vida_fraco1;
    renderer->mob_fraco_1.escudo = 0;

    ConfigurarIA_Fraco(&renderer->mob_fraco_1);
    
    renderer->mob_fraco_1.anim.current_frame = 0; renderer->mob_fraco_1.anim.frame_timer = 0;
    renderer->mob_fraco_1.anim.frame_width = 500; renderer->mob_fraco_1.anim.frame_height = 500; renderer->mob_fraco_1.anim.total_frames = 1;

    //mob medio
    if (!spawn_forte) {
        renderer->mob_medio.sprite = al_load_bitmap("sprites/WEAK_MOB_STAY_01.png");
        must_init(renderer->mob_medio.sprite, "sprite mob medio");
        renderer->mob_medio.posicao = 1;
        renderer->mob_medio.x = MOB2_BEGIN_X; renderer->mob_medio.y = MOB2_BEGIN_Y;
        renderer->mob_medio.width = al_get_bitmap_width(renderer->mob_medio.sprite);
        renderer->mob_medio.height = al_get_bitmap_height(renderer->mob_medio.sprite);
        renderer->mob_medio.scale = MOB1_SCALE;

        int vida_mob_medio = (rand() % 9) + 31;
        renderer->mob_medio.vida_maxima = vida_mob_medio;
        renderer->mob_medio.vida_atual = vida_mob_medio;
        renderer->mob_medio.escudo = 0;

        ConfigurarIA_Medio(&renderer->mob_medio);

        renderer->mob_medio.anim.current_frame = 0; renderer->mob_medio.anim.frame_timer = 0;
        renderer->mob_medio.anim.frame_width = 500; renderer->mob_medio.anim.frame_height = 500; renderer->mob_medio.anim.total_frames = 1;
    } else {
        renderer->mob_medio.sprite = NULL;
        renderer->mob_medio.vida_atual = 0;
    }

    
}

void IniciarRodadaBoss(Renderer* renderer) {
    // 1. LIMPEZA: Destrói sprites antigos se existirem
    if (renderer->mob_fraco_1.sprite) { al_destroy_bitmap(renderer->mob_fraco_1.sprite); renderer->mob_fraco_1.sprite = NULL; }
    if (renderer->mob_medio.sprite) { al_destroy_bitmap(renderer->mob_medio.sprite); renderer->mob_medio.sprite = NULL; }
    if (renderer->mob_forte.sprite) { al_destroy_bitmap(renderer->mob_forte.sprite); renderer->mob_forte.sprite = NULL; }

    renderer->mob_fraco_1.vida_atual = 0;
    renderer->mob_medio.vida_atual = 0;
    renderer->mob_forte.vida_atual = 0;

    renderer->round_atual++;
    printf("--- INICIANDO ROUND %d ---\n", renderer->round_atual);
    printf("\n\n\n");
    printf("--- PREPARE-SE PARA SEU MAIOR PESADELO ---\n");

        renderer->boss.sprite = al_load_bitmap("sprites/BOSS.png");
        must_init(renderer->boss.sprite, "sprite boss");

        renderer->boss.x = BOSS_BEGIN_X; 
        renderer->boss.y = BOSS_BEGIN_Y;

        renderer->boss.width = al_get_bitmap_width(renderer->boss.sprite);
        renderer->boss.height = al_get_bitmap_height(renderer->boss.sprite);
        renderer->boss.scale = BOSS_SCALE;
        
        // Vida base + Bonus do Round
        int vida_boss = (rand() % 101) + 80;
        renderer->boss.vida_maxima = vida_boss;
        renderer->boss.vida_atual = vida_boss;
        renderer->boss.escudo = 20;

        ConfigurarIA_BOSS(&renderer->boss);
        
        // Zera animação
        renderer->boss.anim.current_frame = 0; renderer->boss.anim.frame_timer = 0;
        renderer->boss.anim.frame_width = 500; renderer->boss.anim.frame_height = 500; renderer->boss.anim.total_frames = 1;
    
       renderer->alvo_selecionado = 3;
}

void RenderTargetSelection(Renderer* renderer) {
    if (renderer->estado_jogo != ESTADO_ESCOLHER_ALVO) return;

    Entidade* alvo = NULL;

    // Mapeia o índice 0, 1, 2 para os monstros reais
    if (renderer->alvo_selecionado == 0) alvo = &renderer->mob_fraco_1;
    else if (renderer->alvo_selecionado == 1) alvo = &renderer->mob_medio;
    else if (renderer->alvo_selecionado == 2) alvo = &renderer->mob_forte;
    else if (renderer->alvo_selecionado == 3) alvo = &renderer->boss;

    // Se o alvo existe e está vivo, desenha a seta sobre ele
    if (alvo != NULL && alvo->vida_atual > 0) {
        if(alvo == &renderer->boss){
             al_draw_scaled_bitmap(
            renderer->seta_alvo,
            0, 0,
            al_get_bitmap_width(renderer->seta_alvo),
            al_get_bitmap_height(renderer->seta_alvo),
            alvo->x + (alvo->width * alvo->scale / 2) - 25, // Centraliza X (aprox)
            alvo->y - 100, // Posição Y um pouco acima da cabeça
            al_get_bitmap_width(renderer->seta_alvo), al_get_bitmap_height(renderer->seta_alvo), // Tamanho que você quer desenhar na tela
            0
            );
        }else{
        al_draw_scaled_bitmap(
            renderer->seta_alvo,
            0, 0,
            al_get_bitmap_width(renderer->seta_alvo),
            al_get_bitmap_height(renderer->seta_alvo),
            alvo->x + (alvo->width * alvo->scale / 2) - 25, // Centraliza X (aprox)
            alvo->y - 200, // Posição Y um pouco acima da cabeça
            al_get_bitmap_width(renderer->seta_alvo), al_get_bitmap_height(renderer->seta_alvo), // Tamanho que você quer desenhar na tela
            0
        );
        }
    }
}

void RenderIntention(const Renderer* renderer, const Entidade *mob) {
    if (mob == NULL || mob->sprite == NULL || mob->vida_atual <= 0) return;

    AcaoMob proxima_acao = mob->ia_ciclo[mob->ia_passo_atual];
    ALLEGRO_BITMAP* icone_para_desenhar = NULL;

    if (proxima_acao.tipo == TIPO_ATAQUE) {
        icone_para_desenhar = renderer->icone_intencao_ataque;
    } else if (proxima_acao.tipo == TIPO_DEFESA) {
        icone_para_desenhar = renderer->icone_intencao_defesa;
    }

    if (icone_para_desenhar != NULL) {
        int tamanho_alvo = 40; 
        float mob_centro_x = mob->x + (mob->width * mob->scale / 2.0f);
        float draw_x = mob_centro_x - (tamanho_alvo / 2.0f);
        float draw_y;

        // CORREÇÃO: Verifica se "este mob" é o Boss para ajustar a altura
        if (mob == &renderer->boss) {
             // O Boss é grande, então o ícone fica numa altura diferente
             draw_y = mob->y; 
        } else {
             // Mobs normais: ícone flutua acima da cabeça
             draw_y = mob->y - 80;
        }

        al_draw_scaled_bitmap(
            icone_para_desenhar,
            0, 0, 
            al_get_bitmap_width(icone_para_desenhar), al_get_bitmap_height(icone_para_desenhar), 
            draw_x, draw_y, 
            tamanho_alvo, tamanho_alvo, 
            0 
        );
    }
}

void Render(Renderer* renderer) {
  al_set_target_bitmap(renderer->display_buffer);

  // --- SELETOR DE ESTADOS DE DESENHO ---
  if (renderer->estado_jogo == ESTADO_MENU) {
      RenderMenu(renderer);
  } 
  else if (renderer->estado_jogo == ESTADO_VITORIA) { 
      RenderVictory(renderer);
  }
  else if (renderer->estado_jogo == ESTADO_DERROTA) {
      RenderDefeat(renderer);
  }else {
      
      RenderBackground(renderer, BACKGROUND_BEGIN_X, BACKGROUND_BEGIN_Y, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, BACKGROUND_SCALE);
      RenderDeck(renderer, DRAW_DECK_X, DRAW_DECK_Y);
      RenderDiscard(renderer, DRAW_DISCARD_X, DRAW_DISCARD_Y);

      RenderEntity(renderer, &renderer->player);
      RenderEntity(renderer, &renderer->mob_fraco_1);
      RenderEntity(renderer, &renderer->mob_medio);
      RenderEntity(renderer, &renderer->mob_forte);
      RenderEntity(renderer, &renderer->boss);

      RenderIntention(renderer, &renderer->mob_fraco_1);
      RenderIntention(renderer, &renderer->mob_medio);
      RenderIntention(renderer, &renderer->mob_forte);
      RenderIntention(renderer, &renderer->boss);

      RenderEnergy(renderer, &renderer->energia);
      RenderPlayerHand(renderer);
      
      RenderTargetSelection(renderer); 
  }
 

  al_set_target_backbuffer(renderer->display);
  al_draw_scaled_bitmap(renderer->display_buffer, 0, 0, DISPLAY_BUFFER_WIDTH,
                        DISPLAY_BUFFER_HEIGHT, 0, 0, DISPLAY_WIDTH,
                        DISPLAY_HEIGHT, 0);
  al_flip_display();
}

void ClearRenderer(Renderer* renderer) {
  al_destroy_display(renderer->display);
  al_destroy_bitmap(renderer->display_buffer);
  al_destroy_font(renderer->font);
  
  // Destrói os recursos das entidades
  if (renderer->player.sprite) al_destroy_bitmap(renderer->player.sprite);
  if (renderer->mob_fraco_1.sprite) al_destroy_bitmap(renderer->mob_fraco_1.sprite);
  if (renderer->mob_medio.sprite) al_destroy_bitmap(renderer->mob_medio.sprite);
  if (renderer->mob_forte.sprite) al_destroy_bitmap(renderer->mob_forte.sprite);
  for(int i = 0; i < 4; i++) {
      if (renderer->energia.sprites[i]) {
          al_destroy_bitmap(renderer->energia.sprites[i]);
      }
  }
  if(renderer->descarte_img) al_destroy_bitmap(renderer->descarte_img);
  if(renderer->compra) al_destroy_bitmap(renderer->compra);
  
  al_destroy_bitmap(renderer->mapa_01);
  al_destroy_bitmap(renderer->mapa_00);
  if (renderer->victory_screen) al_destroy_bitmap(renderer->victory_screen);
  if (renderer->defeat_screen) al_destroy_bitmap(renderer->defeat_screen);
  
  if (renderer->seta_alvo) al_destroy_bitmap(renderer->seta_alvo);


  if (renderer->icone_intencao_ataque) al_destroy_bitmap(renderer->icone_intencao_ataque);
  if (renderer->icone_intencao_defesa) al_destroy_bitmap(renderer->icone_intencao_defesa);


}
