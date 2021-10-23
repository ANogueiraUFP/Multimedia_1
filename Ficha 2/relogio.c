#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "glm.h"

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//#define VERTICES 20 

/**************************************
************* CONSTANTE PI ************
**************************************/
  
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

/**************************************
* AUXILIARES CONVERSÃO GRAUS-RADIANOS *
**************************************/

#define rtd(x) (180 * (x) / M_PI)
#define dtr(x) (M_PI * (x) / 180)

#define DEBUG 1

/**************************************
********** VARIÁVEIS GLOBAIS **********
**************************************/

typedef struct
{
  GLboolean doubleBuffer;
  GLint delay;
} Estado;

typedef struct
{
  GLint hor, min, seg;
} Horas;

typedef struct
{
  GLint numLados;
  GLfloat raio;
  GLfloat tamLado;
  Horas hora;
} Modelo;

Estado estado;
Modelo modelo;

/**************************************
*** INICIALIZAÇÃO DO AMBIENTE OPENGL **
**************************************/

void init(void)
{

  struct tm *current_time;
  time_t timer = time(0);

  /* Delay para o timer */
  estado.delay = 1000;

  modelo.tamLado = 1;
  modelo.numLados = 60;
  modelo.raio = 0.75;

  /* Ler hora do Sistema */
  current_time = localtime(&timer);
  modelo.hora.hor = current_time->tm_hour;
  modelo.hora.min = current_time->tm_min;
  modelo.hora.seg = current_time->tm_sec;

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
}

/**************************************
***** CALL BACKS DE JANELA/DESENHO ****
**************************************/

/* Callback para redimensionar janela */
void reshape(int width, int height)
{
  GLint size;

  if (width < height)
    size = width;
  else
    size = height;

  /* glViewport(botom, left, width, height)
     Define parte da janela a ser utilizada pelo OpenGL */
  glViewport(0, 0, (GLint)size, (GLint)size);

  /* Matriz Projeção
     Matriz onde se define como o mundo e apresentado na janela */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /* gluOrtho(left,right,bottom,top,near,far);
     Projeção ortogonal 3D, com profundidade (Z) entre -1 e 1 */
  glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, -1.0);

  /* Matriz Modelview
     Matriz onde são realizadas as tranformações dos modelos desenhados */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


/**************************************
** ESPAÇO PARA DEFINIÇÃO DAS ROTINAS **
****** AUXILIARES DE DESENHO ... ******
**************************************/


/* Callback de desenho */
void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  /* Espaço para chamada das rotinas auxiliares de desenho ... */


  glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(modelo.tamLado / 2, modelo.tamLado / 2);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(-modelo.tamLado / 2, modelo.tamLado / 2);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-modelo.tamLado / 2, -modelo.tamLado / 2);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex2f(modelo.tamLado / 2, -modelo.tamLado / 2);
  glEnd();

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
  glutTimerFunc(estado.delay, timer, 0);
  /* Acções do temporizador ...
     Não colocar aqui primitivas OpenGL de desenho glBegin, glEnd, etc.
     Simplesmente alterar os valores de modelo.hora.hor, modelo.hora.min e modelo.hora.seg */

  /* Redesenhar o ecrã (invoca o callback de desenho) */
  glutPostRedisplay();
}

/**************************************
*********** FUNÇÃO AJUDA **************
**************************************/

void imprime_ajuda(void)
{
  printf("\n\nDesenho de um quadrado\n");
  printf("h,H - Ajuda \n");
  printf("+   - Aumentar tamanho do Lado\n");
  printf("-   - Diminuir tamanho do Lado\n");
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
  case 27: // Tecla Escape
    exit(1);
    /* Ações sobre outras teclas */

  case 'h':
  case 'H':
    imprime_ajuda();
    break;

  case '+':
    if (modelo.tamLado < 1.8)
    {
      modelo.tamLado += 0.05;
      glutPostRedisplay(); 
    }
    break;
  case '-':
    if (modelo.tamLado > 0.2)
    {
      modelo.tamLado -= 0.05;
      glutPostRedisplay(); 
    }
    break;
  }

  if (DEBUG)
    printf("Carregou na tecla %c\n", key);
}

