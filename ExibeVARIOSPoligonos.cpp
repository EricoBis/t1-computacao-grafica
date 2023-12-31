// **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <stdlib.h>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"
#include "DiagramaVoronoi.h"

#include "ListaDeCoresRGB.h"

#include "Temporizador.h"

Temporizador T;
double AccumDeltaT = 0;

Poligono Pontos;

Voronoi Voro;
int *CoresDosPoligonos;

// Limites logicos da area de desenho
Ponto Min, Max, MinVoro, MaxVoro, PontoClicado;

bool desenha = false;
bool FoiClicado = false;

float angulo = 0.0;

// Variaveis relacionadas aos testes de poligonos ---
char nomeArqPoligonos[] = "40_polygons.txt";

bool diagramaMonoCromatico = false; // m
bool mostrarEnvelopes = false; // e
bool linhaHorizVerde = false; // v

bool mostrarEnvelopesInterseccao = false; // r
bool mostrarPoligonosInterseccao = false; // f

bool mostrarEnvelopesComPonto = false; // c

int idPoligono;
Poligono poligonoAtual;

int poligonosPercorridos = 0;

bool pontoMoveu = false;
double distanciaMovimento = 0.1;
Ponto pontoPrincipal;           // Ponto principal que se moverá pela tela
Poligono desenhoPontoPrincipal; // Poligono relativo ao pontoPrincipal (demonstra graficamente o ponto)
// --------------------------------------------------

// **********************************************************************
//
// **********************************************************************
void executaTestesProximoPoligono()
{
    if (poligonoAtual.pontoEstaDentroPoligono(pontoPrincipal))
    {
        cout << ".";
        cout << endl;
        return;
    }

    cout << "- O Ponto saiu do polígono (" << idPoligono << ") --->" << endl;
    cout << endl;
    cout << "** Executando Testes **" << endl;

    cout << "* Teste de Inclusão em Poligonos Concavos..." << endl;
    cout << "    - A linha Horizontal tem intersecção com:" << endl;
    Poligono aux1 = Voro.inclusaoPoligonosConcavos(pontoPrincipal);
    cout << "     *Novo poligono atual determinado pelo teste: " << aux1.getId() << endl;
    cout << endl;

    cout << "* Teste de Inclusão em Poligonos Convexos..." << endl;
    Poligono aux2 = Voro.inclusaoPoligonosConvexos(pontoPrincipal);
    cout << "     *Novo poligono atual determinado pelo teste: " << aux2.getId() << endl;
    cout << endl;

    cout << "* Teste de Inclusão em Poligonos Convexos com Informação de Vizinhança..." << endl;
    Poligono aux3 = Voro.inclusaoPoligonosConvexosViz(pontoPrincipal, poligonoAtual);
    cout << "     *Novo poligono atual determinado pelo teste: " << aux3.getId() << endl;
    cout << endl;

    poligonoAtual = aux3;
    idPoligono = poligonoAtual.getId();

    cout << endl;
    cout << "---> O Ponto entrou no polígono (" << idPoligono << ") -" << endl;

    poligonosPercorridos += 1;
}

void imprimeEstatisticas()
{
    cout << "**** Estatísticas ****" << endl;
    cout << "- Total de Poligonos percorridos: " << poligonosPercorridos << endl;
    cout << "- Vezes que a função HaInterseccao foi chamada: " << getContadorInt() << endl;
    cout << "- Vezes que a função ProdVetorial foi chamada: " << getContadorProdVet() << endl;
    cout << "id atual ----------> " << idPoligono << endl;
}

// **********************************************************************
//
// **********************************************************************
void printString(string s, int posX, int posY)
{
    // defineCor(cor);
    glColor3f(1, 1, 1);
    glRasterPos3i(posX, posY, 0); // define posicao na tela
    for (int i = 0; i < s.length(); i++)
    {
        // GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_TIMES_ROMAN_24,GLUT_BITMAP_HELVETICA_18
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    }
}
// **********************************************************************
//
// **********************************************************************
void ImprimeNumeracaoDosVertices(Poligono &P)
{
    for (int i = 0; i < P.getNVertices(); i++)
    {
        Ponto aux;
        aux = P.getVertice(i);
        char msg[10];
        sprintf(msg, "%d", i);
        printString(msg, aux.x, aux.y);
    }
}

// **********************************************************************
//
// **********************************************************************
void GeraPontos(int qtd)
{
    time_t t;
    Ponto Escala;
    Escala = (Max - Min) * (1.0 / 1000.0);
    srand((unsigned)time(&t));
    for (int i = 0; i < qtd; i++)
    {
        float x = rand() % 1000;
        float y = rand() % 1000;
        x = x * Escala.x + Min.x;
        y = y * Escala.y + Min.y;
        Pontos.insereVertice(Ponto(x, y));
    }
}

