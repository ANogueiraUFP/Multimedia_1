#include "glm.h"
#include "math.h"
#include "time.h"
#include "stdlib.h"

#define VERTICES 10

PONTO ponto_medio(PONTO p1, PONTO p2)
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
}

PONTO *coordenadas(int n, PONTO pc, float raio)
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

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(1024, 1024);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Draw Circle");
    glutDisplayFunc(desenhar_circunferencia);
    glutMainLoop();
    return 0;
}