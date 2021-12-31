#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/**************************************
************* CONSTANTE PI ************
**************************************/

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

/**************************************
 * AUXILIARES CONVERSÃO GRAUS-RADIANOS *
 **************************************/

#define RAD(x) (M_PI * (x) / 180)
#define GRAUS(x) (180 * (x) / M_PI)

/**************************************
******** CONSTANTES AUXILIARES ********
**************************************/

#define DEBUG 0
#define LARGURA_CAMPO 800
#define ALTURA_CAMPO 600
#define LARGURA_RAQUETES 10
#define ALTURA_RAQUETES 50
#define LINHA_SERVICO 50
#define VELOCIDADE_BOLA 8
#define VELOCIDADE_RAQUETES 15
#define TAMANHO_BOLA 10
#define DELAY_MOVIMENTO 30
#define DELAY_TECLAS 20

/**************************************
********** VARIÁVEIS GLOBAIS **********
**************************************/

typedef struct
{
  GLboolean q, a, p, l;
} Teclas;

typedef struct
{
  GLboolean doubleBuffer;  // Double Buffer ON/OFF
  GLint delayMovimento;    // Delay do timer (timer)
  GLint delayTeclas;       // Delay do timer (timerTeclas)
  Teclas teclas;           // Teclas DOWN/UP (Key e KeyUp)
  GLuint menu_vel_bola_id; // Guarda id menu (cria_menu)
  GLuint menu_tam_bola_id; // Guarda id menu (cria_menu)
  GLuint menu_vel_raq_id;  // Guarda id menu (cria_menu)
  GLuint menu_tam_raq_id;  // Guarda id menu (cria_menu)
  GLuint menu_id;          // Guarda id menu (cria_menu)
  GLboolean menuActivo;    // Menu openGL está ON/OFF
  GLboolean debug;         // Mensagens debug ON/OFF
} Estado;

typedef struct
{
  GLfloat x, y;    // Posição da raquete
  GLint pontuacao; // Pontuação do jogador
} Raquete;

typedef struct
{
  GLfloat x, y;     // Posição da bola
  GLint velocidade; // Velocidade da bola
  GLfloat direccao; // Direcção que a bola segue em RAD
  GLint tamanho;    // Tamanho da bola
} Bola;

typedef struct
{
  Raquete jogador1, jogador2; // Dados das raquetes/pontuação
  Bola bola;                  // Dados da bola(posição/direcção/velocidade)
  GLint alturaRaquetes;       // Altura das raquetes (alterada por menu)
  GLint velocidadeRaquetes;   // Velocidade das raquetes (alterada por menu)
  GLint servico;              // Jogador que fez o último serviço (1/2)
  GLboolean parado;           // Jogo parado (GL_TRUE/GL_FALSE)
} Modelo;

Estado estado;
Modelo modelo;

/**************************************
*** INICIALIZAÇÃO DO AMBIENTE OPENGL **
**************************************/

void inicia_jogo()
{

  modelo.servico = (modelo.servico == 2) ? 1 : 2;

  modelo.jogador1.x = LINHA_SERVICO + LARGURA_RAQUETES * .5;
  modelo.jogador2.x = LARGURA_CAMPO - (LINHA_SERVICO + LARGURA_RAQUETES * .5);
  modelo.jogador1.y = modelo.jogador2.y = ALTURA_CAMPO * .5;
  modelo.bola.x = LARGURA_CAMPO * .5;
  modelo.bola.y = ALTURA_CAMPO * .5;

  modelo.bola.direccao = RAD(rand() % 120 - 60);
  if (modelo.servico == 2)
    modelo.bola.direccao = M_PI - modelo.bola.direccao;

  if (modelo.bola.direccao < 0)
    modelo.bola.direccao += 2 * M_PI;
}

