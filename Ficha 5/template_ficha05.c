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
******** MACROS AUXILIARES ********
**************************************/

#define DEBUG 1

#define DELAY_MOVIMENTO 20
#define RAIO_ROTACAO 20

#define LARGURA_BASE 4
#define COMPRIMENTO_BASE 7
#define ALTURA_BASE 1

#define LARGURA_TORRE 2
#define COMPRIMENTO_TORRE 2
#define ALTURA_TORRE 0.5

#define COMPRIMENTO_CANHAO 4
#define RAIO_CANHAO 0.2

/**************************************
********** VARIÁVEIS GLOBAIS **********
**************************************/

float EIXOS = 0;

typedef struct
{
  GLboolean q, a, z, x, up, down, left, right;
} Teclas;

typedef struct
{
  GLfloat x, y, z;
} Pos;

typedef struct
{
  Pos eye, center, up;
  GLfloat fov;
} Camera;

typedef struct
{
  GLboolean doubleBuffer;
  GLint delayMovimento;
  Teclas teclas;
  GLuint menu_id;
  GLboolean menuActivo;
  Camera camera;
  GLboolean debug;
  GLboolean ortho;
} Estado;

typedef struct
{
  GLfloat x, y;
  GLint pontuacao;
} Raquete;

typedef struct
{
  GLfloat x, y;
  GLfloat velocidade;
  GLfloat direccao;
  GLfloat direccaoRodas;
  GLfloat angTorre;
  GLfloat angCanhao;
} Tanque;

typedef struct
{
  Tanque tanque;
  GLboolean parado;
} Modelo;

Estado estado;
Modelo modelo;

/**************************************
*** INICIALIZAÇÃO DO AMBIENTE OPENGL **
**************************************/

void inicia_modelo()
{
  modelo.tanque.x = 20;
  modelo.tanque.y = 0;
  modelo.tanque.velocidade = 0;
  modelo.tanque.direccao = 0;
  modelo.tanque.direccaoRodas = 0;
  modelo.tanque.angTorre = 0;
  modelo.tanque.angCanhao = 0;
}

void init(void)
{

  srand((unsigned)time(NULL));

  modelo.parado = GL_FALSE;

  estado.debug = DEBUG;
  estado.menuActivo = GL_FALSE;
  estado.delayMovimento = DELAY_MOVIMENTO;
  estado.camera.eye.x = 40;
  estado.camera.eye.y = 40;
  estado.camera.eye.z = 40;
  estado.camera.center.x = 0;
  estado.camera.center.y = 0;
  estado.camera.center.z = 0;
  estado.camera.up.x = 0;
  estado.camera.up.y = 0;
  estado.camera.up.z = 1;
  estado.ortho = GL_TRUE;
  estado.camera.fov = 60;

  estado.teclas.a = estado.teclas.q = estado.teclas.z = estado.teclas.x =
      estado.teclas.up = estado.teclas.down = estado.teclas.left = estado.teclas.right = GL_FALSE;

  inicia_modelo();

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  // glutIgnoreKeyRepeat(GL_TRUE);
}

/**************************************
***** CALL BACKS DE JANELA/DESENHO ****
**************************************/

/* Callback para redimensionar janela */
void reshape(int width, int height)
{
  // glViewport(botom, left, width, height)
  // define parte da janela a ser utilizada pelo OpenGL
  glViewport(0, 0, (GLint)width, (GLint)height);

  // Matriz Projeccao
  // Matriz onde se define como o mundo é apresentado na janela
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // gluOrtho2D(left,right,bottom,top);
  // projeção ortogonal 2D, com profundidade (Z) entre -1 e 1

  if (estado.debug)
    printf("Reshape %s\n", (estado.ortho) ? "ORTHO" : "PERSPECTIVE");

  if (estado.ortho)
  {
    if (width < height)
      glOrtho(-20, 20, -20 * (GLdouble)height / width, 20 * (GLdouble)height / width, -100, 100);
    else
      glOrtho(-20 * (GLdouble)width / height, 20 * (GLdouble)width / height, -20, 20, -100, 100);
  }
  else
    gluPerspective(estado.camera.fov, (GLfloat)width / height, 1, 100);

  // Matriz Modelview
  // Matriz onde são realizadas as tranformações dos modelos desenhados
  glMatrixMode(GL_MODELVIEW);
}

/**************************************
** ESPAÇO PARA DEFINIÇÃO DAS ROTINAS **
****** AUXILIARES DE DESENHO ... ******
**************************************/

void desenhaPoligono(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat d[], GLfloat cor[])
{

  glBegin(GL_POLYGON);
  glColor3fv(cor);
  glVertex3fv(a);
  glVertex3fv(b);
  glVertex3fv(c);
  glVertex3fv(d);
  glEnd();
}

void strokeString(char *str, double x, double y, double z, double s)
{
  int i, n;

  n = strlen(str);
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

  n = strlen(str);
  glRasterPos2d(x, y);
  for (i = 0; i < n; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
}

void bitmapCenterString(char *str, double x, double y)
{
  int i, n;

  n = strlen(str);
  glRasterPos2d(x - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)str) * 0.5, y);
  for (i = 0; i < n; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
}

