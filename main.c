#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"
#include "renderer.h"
#include "utils.h"



int main() {
  must_init(al_init(), "allegro");
  must_init(al_init_image_addon(), "allegro");
  must_init(al_init_primitives_addon(), "primitives");
  must_init(al_install_keyboard(), "keyboard");

  srand(time(NULL));

  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
  must_init(timer, "timer");
  //TEMPORIZADOR DE FRAMES POR SEGUNDO, 60 NO CASO

  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");

  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_timer_event_source(timer));
  unsigned char keyboard_keys[ALLEGRO_KEY_MAX]; // tabela ascii do teclado
  ClearKeyboardKeys(keyboard_keys);
  ALLEGRO_EVENT event;

  Renderer renderer = {0}; 
  FillRenderer(&renderer);
  
  InicializarBibliotecaCartas();
  IniciarDeckCombate(&renderer);
  


  al_register_event_source(queue,
                           al_get_display_event_source(renderer.display));

  al_start_timer(timer);
  while (1) {
    al_wait_for_event(queue, &event);
    

    if (renderer.estado_jogo != ESTADO_MENU) {
        int mobs_vivos = 0;
        if (renderer.mob_fraco_1.vida_atual > 0) mobs_vivos++;
        if (renderer.mob_medio.vida_atual > 0) mobs_vivos++;
        if (renderer.mob_forte.vida_atual > 0) mobs_vivos++;
        if (renderer.boss.vida_atual > 0) mobs_vivos++;

        if (mobs_vivos == 0) {
            printf("Todos os inimigos morreram! Preparando proxima rodada...\n");
        
            int proximo_round = renderer.round_atual + 1;
            
            if (proximo_round == 11) { 
                IniciarTurnoJogador(&renderer);
                IniciarRodadaBoss(&renderer); 
            } 
            else if (proximo_round == 12)
            {
                renderer.estado_jogo = ESTADO_VITORIA;

            }
            else{
                IniciarDeckCombate(&renderer);
                IniciarNovaRodada(&renderer);
            }
        } 
        
    }


    
    int done = 0, print_combat = 0, redraw = 0;
    
    

    switch (event.type) {
      case ALLEGRO_EVENT_TIMER:
        redraw = 1;

        
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
          keyboard_keys[i] &= ~GAME_KEY_SEEN;
        }
        break;

      case ALLEGRO_EVENT_KEY_DOWN:
        keyboard_keys[event.keyboard.keycode] = GAME_KEY_SEEN | GAME_KEY_DOWN;  //000001; 0000011;
        
        if (keyboard_keys[ALLEGRO_KEY_Q]) {
          done = 1;
          break;
        }
        
        

        if (renderer.estado_jogo == ESTADO_MENU) {
            
           
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT || event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                // Alterna entre 0 e 1
                renderer.botao_selecionado = !renderer.botao_selecionado;
            }

            
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                if (renderer.botao_selecionado == 0) {
                    
                    renderer.estado_jogo = ESTADO_ESCOLHER_CARTA;
                    renderer.player.vida_atual = 100;
                    renderer.player.escudo = 0;

                    renderer.round_atual = 0; 
                    IniciarNovaRodada(&renderer);    
                    
                    IniciarDeckCombate(&renderer);
                    break;
                } 
                else {
                    
                    done = 1;
                }
            }
        }

        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            
           
            if (renderer.estado_jogo == ESTADO_ESCOLHER_ALVO) {
                renderer.estado_jogo = ESTADO_ESCOLHER_CARTA;
            }
            
            else if (renderer.estado_jogo == ESTADO_ESCOLHER_CARTA) {
                
                
                ExecutarTurnoInimigos(&renderer);

               
                if (renderer.player.vida_atual <= 0) {
                    printf("GAME OVER\n");
                    renderer.estado_jogo = ESTADO_DERROTA;
                    break;
                } 
                else {
                   
                    IniciarTurnoJogador(&renderer);
                }
            }
        }
         

       else if (renderer.estado_jogo == ESTADO_ESCOLHER_CARTA) {
            
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                renderer.indice_selecionado--;
                if(renderer.indice_selecionado < 0) renderer.indice_selecionado = renderer.mao_tamanho - 1;
            }
           
            
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                renderer.indice_selecionado++;
                if(renderer.indice_selecionado > renderer.mao_tamanho - 1) renderer.indice_selecionado = 0;
            }
            

           
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
            
            // Só faz algo se tiver cartas na mão e estiver no ESTADO DE CARTA
           
            if (renderer.estado_jogo == ESTADO_ESCOLHER_CARTA && renderer.mao_tamanho > 0) {
                
                int index = renderer.indice_selecionado;
                int id_carta = renderer.mao[index];
                Carta c = BIBLIOTECA[id_carta]; 

                // Verifica energia
                if (renderer.energia.energia_atual >= c.custo) {
                    
                   
                    if (c.tipo == TIPO_ESPECIAL) {
                       printf("Jogou Especial: %s\n", c.nome);
                        renderer.energia.energia_atual -= c.custo;
                        
                        
                        for(int i=0; i < renderer.mao_tamanho; i++) {
                            
                            renderer.descarte[renderer.descarte_tamanho++] = renderer.mao[i];
                        }

                        
                        renderer.mao_tamanho = 0;
                        renderer.indice_selecionado = 0;


                        // COMPRA 5 NOVAS CARTAS
                        
                        for(int k=0; k<5; k++) {
                            ComprarCarta(&renderer);
                        }
                    }
                        
                        
                
                

                    // --- CASO 2: ATAQUE (Precisa selecionar alvo) ---
                    else if (c.tipo == TIPO_ATAQUE) {
                        renderer.estado_jogo = ESTADO_ESCOLHER_ALVO;
                        renderer.alvo_selecionado = 0;
                    } 
                    
                    // --- CASO 3: DEFESA (Usa imediatamente) ---
                    else if (c.tipo == TIPO_DEFESA) {
                        printf("Jogou Defesa: %s\n", c.nome);
                        
                        renderer.player.escudo += c.valor_escudo;
                        renderer.energia.energia_atual -= c.custo;

                        int id_jogada = renderer.mao[index];
                        renderer.descarte[renderer.descarte_tamanho++] = id_jogada;

                        
                        for(int i = index; i < renderer.mao_tamanho - 1; i++) {
                            renderer.mao[i] = renderer.mao[i+1];
                        }
                        renderer.mao_tamanho--;
                        
                        // Ajusta cursor
                        if (renderer.indice_selecionado >= renderer.mao_tamanho && renderer.mao_tamanho > 0) {
                            renderer.indice_selecionado = renderer.mao_tamanho - 1;
                        }
                    }

                } else {
                    printf("Sem energia suficiente!\n");
                }
            }
        }

    }
        
        else if (renderer.estado_jogo == ESTADO_ESCOLHER_ALVO) {
            
            // Navega entre os inimigos (0, 1, 2)
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
               int candidato = renderer.alvo_selecionado;
                
                // Tenta no máximo 4 vezes achar alguém vivo
                for (int i = 0; i < 4; i++) {
                    candidato++; 
                    if (candidato > 3) candidato = 0; // Volta ao início se passar do Boss

                    
                    int esta_vivo = 0;
                    if (candidato == 0 && renderer.mob_fraco_1.vida_atual > 0) esta_vivo = 1;
                    else if (candidato == 1 && renderer.mob_medio.vida_atual > 0) esta_vivo = 1;
                    else if (candidato == 2 && renderer.mob_forte.vida_atual > 0) esta_vivo = 1;
                    else if (candidato == 3 && renderer.boss.vida_atual > 0) esta_vivo = 1; 

                    
                    if (esta_vivo) {
                        renderer.alvo_selecionado = candidato;
                        break; 
                    }
                }
            }
            
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                         
           int candidato = renderer.alvo_selecionado;
                
                // Tenta no máximo 4 vezes achar alguém vivo
                for (int i = 0; i < 4; i++) {
                    candidato++; 
                    if (candidato > 3) candidato = 0; // Volta ao início se passar do Boss

                    
                    int esta_vivo = 0;
                    if (candidato == 0 && renderer.mob_fraco_1.vida_atual > 0) esta_vivo = 1;
                    else if (candidato == 1 && renderer.mob_medio.vida_atual > 0) esta_vivo = 1;
                    else if (candidato == 2 && renderer.mob_forte.vida_atual > 0) esta_vivo = 1;
                    else if (candidato == 3 && renderer.boss.vida_atual > 0) esta_vivo = 1;

                    // Se achou um vivo, confirma a troca e sai do loop
                    if (esta_vivo) {
                        renderer.alvo_selecionado = candidato;
                        break; 
                    }
                }
            }

           
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                renderer.estado_jogo = ESTADO_ESCOLHER_CARTA;
            }

            
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                int id_carta = renderer.mao[renderer.indice_selecionado];
                Carta c = BIBLIOTECA[id_carta];

                if (renderer.energia.energia_atual >= c.custo) {
                    // Define quem é o alvo baseado na seleção
                    Entidade* alvo = NULL;
                    if (renderer.alvo_selecionado == 0) alvo = &renderer.mob_fraco_1;
                    if (renderer.alvo_selecionado == 1) alvo = &renderer.mob_medio;
                    if (renderer.alvo_selecionado == 2) alvo = &renderer.mob_forte;
                    if (renderer.alvo_selecionado == 3) alvo = &renderer.boss;

                    // Só ataca se o alvo existir e estiver vivo
                    if (alvo != NULL && alvo->vida_atual > 0) {
                        printf("Atacando alvo %d com %s\n", renderer.alvo_selecionado, c.nome);
                        
                        int sobra = alvo->escudo - c.valor_ataque;
                        if( sobra >= 0){
                            alvo->escudo = sobra;
                        }else if(sobra < 0){
                            alvo->vida_atual = alvo->vida_atual + sobra;
                            if (alvo->vida_atual < 0) alvo->vida_atual = 0;
                        }
                       
                        
                        // Gasta energia e remove carta
                        renderer.energia.energia_atual -= c.custo;
                        
                        renderer.descarte[renderer.descarte_tamanho++] = id_carta;
                        
                        // Remove carta (descarte)
                        for(int i = renderer.indice_selecionado; i < renderer.mao_tamanho - 1; i++) {
                            renderer.mao[i] = renderer.mao[i+1];
                        }
                        renderer.mao_tamanho--;
                        if (renderer.indice_selecionado >= renderer.mao_tamanho && renderer.mao_tamanho > 0) 
                            renderer.indice_selecionado--;
                            

                        // VOLTA PARA SELEÇÃO DE CARTAS
                        renderer.estado_jogo = ESTADO_ESCOLHER_CARTA;
                    } 
                } else {
                    printf("Sem energia!\n");
                    renderer.estado_jogo = ESTADO_ESCOLHER_CARTA; // Cancela se não tiver energia
                }
            }
        }
        
        if (renderer.estado_jogo == ESTADO_VITORIA) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                
                renderer.estado_jogo = ESTADO_MENU;
                renderer.round_atual = 0;
                
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE || event.keyboard.keycode == ALLEGRO_KEY_Q) {
                done = 1;
            }
        }

        if (renderer.estado_jogo == ESTADO_DERROTA) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                
                renderer.estado_jogo = ESTADO_MENU;
                renderer.round_atual = 0; 
                
                
                renderer.player.vida_atual = 100; 
                break;
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE || event.keyboard.keycode == ALLEGRO_KEY_Q) {
                done = 1; // Sai do jogo
            }
        }

        if (keyboard_keys[ALLEGRO_KEY_X]) {
          renderer.player.vida_atual = 1;
          printf("vida player um");
        
        }
        
        if (keyboard_keys[ALLEGRO_KEY_SPACE]) {
          renderer.mob_forte.vida_atual = 0;
          renderer.mob_fraco_1.vida_atual = 0;
          renderer.mob_medio.vida_atual = 0;
          renderer.boss.vida_atual = 0;
        
        }

        if (keyboard_keys[ALLEGRO_KEY_ENTER]) {
          int id_escolhido = renderer.mao[renderer.indice_selecionado];
          Carta carta_jogada = BIBLIOTECA[id_escolhido];
        
        }
        

        break;
      case ALLEGRO_EVENT_KEY_UP:
        keyboard_keys[event.keyboard.keycode] &= ~GAME_KEY_DOWN;
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    }
    if (done) {
      break;
    }
    // You want to put your combat logic here.
    if (redraw) {
    Render(&renderer);
    
      redraw = 0;
    }
  }



  //animaco_personagns

  al_destroy_timer(timer);
  al_destroy_event_queue(queue);
  ClearRenderer(&renderer);
  return 0;
}