void init(void)
{

  srand((unsigned)time(NULL));

  modelo.bola.velocidade = VELOCIDADE_BOLA;
  modelo.bola.tamanho = TAMANHO_BOLA;
  modelo.alturaRaquetes = ALTURA_RAQUETES;
  modelo.velocidadeRaquetes = VELOCIDADE_RAQUETES;
  modelo.servico = 2;
  modelo.jogador1.pontuacao = modelo.jogador2.pontuacao = 0;
  modelo.parado = GL_FALSE;

  estado.menuActivo = GL_FALSE;
  estado.delayMovimento = DELAY_MOVIMENTO;
  estado.delayTeclas = DELAY_TECLAS;

  estado.teclas.a = estado.teclas.q = estado.teclas.l = estado.teclas.p = GL_FALSE;

  inicia_jogo();

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);

  glutIgnoreKeyRepeat(GL_TRUE);
}

/**************************************
***** CALL BACKS DE JANELA/DESENHO ****
**************************************/

/* Callback para redimensionar janela */
void reshape(int width, int height)
{
  // glViewport(botom, left, width, height)
  // Define parte da janela a ser utilizada pelo OpenGL
  glViewport(0, 0, (GLint)width, (GLint)height);

  // Matriz Projeção
  // Matriz onde se define como o mundo e apresentado na janela
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // gluOrtho2D(left,right,bottom,top);
  // Projeção ortogonal 2D, com profundidade (Z) entre -1 e 1
  gluOrtho2D(0, LARGURA_CAMPO, 0, ALTURA_CAMPO);

  // Matriz Modelview
  // Matriz onde são realizadas as tranformações dos modelos desenhados
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/**************************************
** ESPAÇO PARA DEFINIÇÃO DAS ROTINAS **
****** AUXILIARES DE DESENHO ... ******
**************************************/

void strokeString(char *str, double x, double y, double z, double s)
{
  int i, n;

  n = (int)strlen(str);
  glPushMatrix();
  glColor3d(0.0, 0.0, 0.0);
  glTranslated(x, y, z);
  glScaled(s, s, s);
  for (i = 0; i < n; i++)
    glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)str[i]);

  glPopMatrix();
}

void bitmapString(char *str, double x, double y)
{
  int i, n;

  // fonte pode ser:
  // GLUT_BITMAP_8_BY_13
  // GLUT_BITMAP_9_BY_15
  // GLUT_BITMAP_TIMES_ROMAN_10
  // GLUT_BITMAP_TIMES_ROMAN_24
  // GLUT_BITMAP_HELVETICA_10
  // GLUT_BITMAP_HELVETICA_12
  // GLUT_BITMAP_HELVETICA_18
  //
  // int glutBitmapWidth  	(	void *font , int character);
  // devolve a largura de um carácter
  //
  // int glutBitmapLength 	(	void *font , const unsigned char *string );
  // devolve a largura de uma string (soma da largura de todos os caracteres)

  n = (int)strlen(str);
  glRasterPos2d(x, y);
  for (i = 0; i < n; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
}

void bitmapCenterString(char *str, double x, double y)
{
  int i, n;

  n = (int)strlen(str);
  glRasterPos2d(x - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)str) * 0.5, y);
  for (i = 0; i < n; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
}

void desenha_campo(int p1, int p2)
{
  char str[255];

  glLineWidth(3);
  glLineStipple(2, 0xFF00);

  glEnable(GL_LINE_STIPPLE);
  glBegin(GL_LINES);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex2f(LARGURA_CAMPO * .5, 0);
  glVertex2f(LARGURA_CAMPO * .5, ALTURA_CAMPO);
  glEnd();
  glDisable(GL_LINE_STIPPLE);

  snprintf(str, 255, "%d", p1);
  bitmapString(str, LARGURA_CAMPO * .4, ALTURA_CAMPO * .9);
  snprintf(str, 255, "%d", p2);
  bitmapString(str, LARGURA_CAMPO * .6, ALTURA_CAMPO * .9);

  if (estado.debug)
  {
    snprintf(str, 255, "ang:%.1f*PI\n x:%.1f\n y:%.1f ", modelo.bola.direccao / M_PI, modelo.bola.x, modelo.bola.y);
    bitmapCenterString(str, LARGURA_CAMPO * .5, ALTURA_CAMPO * .1);
  }
}