// ... Definição das rotinas auxiliares de desenho ...

void cubo(GLfloat vertices[][3], GLfloat cores[][3])
{

  desenhaPoligono(vertices[0], vertices[1], vertices[2], vertices[3], cores[0]);
  desenhaPoligono(vertices[0], vertices[3], vertices[4], vertices[7], cores[0]);
  desenhaPoligono(vertices[1], vertices[2], vertices[5], vertices[6], cores[0]);
  desenhaPoligono(vertices[4], vertices[5], vertices[6], vertices[7], cores[0]);
  desenhaPoligono(vertices[0], vertices[1], vertices[6], vertices[7], cores[0]);
  desenhaPoligono(vertices[2], vertices[3], vertices[4], vertices[5], cores[0]);
}

void torre(Tanque t)
{
  float c = COMPRIMENTO_TORRE / 2;
  float l = LARGURA_TORRE / 2;
  GLfloat vertices[][3] = {{t.x + l, t.y + c, ALTURA_BASE},
                           {t.x - l, t.y + c, ALTURA_BASE},
                           {t.x - l, t.y - c, ALTURA_BASE},
                           {t.x + l, t.y - c, ALTURA_BASE},
                           {t.x + l, t.y - c, ALTURA_BASE + ALTURA_TORRE},
                           {t.x - l, t.y - c, ALTURA_BASE + ALTURA_TORRE},
                           {t.x - l, t.y + c, ALTURA_BASE + ALTURA_TORRE},
                           {t.x + l, t.y + c, ALTURA_BASE + ALTURA_TORRE}};

  GLfloat cores[][3] = {{1.0, 0.0, 0.0}};

  cubo(vertices, cores);
}

void base(Tanque t)
{
  float c = COMPRIMENTO_BASE / 2;
  float l = LARGURA_BASE / 2;
  GLfloat vertices[][3] = {{t.x + l, t.y + c, 0},
                           {t.x - l, t.y + c, 0},
                           {t.x - l, t.y - c, 0},
                           {t.x + l, t.y - c, 0},
                           {t.x + l, t.y - c, ALTURA_BASE},
                           {t.x - l, t.y - c, ALTURA_BASE},
                           {t.x - l, t.y + c, ALTURA_BASE},
                           {t.x + l, t.y + c, ALTURA_BASE}};

  GLfloat cores[][3] = {{0.0, 1.0, 0.0}};
  cubo(vertices, cores);
}

void canhao(Tanque t)
{
  float c = t.y + COMPRIMENTO_TORRE / 2;
  float c1 = COMPRIMENTO_CANHAO;
  float r = RAIO_CANHAO;
  GLfloat vertices[][3] = {{t.x + r, c + c1, ALTURA_BASE},
                           {t.x - r, c + c1, ALTURA_BASE},
                           {t.x - r, c, ALTURA_BASE},
                           {t.x + r, c, ALTURA_BASE},
                           {t.x + r, c, ALTURA_BASE + (r * 2)},
                           {t.x - r, c, ALTURA_BASE + (r * 2)},
                           {t.x - r, c + c1, ALTURA_BASE + (r * 2)},
                           {t.x + r, c + c1, ALTURA_BASE + (r * 2)}};

  GLfloat cores[][3] = {{0.0, 0.0, 1.0}};

  cubo(vertices, cores);
}

void desenhaTanque(Tanque t)
{

  // desenha base centrada na posição t.x, t.y, 0 - x,y,z
  glPushMatrix();
  glTranslatef(t.x, t.y, 0);
  glRotatef(modelo.tanque.direccao, 0.0f, 0.0f, 1.0f);
  glTranslatef(-t.x, -t.y, 0);
  base(t);

  glTranslatef(t.x, t.y, 0);
  glRotatef(t.angTorre, 0.0f, 0.0f, 1.0f);
  glTranslatef(-t.x, -t.y, 0);
  torre(t);

  glTranslatef(t.x, (t.y + (COMPRIMENTO_TORRE / 2)), ALTURA_BASE);
  glRotatef(t.angCanhao, 1.0f, 0, 0);
  glTranslatef(-t.x, -(t.y + (COMPRIMENTO_TORRE / 2)), -ALTURA_BASE);
  canhao(t);
  glPopMatrix();
}

void desenhaChao(GLfloat dimensao)
{

  glBegin(GL_POLYGON);
  glVertex3f(dimensao, dimensao, 0);
  glVertex3f(-dimensao, dimensao, 0);
  glVertex3f(-dimensao, -dimensao, 0);
  glVertex3f(dimensao, -dimensao, 0);
  glEnd();
}