/* Callback para interação via teclado (largar a tecla) */
void keyUp(unsigned char key, int x, int y)
{
  if (DEBUG)
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
    //glutPostRedisplay();
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

void poligono(GLint n, GLfloat x0, GLfloat y0, GLfloat r)
{
  // podemos aqui ter como base a void desenhar_circunferencia abaixo?
}

/*PONTO ponto_medio(PONTO p1, PONTO p2)
{
    PONTO paux;
    paux.x = p1.x + p2.x / 2;
    paux.y = p1.y + p2.y / 2;
    return paux;
}

void desenhar_circunferencia(void)
{
    PONTO p0, p1;

    p0.x = p0.y = 0.0;
    p1.x = p1.y = 0.8;

    PONTO pc = ponto_medio(p0, p1);

    float dist = distancia(p0, p1);
    float raio = dist / 2;

    PONTO *pontos = calloc(VERTICES, sizeof(PONTO));

    pontos = coordenadas(VERTICES, pc, raio);

    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(0.0f, 0.0f, 1.0f);

    glBegin(GL_POLYGON);

    for (int i = 0; i < VERTICES; i++)
    {
        glColor3f(0.43f, 0.42f, 0.42f);
        glVertex2f(pontos[i].x, pontos[i].y);
    }

    glEnd();
    glFlush();
}

float distancia(PONTO p1, PONTO p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}*/

void mostrador(int n, PONTO p0, PONTO p1, PONTO pc, PONTO *pontos, PONTO *pontos1, PONTO *pontos2, SEMIRETA *retas)
{
  //desenha borda do relogio (cinza)
  glBegin(GL_POLYGON);
  pontos1 = coordenadas(n, pc, modelo.raio + 0.08);
  for (int i = 0; i < n; i++)
  {
    glColor3f(0.43f, 0.42f, 0.42f);
    glVertex2f(pontos1[i].x, pontos1[i].y);
    // printf("Ponto[%d]-->(%d, %d)\n", i, pontos[i].x, pontos[i].y);
  }
  glEnd();
  glFlush();

  //desenha interior do relogio (branco)
  glBegin(GL_POLYGON);
  pontos = coordenadas(n, pc, modelo.raio);
  for (int i = 0; i < n; i++)
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(pontos[i].x, pontos[i].y);
    // printf("Ponto[%d]-->(%d, %d)\n", i, pontos[i].x, pontos[i].y);
  }
  glEnd();
  glFlush();

  //centro do relogio (ponto central preto)
  glBegin(GL_POLYGON);
  pontos2 = coordenadas(n, pc, 0.02);
  for (int i = 0; i <= n; i++)
  {
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(pontos2[i].x, pontos2[i].y);
    // printf("Ponto[%d]-->(%d, %d)\n", i, pontos[i].x, pontos[i].y);
  }
  glEnd();
  glFlush();

  //desenhar traços das horas e minutos
  retas = coordenadas_semireta(pc, (distancia(p0, p1) / 2) + modelo.raio);
  int caracter = 49, count = 0;
  float anguloHoras = ((360.0 * M_PI / 180.0) / (float)60);
  for (int i = 0; i < 60; i++)
  {
    if (i % 5 == 0)
    {
      if (caracter > 57)
      {
        glRasterPos2f(((modelo.raio - 0.18) * cos(-anguloHoras * i + (M_PI) / 3) + pc.x) - 0.02, (modelo.raio - 0.18) * sin(-anguloHoras * i + (M_PI) / 3) + pc.y);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 49);

        glRasterPos2f(((modelo.raio - 0.18) * cos(-anguloHoras * i + (M_PI) / 3) + pc.x) + 0.02, (modelo.raio - 0.18) * sin(-anguloHoras * i + (M_PI) / 3) + pc.y);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + count);
        count++;
      }
      else
      {
        glRasterPos2f((modelo.raio - 0.18) * cos(-anguloHoras * i + (M_PI) / 3) + pc.x, (modelo.raio - 0.18) * sin(-anguloHoras * i + (M_PI) / 3) + pc.y);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, caracter);
      }
      caracter++;
    }

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(retas[i].p1.x, retas[i].p1.y);
    glVertex2f(retas[i].p2.x, retas[i].p2.y);
    glEnd();
    glFlush();
    // printf("Ponto[%d]-->(%d, %d)\n", i, pontos[i].x, pontos[i].y);
  }
}

void ponteiros(PONTO pc, SEMIRETA *retas)
{
  refreshtime();
  //ponteiros
  //retas = coordenadas_ponteiros(pc, modelo.raio);
  float angulo_horas, angulo_min;

  if (modelo.hora.hor > 12)
  {
    modelo.hora.hor = modelo.hora.hor - 12;
  }
  float anguloHoras = ((360.0 * M_PI / 180.0) / (float)12);
  float anguloMinSec = ((360.0 * M_PI / 180.0) / (float)60);

  //horas
  glLineWidth(4.0);
  glBegin(GL_LINES);
  glVertex2f(pc.x, pc.y);
  glVertex2f((modelo.raio - 0.27) * cos(-anguloHoras * modelo.hora.hor + (M_PI) / 2) + pc.x, (modelo.raio - 0.27) * sin(-anguloHoras * modelo.hora.hor + (M_PI) / 2) + pc.y);
  glEnd();
  glFlush();

  //minutos
  glLineWidth(2.0);
  glBegin(GL_LINES);
  glVertex2f(pc.x, pc.y);
  glVertex2f((modelo.raio - 0.20) * cos(-anguloMinSec * modelo.hora.min + (M_PI) / 2) + pc.x, (modelo.raio - 0.20) * sin(-anguloMinSec * modelo.hora.min + (M_PI) / 2) + pc.y);
  glEnd();
  glFlush();

  //segundos
  glLineWidth(1);
  glBegin(GL_LINES);
  glVertex2f(pc.x, pc.y);
  glVertex2f((modelo.raio - 0.15) * cos(-anguloMinSec * modelo.hora.seg + (M_PI) / 2) + pc.x, (modelo.raio - 0.15) * sin(-anguloMinSec * modelo.hora.seg + (M_PI) / 2) + pc.y);
  glEnd();
  glFlush();
}