void desenha_raquete(Raquete raq)
{
  glBegin(GL_POLYGON);
  glVertex2f(raq.x - LARGURA_RAQUETES * .5, raq.y - modelo.alturaRaquetes * .5);
  glVertex2f(raq.x + LARGURA_RAQUETES * .5, raq.y - modelo.alturaRaquetes * .5);
  glVertex2f(raq.x + LARGURA_RAQUETES * .5, raq.y + modelo.alturaRaquetes * .5);
  glVertex2f(raq.x - LARGURA_RAQUETES * .5, raq.y + modelo.alturaRaquetes * .5);
  glEnd();
}

void desenha_bola(Bola bola)
{
  glPointSize(modelo.bola.tamanho);

  glBegin(GL_POINTS);
  glVertex2f(bola.x, bola.y);
  glEnd();
}

// ... Definição das rotinas auxiliares de desenho ...

/* Callback de desenho */
void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT);

  // ... chamada das rotinas auxiliares de desenho ...

  desenha_campo(modelo.jogador1.pontuacao, modelo.jogador2.pontuacao);

  glColor3f(1.0f, 0.0f, 0.0f);
  desenha_raquete(modelo.jogador1);

  glColor3f(0.0f, 0.0f, 1.0f);
  desenha_raquete(modelo.jogador2);

  glColor3f(1.0f, 1.0f, 0.0f);
  desenha_bola(modelo.bola);

  if (modelo.parado)
  {
    glColor3f(0.7, 0.7, 0.7);
    bitmapCenterString("Jogo Parado, use o menu para continuar", LARGURA_CAMPO * .5, ALTURA_CAMPO * .5);
  }

  glFlush();
  if (estado.doubleBuffer)
    glutSwapBuffers();
}

/**************************************
******** CALLBACKS TIME/IDLE **********
**************************************/

/* Callback Idle */
void idle(void)
{
  /* Ações a tomar quando o GLUT está idle */

  /* Redesenhar o ecrã */
  // glutPostRedisplay();
}

/* Callback de temporizador */
void timer(int value)
{

  glutTimerFunc(estado.delayMovimento, timer, 0);
  // ... Ações do temporizador ...

  if (estado.menuActivo || modelo.parado) // Sair no caso do jogo estar parado ou menu estar activo
    return;

  modelo.bola.x = (modelo.bola.velocidade * cos(modelo.bola.direccao) + modelo.bola.x);
  modelo.bola.y = (modelo.bola.velocidade * sin(modelo.bola.direccao) + modelo.bola.y);

  if (modelo.bola.x - TAMANHO_BOLA * 2 < 0)
  {
    modelo.bola.x = modelo.bola.x;
    modelo.bola.y = modelo.bola.y;
    modelo.jogador2.pontuacao++;
    inicia_jogo();
  }
  if (modelo.bola.x + TAMANHO_BOLA * 2 > LARGURA_CAMPO)
  {
    modelo.bola.x = modelo.bola.x;
    modelo.bola.y = modelo.bola.y;
    modelo.jogador1.pontuacao++;
    inicia_jogo();
  }

  if (modelo.bola.y - TAMANHO_BOLA / 2 < 0 || modelo.bola.y + TAMANHO_BOLA / 2 > ALTURA_CAMPO)
  {
    modelo.bola.direccao = -modelo.bola.direccao;
  }

  if (modelo.bola.x - TAMANHO_BOLA / 2 < modelo.jogador1.x + LARGURA_RAQUETES / 2 &&
      modelo.bola.x + TAMANHO_BOLA / 2 > modelo.jogador1.x - LARGURA_RAQUETES / 2 &&
      modelo.bola.y - TAMANHO_BOLA / 2 < modelo.jogador1.y + ALTURA_RAQUETES / 2 &&
      modelo.bola.y + TAMANHO_BOLA / 2 > modelo.jogador1.y - ALTURA_RAQUETES / 2)
  {
    modelo.bola.velocidade = -modelo.bola.velocidade;
    modelo.bola.direccao = -modelo.bola.direccao;
  }

  if (modelo.bola.x - TAMANHO_BOLA / 2 < modelo.jogador2.x + LARGURA_RAQUETES / 2 &&
      modelo.bola.x + TAMANHO_BOLA / 2 > modelo.jogador2.x - LARGURA_RAQUETES / 2 &&
      modelo.bola.y - TAMANHO_BOLA / 2 < modelo.jogador2.y + ALTURA_RAQUETES / 2 &&
      modelo.bola.y + TAMANHO_BOLA / 2 > modelo.jogador2.y - ALTURA_RAQUETES / 2)
  {
    modelo.bola.velocidade = -modelo.bola.velocidade;
    modelo.bola.direccao = -modelo.bola.direccao;
  }

  // Redesenhar o ecrã
  glutPostRedisplay();
}