/* Callback de desenho */
void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  gluLookAt(estado.camera.eye.x, estado.camera.eye.y, estado.camera.eye.z,
            estado.camera.center.x, estado.camera.center.y, estado.camera.center.z,
            estado.camera.up.x, estado.camera.up.y, estado.camera.up.z);

  // ... Chamada das rotinas auxiliares de desenho ...

  glColor3f(0.5f, 0.5f, 0.5f);
  desenhaChao(RAIO_ROTACAO + 5);

  glPushMatrix();

  // ALINEA1.7, FALTA TRANSLATE
  // glRotatef(EIXOS, 0, 0, 1);

  desenhaTanque(modelo.tanque);

  glPopMatrix();

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

  // ALINEA 1.7
  // EIXOS = EIXOS + 1;
  printf("Direcao%.2f\nx:%.2f\ny%.2f\n", modelo.tanque.direccao, modelo.tanque.x, modelo.tanque.y);

  if (estado.menuActivo || modelo.parado) // Sair em caso de o jogo estar parado ou menu estar activo
    return;

  // redesenhar o ecra
  glutPostRedisplay();
}

/**************************************
*********** FUNÇÃO AJUDA **************
**************************************/

void imprime_ajuda(void)
{
  printf("\n\nDesenho de um quadrado\n");
  printf("h,H - Ajuda \n");
  printf("z,Z - Roda torre para a esquerda\n");
  printf("x,X - Roda torre para a direita\n");
  printf("q,Q - Levantar canhao\n");
  printf("a,A - Baixar canhao\n");
  printf("i,I - Reinicia modelo\n");
  printf("o,O - Alterna entre projecãoo Ortografica e Perspectiva\n");
  printf("f,F - Poligono Fill \n");
  printf("l,L - Poligono Line \n");
  printf("m,M - Poligono Point \n");
  printf("p,P - Inicia/para movimento\n");
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
    // ... Ações sobre outras teclas ...

  case 'h':
  case 'H':
    imprime_ajuda();
    break;
  case 'i':
  case 'I':
    inicia_modelo();
    break;
  case 'o':
  case 'O':
    estado.ortho = !estado.ortho;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    break;
  case 'R':
  case 'r':
    if (modelo.tanque.angCanhao >= 25)
      break;
    else
      modelo.tanque.angCanhao += 2;
    estado.teclas.q = GL_TRUE;
    break;
  case 'F':
  case 'f':
    if (modelo.tanque.angCanhao <= 0)
      break;
    else
      modelo.tanque.angCanhao -= 2;
    estado.teclas.a = GL_TRUE;
    break;
  case 'Q':
  case 'q':
    modelo.tanque.angTorre += 2;
    estado.teclas.z = GL_TRUE;
    break;
  case 'E':
  case 'e':
    modelo.tanque.angTorre -= 2;
    estado.teclas.x = GL_TRUE;
    break;
  case 'X':
  case 'x':
    estado.debug = !estado.debug;
    if (estado.menuActivo || modelo.parado)
      glutPostRedisplay();
    printf("DEBUG is %s\n", (estado.debug) ? "ON" : "OFF");
    break;
  case 'Z':
  case 'z':
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case 'm':
  case 'M':
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    break;
  case 'l':
  case 'L':
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  case 'A':
  case 'a':
    modelo.tanque.direccao += 2;
    break;
  case 'D':
  case 'd':
    modelo.tanque.direccao -= 2;
    break;
  case 'w':
  case 'W':
    if (modelo.tanque.direccao > 0 && modelo.tanque.direccao < 90)
    {
      modelo.tanque.y -= sin(modelo.tanque.direccao) * 2;
      modelo.tanque.x -= cos(modelo.tanque.direccao) * 4;
    }else
    break;
  case 'S':
  case 's':
    if (modelo.tanque.direccao > 0)
    {
      modelo.tanque.y -= 2;
      modelo.tanque.x += 2;
    }
    if (modelo.tanque.direccao < 0)
    {
      modelo.tanque.y -= 2;
      modelo.tanque.x -= 2;
    }

    break;

  case 'p':
  case 'P':
    modelo.parado = !modelo.parado;
    break;
  }

  if (DEBUG)
    printf("Carregou na tecla %c\n", key);
}

/* Callback para interação via teclado (largar a tecla) */
void keyUp(unsigned char key, int x, int y)
{
  switch (key)
  {
    // ... Ações sobre largar teclas ...
  }

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
************ FUNÇÃO MAIN **************
**************************************/

int main(int argc, char **argv)
{
  estado.doubleBuffer = GL_TRUE;

  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(((estado.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB | GLUT_DEPTH);
  if (glutCreateWindow("Tanque") == GL_FALSE)
    exit(1);

  init();

  imprime_ajuda();

  /* Registar callbacks do GLUT */

  /* callbacks de janelas/desenho */
  glutReshapeFunc(reshape);
  glutDisplayFunc(draw);

  /* Callbacks de teclado */
  glutKeyboardFunc(key);
  glutKeyboardUpFunc(keyUp);
  glutSpecialFunc(specialKey);
  glutSpecialUpFunc(specialKeyUp);

  /* Callbacks timer/idle */
  glutTimerFunc(estado.delayMovimento, timer, 0);
  // glutIdleFunc(idle);

  /* Começar loop */
  glutMainLoop();

  return 0;
}