void mostrador(int n, PONTO p0, PONTO p1, PONTO pc, PONTO *pontos, PONTO *pontos1, PONTO *pontos2, SEMIRETA *retas)
{
  //desenha borda do relogio (cinza)
  glBegin(GL_POLYGON);
  pontos1 = coordenadas(n, pc, modelo.raio + 0.08);
  for (int i = 0; i < n; i++)
  {
    glColor3f(0.43f, 0.42f, 0.42f);
    glVertex2f(pontos1[i].x, pontos1[i].y);
    // printf("Ponto[%d]-->(%d, %d)\n", i, pontos[i].x, pontos[i].y);
  }
  glEnd();
  glFlush();

  //desenha interior do relogio (branco)
  glBegin(GL_POLYGON);
  pontos = coordenadas(n, pc, modelo.raio);
  for (int i = 0; i < n; i++)
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(pontos[i].x, pontos[i].y);
    // printf("Ponto[%d]-->(%d, %d)\n", i, pontos[i].x, pontos[i].y);
  }
  glEnd();
  glFlush();

  //centro do relogio (ponto central preto)
  glBegin(GL_POLYGON);
  pontos2 = coordenadas(n, pc, 0.02);
  for (int i = 0; i <= n; i++)
  {
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(pontos2[i].x, pontos2[i].y);
    // printf("Ponto[%d]-->(%d, %d)\n", i, pontos[i].x, pontos[i].y);
  }
  glEnd();
  glFlush();

  //desenhar traços das horas e minutos
  retas = coordenadas_semireta(pc, (distancia(p0, p1) / 2) + modelo.raio);
  int caracter = 49, count = 0;
  float anguloHoras = ((360.0 * M_PI / 180.0) / (float)60);
  for (int i = 0; i < 60; i++)
  {
    if (i % 5 == 0)
    {
      if (caracter > 57)
      {
        glRasterPos2f(((modelo.raio - 0.18) * cos(-anguloHoras * i + (M_PI) / 3) + pc.x) - 0.02, (modelo.raio - 0.18) * sin(-anguloHoras * i + (M_PI) / 3) + pc.y);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 49);

        glRasterPos2f(((modelo.raio - 0.18) * cos(-anguloHoras * i + (M_PI) / 3) + pc.x) + 0.02, (modelo.raio - 0.18) * sin(-anguloHoras * i + (M_PI) / 3) + pc.y);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + count);
        count++;
      }
      else
      {
        glRasterPos2f((modelo.raio - 0.18) * cos(-anguloHoras * i + (M_PI) / 3) + pc.x, (modelo.raio - 0.18) * sin(-anguloHoras * i + (M_PI) / 3) + pc.y);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, caracter);
      }
      caracter++;
    }

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(retas[i].p1.x, retas[i].p1.y);
    glVertex2f(retas[i].p2.x, retas[i].p2.y);
    glEnd();
    glFlush();
    // printf("Ponto[%d]-->(%d, %d)\n", i, pontos[i].x, pontos[i].y);
  }
}


/**************************************
************ FUNÇÃO MAIN **************
**************************************/

int main(int argc, char **argv)
{
  estado.doubleBuffer = GL_FALSE; // Colocar GL_TRUE para ligar o Double Buffer

  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(300, 300);
  glutInitDisplayMode(((estado.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB);
  if (glutCreateWindow("Exemplo") == GL_FALSE)
    exit(1);

  init();

  imprime_ajuda();

  /* Registar callbacks do GLUT */

  /* callbacks de janelas/desenho */
  glutReshapeFunc(reshape);
  glutDisplayFunc(draw);

  /* Callbacks de teclado */
  glutKeyboardFunc(key);
  //glutKeyboardUpFunc(keyUp);
  //glutSpecialFunc(specialKey);
  //glutSpecialUpFunc(specialKeyUp);

  /* Callbacks timer/idle */
  //glutTimerFunc(estado.delay, timer, 0);
  //glutIdleFunc(idle);

  /* Começar loop */
  glutMainLoop();

  return 0;
}