void timerTeclas(int value)
{
  glutTimerFunc(estado.delayTeclas, timerTeclas, 0);
  // ... Ações das teclas ...

  if (estado.menuActivo || modelo.parado)
    return;

  if (estado.teclas.a && modelo.jogador1.y > 0 + ALTURA_RAQUETES / 2)
  {
    modelo.jogador1.y -= 5;
  }
  if (estado.teclas.q && modelo.jogador1.y < ALTURA_CAMPO - ALTURA_RAQUETES / 2)
  {
    modelo.jogador1.y += 5;
  }
  if (estado.teclas.l && modelo.jogador2.y > 0 + ALTURA_RAQUETES / 2)
  {
    modelo.jogador2.y -= 5;
  }
  if (estado.teclas.p && modelo.jogador2.y < ALTURA_CAMPO - ALTURA_RAQUETES / 2)
  {
    modelo.jogador2.y += 5;
  }

  // Não é necessário redesenhar o ecrã, é feito no timer de animação
}

/**************************************
*********** FUNÇÃO AJUDA **************
**************************************/

void imprime_ajuda(void)
{
  printf("\n\nMenu de ajuda\n");
  printf("h,H - Ajuda \n");
  printf("q,Q - Jogador 1 cima\n");
  printf("a,A - Jogador 1 baixo\n");
  printf("P,p - Jogador 2 cima\n");
  printf("L,l - Jogador 2 baixo\n");
  printf("D,d - Debug On/OFF\n");
  printf("ESC - Sair\n");
}

/**************************************
********* CALLBACKS TECLADO ***********
**************************************/

/* Callback para interação via teclado (carregar na tecla) */
void key(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 27:
    exit(1);

    /* ... Ações sobre outras teclas ... */

  case 'h':
  case 'H':
    imprime_ajuda();
    break;
  case 'i':
  case 'I':
    inicia_jogo();
    break;
  case 'Q':
  case 'q':
    estado.teclas.q = GL_TRUE;
    break;
  case 'A':
  case 'a':
    estado.teclas.a = GL_TRUE;
    break;
  case 'P':
  case 'p':
    estado.teclas.p = GL_TRUE;
    break;
  case 'L':
  case 'l':
    estado.teclas.l = GL_TRUE;
    break;
  case 'D':
  case 'd':
    estado.debug = !estado.debug;
    if (estado.menuActivo || modelo.parado)
      glutPostRedisplay();
    printf("DEBUG is %s\n", (estado.debug) ? "ON" : "OFF");
    break;
  }

  if (estado.debug)
    printf("Carregou na tecla %c\n", key);
}