// **********************************************************************
//
// **********************************************************************
void init()
{
    srand(0);
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 0.3f, 0.3f);

    Voro.LePoligonos(nomeArqPoligonos); // le o arquivo com os poligonos
    Voro.obtemLimites(Min, Max);        // calcula os limites, para ajustar a janela
    Voro.obtemVizinhosDasArestas();     // calcula os vizinhos
    Voro.geraEnvelopesPoligonos();      // gera envelopes para cada poligono
    Voro.imprimePontosEnvelopes();

    Min.imprime("Minimo:", "\n");
    Max.imprime("Maximo:", "\n");

    CoresDosPoligonos = new int[Voro.getNPoligonos()];

    for (int i = 0; i < Voro.getNPoligonos(); i++) // sorteia as cores dos poligonos
    {
        CoresDosPoligonos[i] = rand() % 80;
    }

    // Guarda o Min e Max do diagrama para uso posterior
    MinVoro = Min;
    MaxVoro = Max;

    // Ajusta a largura da janela l�gica
    // em fun��o do tamanho dos pol�gonos
    Ponto Largura;
    Largura = Max - Min;

    Min = Min - Largura * 0.1;
    Max = Max + Largura * 0.1;

    //
    desenhoPontoPrincipal.LePoligono("PontoPrincipal.txt");
    pontoPrincipal = Ponto(0, 0);

    poligonoAtual = Voro.inclusaoPoligonosConvexos(pontoPrincipal); // Encontra o poligono onde o ponto está inicialmente
    idPoligono = poligonoAtual.getId();
    //
}

double nFrames = 0;
double TempoTotal = 0;
// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0 / 30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        if (pontoMoveu)
        {
            executaTestesProximoPoligono();
            pontoMoveu = false;
        }

        glutPostRedisplay();
    }
}

// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
// **********************************************************************
void reshape(int w, int h)
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x, Max.x,
            Min.y, Max.y,
            0, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
//
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x + Min.x) / 2;
    Meio.y = (Max.y + Min.y) / 2;
    Meio.z = (Max.z + Min.z) / 2;

    glBegin(GL_LINES);
    //  eixo horizontal
    glVertex2f(Min.x, Meio.y);
    glVertex2f(Max.x, Meio.y);
    //  eixo vertical
    glVertex2f(Meio.x, Min.y);
    glVertex2f(Meio.x, Max.y);
    glEnd();
}
// **********************************************************************

// **********************************************************************
void DesenhaLinha(Ponto P1, Ponto P2)
{
    glBegin(GL_LINES);
    glVertex3f(P1.x, P1.y, P1.z);
    glVertex3f(P2.x, P2.y, P2.z);
    glEnd();
}
// **********************************************************************
void InterseptaArestas(Poligono P)
{
    /*
    Ponto P1, P2;
    for (int i=0; i < P.getNVertices();i++)
    {
        P.getAresta(i, P1, P2);
        //if(PassaPelaFaixa(i,F))
        if (HaInterseccao(PontoClicado,Esq, P1, P2))
            P.desenhaAresta(i);
    }*/
}
// **********************************************************************
//  void display( void )
// **********************************************************************
void display(void)
{
    // Limpa a tela coma cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da area OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // Coloque aqui as chamadas das rotinas que desenham os objetos
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    glLineWidth(1);
    glColor3f(1, 1, 1); // R, G, B  [0..1]
    DesenhaEixos();

    glRotatef(angulo, 0, 0, 1);
    glLineWidth(2);

    Poligono P;
    for (int i = 0; i < Voro.getNPoligonos(); i++) // desenha o diagrama
    {
        if (diagramaMonoCromatico)
            glColor3f(0, 0, 0);
        else
            defineCor(CoresDosPoligonos[i]);

        P = Voro.getPoligono(i);
        P.pintaPoligono();
    }

    if(mostrarPoligonosInterseccao)
        Voro.desenhaPoligonosInterseccao();
    glColor3f(1, 1, 1);

    poligonoAtual.pintaPoligono();

    if (diagramaMonoCromatico)
        glColor3f(1, 1, 1);
    else
        glColor3f(0, 0, 0);

    for (int i = 0; i < Voro.getNPoligonos(); i++) // desenha bordas dos poligonos
    {
        P = Voro.getPoligono(i);
        P.desenhaPoligono();
    }
    Voro.desenhaArestaSaida();

    //---
    if (diagramaMonoCromatico)
        glColor3f(0, 0, 1);
    else
        glColor3f(0, 0, 0);

    if (mostrarEnvelopes)
        Voro.desenhaEnvelopesPoligonos();
    if (mostrarEnvelopesInterseccao)
    {
        mostrarEnvelopes = false;
        Voro.desenhaEnvelopesInterseccao();
    }
    if(mostrarEnvelopesComPonto)
    {
        glColor3f(0, 1, 0);
        Voro.desenhaEnvelopesComPonto();
    }
    //---

    glPushMatrix();
    if(linhaHorizVerde)
        glColor3f(0, 9, 0); // R, G, B  [0..1]
    else
        glColor3f(0, 0, 0.6); // R, G, B  [0..1]
    Ponto EsqVoro, DirVoro;
    EsqVoro.x = MinVoro.x;
    EsqVoro.y = pontoPrincipal.y;
    DirVoro.x = MaxVoro.x;
    DirVoro.y = pontoPrincipal.y;
    DesenhaLinha(DirVoro, EsqVoro);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 0, 0);
    glTranslatef(pontoPrincipal.x, pontoPrincipal.y, pontoPrincipal.z);
    desenhoPontoPrincipal.desenhaPoligono();
    desenhoPontoPrincipal.pintaPoligono();
    glPopMatrix();

    if (desenha)
    {
        desenha = false;
    }

    // se o ponto for clicado desenha uma linha horizontal do ponto at� o limite da esquerda (mesmo y)
    if (FoiClicado == true)
    {
        Ponto Esq;
        Ponto Dir(-1, 0);
        Esq = PontoClicado + Dir * (1000);
        glColor3f(0, 1, 0); // R, G, B  [0..1]
        DesenhaLinha(PontoClicado, Esq);

        glColor3f(1, 0, 0); // R, G, B  [0..1]
    }

    glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n�mero de segundos e informa quanto frames
