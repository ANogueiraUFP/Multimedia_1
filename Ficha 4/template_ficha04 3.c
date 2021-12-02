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

#define DEBUG 1
#define LADO_MAXIMO     2
#define LADO_MINIMO     0.3
#define DELTA_LADO      0.1

/**************************************
********** VARIÁVEIS GLOBAIS **********
**************************************/

typedef struct {
  GLboolean   doubleBuffer;
  GLint       delayMovimento;
  GLboolean   debug;
  GLboolean   movimentoTranslacao;      // se os cubinhos se movem
  GLboolean   movimentoRotacao;         // se o cubo grande roda;
}Estado;


typedef struct {
  GLfloat   theta[3];     // 0-Rotação em X; 1-Rotação em Y; 2-Rotação em Z
  
  GLint     eixoRodar;    // eixo que está a rodar (mudar com o rato)
  GLfloat   ladoCubo;     // comprimento do lado do cubo
  GLfloat   deltaRotacao; // incremento a fazer ao angulo quando roda
  GLboolean sentidoTranslacao; //sentido da translação dos cubos pequenos
  GLfloat   translacaoCubo; //
  GLfloat   deltaTranslacao; // incremento a fazer na translacao
  GLboolean sentidoRotacao;  //sentido da rotação dos cubos pequenos
  GLfloat   thetaCubo;     // Rotação dos cubinhos
}Modelo;

Estado estado;
Modelo modelo;

/**************************************
*** INICIALIZAÇÃO DO AMBIENTE OPENGL **
**************************************/

void inicia_modelo()
{
  estado.delayMovimento=50;
  estado.movimentoTranslacao=GL_FALSE;
  estado.movimentoRotacao=GL_FALSE;

  modelo.theta[0]=0;
  modelo.theta[1]=0;
  modelo.theta[2]=0;
  modelo.eixoRodar=0;  // eixo de X;
  modelo.ladoCubo=1;
}

void init(void)
{
  inicia_modelo();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  //glEnable(GL_DEPTH_TEST);

}

/**************************************
***** CALL BACKS DE JANELA/DESENHO ****
**************************************/

/* Callback para redimensionar janela */
void reshape(int width, int height)
{
  glViewport(0, 0, (GLint) width, (GLint) height);  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (width < height)
     glOrtho(-5, 5, -5*(GLdouble)height/width, 5*(GLdouble)height/width,-10,10);
  else
     glOrtho(-5*(GLdouble)width/height, 5*(GLdouble)width/height,-5, 5, -10,10);

   
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/**************************************
** ESPAÇO PARA DEFINIÇÃO DAS ROTINAS **
****** AUXILIARES DE DESENHO ... ******
**************************************/

void desenhaPoligono(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat  d[], GLfloat cor[])
{
  /* draw a polygon via list of vertices */

  glBegin(GL_POLYGON);
    glColor3fv(cor);
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
    glVertex3fv(d);
  glEnd();
}

void cubo()
{
 GLfloat vertices[][3] = {{-0.5,-0.5,-0.5}, 
                          {0.5,-0.5,-0.5}, 
                          {0.5,0.5,-0.5}, 
                          {-0.5,0.5,-0.5}};

  GLfloat cores[][3] = {{0.0,1.0,1.0},
                        {1.0,0.0,0.0},
                        {1.0,1.0,0.0}, 
                        {0.0,1.0,0.0}, 
                        {1.0,0.0,1.0}, 
                        {0.0,0.0,1.0}, 
                        {1.0,1.0,1.0}};

  desenhaPoligono(vertices[1],vertices[0],vertices[3],vertices[2],cores[0]);
}

// ... Definição das rotinas auxiliares de desenho ...

/* Callback de desenho */
void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT);

  // ... chamada das rotinas auxiliares de desenho ...

  glPushMatrix();
    glRotatef(modelo.theta[0],1,0,0);
    glRotatef(modelo.theta[1],0,1,0);
    glRotatef(modelo.theta[2],0,0,1);
    glScalef(modelo.ladoCubo,modelo.ladoCubo,modelo.ladoCubo);
    cubo();
  glPopMatrix();


  if (estado.doubleBuffer)
    glutSwapBuffers();
  else
    glFlush();
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
  /* ... accoes do temporizador não colocar aqui transformações, alterar
     somente os valores das variáveis ... */

  // alterar o modelo.theta[] usando a variável modelo.eixoRodar como indice

  
  /* redesenhar o ecrã */
  glutPostRedisplay();
}


/**************************************
*********** FUNÇÃO AJUDA **************
**************************************/