/* Callback para interação via teclado (largar a tecla) */
void keyUp(unsigned char key, int x, int y)
{
  switch (key)
  {
    // ... Ações sobre largar teclas ...

  case 'Q':
  case 'q':
    estado.teclas.q = GL_FALSE;
    break;
  case 'A':
  case 'a':
    estado.teclas.a = GL_FALSE;
    break;
  case 'P':
  case 'p':
    estado.teclas.p = GL_FALSE;
    break;
  case 'L':
  case 'l':
    estado.teclas.l = GL_FALSE;
    break;
  }

  if (estado.debug)
    printf("Largou a tecla %c\n", key);
}

/* Callback para interacção via teclas especiais (carregar na tecla) */
void specialKey(int key, int x, int y)
{
  /* Ações sobre outras teclas especiais
      GLUT_KEY_F1 ... GLUT_KEY_F12
      GLUT_KEY_UP
      GLUT_KEY_DOWN
      GLUT_KEY_LEFT
      GLUT_KEY_RIGHT
      GLUT_KEY_PAGE_UP
      GLUT_KEY_PAGE_DOWN
      GLUT_KEY_HOME
      GLUT_KEY_END
      GLUT_KEY_INSERT */

  switch (key)
  {
    /* Redesenhar o ecrã */
    // glutPostRedisplay();
  }

  if (DEBUG)
    printf("Carregou na tecla especial %d\n", key);
}

/* Callback para interação via teclas especiais (largar a tecla) */
void specialKeyUp(int key, int x, int y)
{

  if (DEBUG)
    printf("Largou a tecla especial %d\n", key);
}

/**************************************
************ FUNÇÕES MENU *************
**************************************/

void menuStatus(int status, int x, int y)
{
  /* status => GLUT_MENU_IN_USE, GLUT_MENU_NOT_IN_USE
     x,y    => coordenadas do ponteiro quando se entra no menu
  */

  if (status == GLUT_MENU_IN_USE)
  {
    estado.menuActivo = GL_TRUE;
  }
  else
  {
    estado.menuActivo = GL_FALSE;
  }

  if (estado.debug)
    printf("MenuStatus status:%d coord:%d %d\n", status, x, y);
}

void menu(int opcao)
{

  switch (opcao)
  {
  case 0:
    exit(1);
    break;
  // Velocidade da bola
  case 1:
    if (modelo.bola.velocidade > 2)
      modelo.bola.velocidade -= 2;
    break;
  case 2:
    if (modelo.bola.velocidade < 16)
      modelo.bola.velocidade += 2;
    break;
  // Tamanho da bola
  case 3:
    if (modelo.bola.tamanho > 5)
      modelo.bola.tamanho--;
    break;
  case 4:
    if (modelo.bola.tamanho < 15)
      modelo.bola.tamanho++;
    break;
  // Velocidade da raquete
  case 5:
    if (modelo.velocidadeRaquetes > 10)
      modelo.velocidadeRaquetes -= 5;
    break;
  case 6:
    if (modelo.velocidadeRaquetes < 30)
      modelo.velocidadeRaquetes += 5;
    break;
  // Tamanho da raquete
  case 7:
    if (modelo.alturaRaquetes > 30)
      modelo.alturaRaquetes -= 10;
    break;
  case 8:
    if (modelo.velocidadeRaquetes < 100)
      modelo.alturaRaquetes += 10;
    break;
  case 9:
    modelo.parado = !modelo.parado;
    if (modelo.parado)
    {
      glutChangeToMenuEntry(1, "Começar", 9);
      glutPostRedisplay();
    }
    else
    {
      glutChangeToMenuEntry(1, "Parar", 9);
    }
    break;
  }
}