// se passaram neste per�odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while (true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:     // Termina o programa qdo
        exit(0); // a tecla ESC for pressionada
        break;
    case 't':
        ContaTempo(3);
        break;
    case ' ':
        desenha = !desenha;
        break;
    case 'p':
        imprimeEstatisticas();
        break;
    case 'e':
        mostrarEnvelopes = !mostrarEnvelopes;
        break;
    case 'f':
        mostrarPoligonosInterseccao = !mostrarPoligonosInterseccao;
        break;
    case 'r':
        mostrarEnvelopesInterseccao = !mostrarEnvelopesInterseccao;
        break;
    case 'm':
        diagramaMonoCromatico = !diagramaMonoCromatico;
        break;
    case 'v':
        linhaHorizVerde = !linhaHorizVerde;
        break;
    case 'c':
        mostrarEnvelopesComPonto = !mostrarEnvelopesComPonto;
        break;
    default:
        break;
    }
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
// **********************************************************************
void arrow_keys(int a_keys, int x, int y)
{
    Ponto novoPonto = pontoPrincipal;
    switch (a_keys)
    {
    case GLUT_KEY_UP: // Se pressionar UP
        novoPonto.soma(0, distanciaMovimento, 0);
        pontoMoveu = true;
        break;
    case GLUT_KEY_DOWN: // Se pressionar DOWN
        novoPonto.soma(0, -distanciaMovimento, 0);
        pontoMoveu = true;
        break;
    case GLUT_KEY_LEFT: // Se pressionar LEFT
        novoPonto.soma(-distanciaMovimento, 0, 0);
        pontoMoveu = true;
        break;
    case GLUT_KEY_RIGHT: // Se pressionar RIGHT
        novoPonto.soma(distanciaMovimento, 0, 0);
        pontoMoveu = true;
    default:
        break;
    }

    // Verifica se o novo ponto saiu dos limites
    if (novoPonto.x < MinVoro.x)
        novoPonto.x = MinVoro.x;
    else if (novoPonto.x > MaxVoro.x)
        novoPonto.x = MaxVoro.x;

    if (novoPonto.y < MinVoro.y)
        novoPonto.y = MinVoro.y;
    else if (novoPonto.y > MaxVoro.y)
        novoPonto.y = MaxVoro.y;

    // Atribui o novo ponto ao ponto principal
    pontoPrincipal = novoPonto;
}
// **********************************************************************
// Esta fun��o captura o clique do botao direito do mouse sobre a �rea de
// desenho e converte a coordenada para o sistema de refer�ncia definido
// na glOrtho (ver fun��o reshape)
// Este c�digo � baseado em http://hamala.se/forums/viewtopic.php?t=20
// **********************************************************************
void Mouse(int button, int state, int x, int y)
{
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat wx = x, wy, wz;
    GLdouble ox = 0.0, oy = 0.0, oz = 0.0;

    if (state != GLUT_DOWN)
        return;
    if (button != GLUT_LEFT_BUTTON)
        return;
    cout << "Botao da Esquerda! ";

    glGetIntegerv(GL_VIEWPORT, viewport);
    y = viewport[3] - y;
    wy = y;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(wx, wy, wz, modelview, projection, viewport, &ox, &oy, &oz);
    PontoClicado = Ponto(ox, oy, oz);
    PontoClicado.imprime("- Ponto no universo: ", "\n");
    FoiClicado = true;
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int main(int argc, char **argv)
{
    cout << "Programa OpenGL" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize(650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de titulo da janela.
    glutCreateWindow("Poligonos em OpenGL");

    // executa algumas inicializa��es
    init();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc(display);

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc(reshape);

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc(keyboard);

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc(arrow_keys);

    glutMouseFunc(Mouse);

    // inicia o tratamento dos eventos
    glutMainLoop();

    return 0;
}