void imprime_ajuda(void)
{
  printf("\n\nDesenho de um quadrado\n");
  printf("h,H - Ajuda \n");
  printf("F1  - Reiniciar \n");
  printf("F2  - Poligono Fill \n");
  printf("F3  - Poligono Line \n");
  printf("F4  - Poligono Point \n");
  printf("+   - Aumentar tamanho dos Cubos\n");
  printf("-   - Diminuir tamanho dos Cubos\n");
  printf("i,I - Reiniciar Variáveis\n");
  printf("p,p - Iniciar/Parar movimento dos cubinhos\n");
  printf("ESC - Sair\n");
  printf("teclas do rato para iniciar/parar rotação e alternar eixos\n");

}

/**************************************
********* CALLBACKS TECLADO ***********
**************************************/

/* Callback para interação via teclado (carregar na tecla) */
void key(unsigned char key, int x, int y)
{
  switch (key) {
    case 27:
      exit(1);
    /* ... accoes sobre outras teclas ... */

    case 'h' :
    case 'H' :
                imprime_ajuda();
            break;
    case '+':
            if(modelo.ladoCubo<LADO_MAXIMO)
            {
              modelo.ladoCubo+=DELTA_LADO;
              glutPostRedisplay();
            }
          break;

    case '-':
            if(modelo.ladoCubo>LADO_MINIMO)
            {
              modelo.ladoCubo-=DELTA_LADO;
              glutPostRedisplay();
            }
          break;

    case 'i' :
    case 'I' :
            inicia_modelo();
            glutPostRedisplay();
          break;
    case 'p' :
    case 'P' :
            estado.movimentoTranslacao=!estado.movimentoTranslacao;
          break;

  }

  if(DEBUG)
    printf("Carregou na tecla %c\n",key);
}

/* Callback para interação via teclado (largar a tecla) */
void keyUp(unsigned char key, int x, int y)
{
  if(DEBUG)
    printf("Largou a tecla %c\n",key);
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
    case GLUT_KEY_F1 :
            inicia_modelo();
            glutPostRedisplay();
        break;
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
*********** CALLBACKS RATO ************
**************************************/

void mouseMotion(int x, int y)
{
    /* x,y    => coordenadas do ponteiro quando se move no rato
                 a carregar em teclas
    */

  if(DEBUG)
    printf("Mouse Motion %d %d\n",x,y);

}

void mousePassiveMotion(int x, int y)
{
    /* x,y    => coordenadas do ponteiro quando se move no rato
                 sem estar a carregar em teclas
    */

  if(DEBUG)
    printf("Mouse Passive Motion %d %d\n",x,y);

}

void mouse(int button, int state, int x, int y)
{
  /* button => GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
     state  => GLUT_UP, GLUT_DOWN
     x,y    => coordenadas do ponteiro quando se carrega numa tecla do rato
  */

  // alterar o eixo que roda (variável modelo.eixoRodar)

  switch(button){
    case GLUT_LEFT_BUTTON :      
      if(state == GLUT_DOWN)
      {        
      }
     break;
    case GLUT_MIDDLE_BUTTON :      
      if(state == GLUT_DOWN)
      {        
      }
      break;
    case GLUT_RIGHT_BUTTON :
      if(state == GLUT_DOWN)
      {        
      }
      break;
  }
  if(DEBUG)
    printf("Mouse button:%d state:%d coord:%d %d\n",button,state,x,y);
}

/**************************************
************ FUNÇÃO MAIN **************
**************************************/

int main(int argc, char **argv)
{
  estado.doubleBuffer = GL_TRUE;

  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(400,400);
  glutInitDisplayMode(((estado.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB);
  if (glutCreateWindow("Cubo") == GL_FALSE)
    exit(1);

  init();

  imprime_ajuda();

  /* Registar callbacks do GLUT */

  // Callbacks de janelas/desenho
  glutReshapeFunc(reshape);
  glutDisplayFunc(draw);

  // Callbacks de teclado
  glutKeyboardFunc(key);
  //glutKeyboardUpFunc(keyUp);
  glutSpecialFunc(specialKey);
  //glutSpecialUpFunc(SpecialKeyUp);

  // Callbacks do rato
  //glutPassiveMotionFunc(MousePassiveMotion);
  //glutMotionFunc(MouseMotion);
  glutMouseFunc(mouse);

  // Callbacks timer/idle
  glutTimerFunc(estado.delayMovimento, timer, 0);
  //glutIdleFunc(Idle);

  // Começar
  glutMainLoop();

  return 0;
}
