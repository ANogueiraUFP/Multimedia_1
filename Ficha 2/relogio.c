#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <GL/glut.h>

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

typedef struct ponto
{
  float x, y;
} PONTO;

typedef struct semireta
{
  PONTO p1, p2;
} SEMIRETA;

Estado estado;
Modelo modelo;

/**************************************
*** INICIALIZAÇÃO DO AMBIENTE OPENGL **
**************************************/

void init(void)
{

  /* Delay para o timer */
  estado.delay = 1000;

  modelo.tamLado = 1;
  modelo.numLados = 60;
  modelo.raio = 0.75;

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
}

/**************************************
***** CALL BACKS DE JANELA/DESENHO ****
**************************************/
void refreshtime(void)
{
  struct tm *current_time;
  time_t timer = time(0);

  /* Ler hora do Sistema */
  current_time = localtime(&timer);
  modelo.hora.hor = current_time->tm_hour;
  modelo.hora.min = current_time->tm_min;
  modelo.hora.seg = current_time->tm_sec;
}
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

void menu(void)
{
  printf("\n\nDesenho de um poligono\n");
  printf("+   - Aumentar velocidade\n");
  printf("-   - Diminuir velocidade\n");
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

  case '+':
    
    //estado.delay=50;
    //modelo.hora.hor=modelo.hora.hor +50;
    break;

  case '-':
    
    break;
  }

  if (DEBUG)
    printf("Carregou na tecla %c\n", key);
}

PONTO ponto_medio(PONTO p1, PONTO p2)
{
  PONTO paux;
  paux.x = p1.x + p2.x / 2;
  paux.y = p1.y + p2.y / 2;
  return paux;
}

float distancia(PONTO p1, PONTO p2)
{
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

PONTO *poligono(int n, PONTO pc, float raio)
{
  PONTO *pontos = calloc(n, sizeof(PONTO));
  float angulo = (360.0 * M_PI / 180.0) / (float)n;
  float a = 0.0;
  for (int i = 0; i < n; i++, a += angulo)
  {
    pontos[i].x = raio * cos(a) + pc.x;
    pontos[i].y = raio * sin(a) + pc.y;
  }
  return pontos;
}

/**
 * Retorna um array com poligono de varias retas, neste caso utilizadas para marcar os traços das horas e dos minutos
 * */
SEMIRETA *poligono_semireta(PONTO pc, float raio)
{
  SEMIRETA *retas = (SEMIRETA *)calloc(60, sizeof(SEMIRETA));
  float angulo = (360.0 * M_PI / 180.0) / (float)60;
  float a = 0.0;
  for (int i = 0; i < 60; i++, a += angulo)
  {
    retas[i].p1.x = raio * cos(a) + pc.x;
    retas[i].p1.y = raio * sin(a) + pc.y;

    if (i % 5 == 0)
    {
      retas[i].p2.x = (raio - 0.12) * cos(a) + pc.x;
      retas[i].p2.y = (raio - 0.12) * sin(a) + pc.y;
    }
    else
    {
      retas[i].p2.x = (raio - 0.08) * cos(a) + pc.x;
      retas[i].p2.y = (raio - 0.08) * sin(a) + pc.y;
    }
  }
  return retas;
}

void ponteiros(PONTO pc, SEMIRETA *retas)
{
  refreshtime();
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
  //desenha contorno do relogio (cinza)
  glBegin(GL_POLYGON);
  pontos1 = poligono(n, pc, modelo.raio + 0.08);
  for (int i = 0; i < n; i++)
  {
    glColor3f(0.43f, 0.42f, 0.42f);
    glVertex2f(pontos1[i].x, pontos1[i].y);
  }
  glEnd();
  glFlush();

  //desenha interior do relogio (branco)
  glBegin(GL_POLYGON);
  pontos = poligono(n, pc, modelo.raio);
  for (int i = 0; i < n; i++)
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(pontos[i].x, pontos[i].y);
  }
  glEnd();
  glFlush();

  //centro do relogio
  glBegin(GL_POLYGON);
  pontos2 = poligono(n, pc, 0.02);
  for (int i = 0; i <= n; i++)
  {
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(pontos2[i].x, pontos2[i].y);
  }
  glEnd();
  glFlush();

  //desenhar traços horas + minutos
  retas = poligono_semireta(pc, (distancia(p0, p1) / 2) + modelo.raio);
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
  }
}

void desenhar_relogio(void)
{
  int n = 360;
  PONTO p0, p1, pc, *pontos, *pontos1, *pontos2;
  SEMIRETA *retas;
  p0.x = p0.y = 0;
  p1.x = p1.y = 0;
  pc = ponto_medio(p0, p1);

  glClear(GL_COLOR_BUFFER_BIT);

  mostrador(n, p0, p1, pc, pontos, pontos1, pontos2, retas);
  ponteiros(pc, retas);
}

/**************************************
************ FUNÇÃO MAIN **************
**************************************/

int main(int argc, char **argv)
{
  estado.doubleBuffer = GL_FALSE; // Colocar GL_TRUE para ligar o Double Buffer

  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(700, 700);
  glutInitDisplayMode(((estado.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB);
  if (glutCreateWindow("Relógio") == GL_FALSE)
    exit(1);

   init();
  menu();

  /* Registar callbacks do GLUT */

  /* callbacks de janelas/desenho */
  glutReshapeFunc(reshape);
  glutDisplayFunc(desenhar_relogio);

  /* Callbacks de teclado */
  glutKeyboardFunc(key);
  //glutKeyboardUpFunc(keyUp);
  //glutSpecialFunc(specialKey);
  //glutSpecialUpFunc(specialKeyUp);

  /* Callbacks timer/idle */
  glutTimerFunc(estado.delay, timer, 0);
  glutIdleFunc(idle);

  /* Começar loop */
  glutMainLoop();

  return 0;
}