void cria_menu()
{

  // Funções de manipulação de menus
  // id=glutCreateMenu(menu)  - cria um menu tratado pela função menu e devolve o id
  // glutDestroyMenu(id)      - destroi o menu identificado por id
  // id=glutGetMenu()         - devolve o id do menu actual
  // glutSetMenu(id)          - torna actual o menu com o id
  //
  // glutAddMenuEntry("texto", valor) -  adiciona uma entrada ao menu actual
  // glutChangeToMenuEntry(item, "novo texto", novo_valor)
  //                                  -  altera a entrada item(1,2,3..) do menu actual
  // glutAddSubMenu("texto", id)      -  adiciona o submenu id ao menu actual
  // glutChangeToSubMenu(item, "novo texto", novo_id)
  //                                  -  altera a entrada submenu item(1,2,3..) do menu actual
  // glutRemoveMenuItem(item)         - apaga a entrada item(1,2,3...)
  //
  // glutAttachMenu(botao)            - associa o menu actual ao botao
  // glutDetachMenu(botao)            - desassocia o menu associado ao botao GLUT_RIGHT_BUTTON
  // botao = GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON ou GLUT_RIGHT_BUTTON

  /* DEFINIR MENUS */
  estado.menu_vel_bola_id = glutCreateMenu(menu); // Regista função que trata do menu
  glutAddMenuEntry("Diminuir", 1);
  glutAddMenuEntry("Aumentar", 2);

  estado.menu_tam_bola_id = glutCreateMenu(menu); // Regista função que trata do menu
  glutAddMenuEntry("Diminuir", 3);
  glutAddMenuEntry("Aumentar", 4);

  estado.menu_vel_raq_id = glutCreateMenu(menu); // Regista função que trata do menu
  glutAddMenuEntry("Diminuir", 5);
  glutAddMenuEntry("Aumentar", 6);

  estado.menu_tam_raq_id = glutCreateMenu(menu); // Regista função que trata do menu
  glutAddMenuEntry("Diminuir", 7);
  glutAddMenuEntry("Aumentar", 8);

  estado.menu_id = glutCreateMenu(menu); // Regista função que trata do menu
  glutAddMenuEntry("Parar", 9);

  // Adicionar submenus
  glutAddSubMenu("Velocidade da bola", estado.menu_vel_bola_id);
  glutAddSubMenu("Tamanho da bola", estado.menu_tam_bola_id);
  glutAddSubMenu("Velocidade da raquete", estado.menu_vel_raq_id);
  glutAddSubMenu("Altura da raquete", estado.menu_tam_raq_id);

  glutAddMenuEntry("Sair", 0);

  glutAttachMenu(GLUT_RIGHT_BUTTON); // Agarrar menu actual ao botão
}

/**************************************
************ FUNÇÃO MAIN **************
**************************************/

int main(int argc, char **argv)
{
  estado.doubleBuffer = GL_TRUE;

  glutInit(&argc, argv);
  // glutInitWindowPosition(0, 0);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - LARGURA_CAMPO) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - ALTURA_CAMPO) / 2);
  glutInitWindowSize(LARGURA_CAMPO, ALTURA_CAMPO);
  glutInitDisplayMode(((estado.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB);
  if (glutCreateWindow("Ping-Pong") == GL_FALSE)
    exit(1);

  init();

  imprime_ajuda();

  /* Registar callbacks do GLUT */

  // Callbacks de janelas/desenho
  glutReshapeFunc(reshape);
  glutDisplayFunc(draw);

  // Callbacks de teclado
  glutKeyboardFunc(key);
  glutKeyboardUpFunc(keyUp);
  // glutSpecialFunc(SpecialKey);
  // glutSpecialUpFunc(SpecialKeyUp);
  //  Callbacks timer/idle
  glutTimerFunc(estado.delayMovimento, timer, 0);
  glutTimerFunc(estado.delayTeclas, timerTeclas, 0);

  // Menus
  cria_menu();
  glutMenuStatusFunc(menuStatus);

  // Começar
  glutMainLoop();

  return 0;
}
