//
//  Poligono.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 18/08/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//
#include <iostream>
#include <fstream>
using namespace std;

#include "Poligono.h"
#include "Envelope.h"

Poligono::Poligono()
{
}

// **********************************************************************
// Métodos que gerenciam vizinhos de arestas.
// **********************************************************************

// Adiciona novo vizinho a uma aresta
void Poligono::adicionarVizinho(int indiceAresta, Poligono* vizinho)
{
    vizinhosArestas[indiceAresta] = vizinho;
}

// Retorna poligono vizinho pelo indice da aresta
Poligono* Poligono::getVizinhoAresta(int indiceAresta)
{
    if(vizinhosArestas.find(indiceAresta) != vizinhosArestas.end())
         return vizinhosArestas[indiceAresta];
    return nullptr;
}

//***********************************************************************

// **********************************************************************
// Métodos que gerenciam o Envelope do poligono
// **********************************************************************

// Armazena Envelope do poligono
void Poligono::armazenaEnvelope()
{
    Ponto P1, P2;

    obtemLimites(P1, P2);
    envelope.GeraEnvelope(P1, P2);
}

// Retorna envelope do poligono atual
Envelope Poligono::getEnvelope()
{
    return envelope;
}
// **********************************************************************

void Poligono::insereVertice(Ponto p)
{
    Vertices.push_back(p);
}

void Poligono::insereVertice(Ponto p, int pos)
{
    if ((pos < 0) || (pos > Vertices.size()))
    {
        cout << "Metodo " << __FUNCTION__ << ". Posicao Invalida. Vertice nao inserido." << endl;
        return;
    }
    Vertices.insert(Vertices.begin() + pos, p);
}

Ponto Poligono::getVertice(int i)
{
    return Vertices[i];
}

void Poligono::pintaPoligono()
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < Vertices.size(); i++)
        glVertex3f(Vertices[i].x, Vertices[i].y, Vertices[i].z);
    glEnd();
}

void Poligono::desenhaPoligono()
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < Vertices.size(); i++)
        glVertex3f(Vertices[i].x, Vertices[i].y, Vertices[i].z);
    glEnd();
}
void Poligono::desenhaVertices()
{
    glBegin(GL_POINTS);
    for (int i = 0; i < Vertices.size(); i++)
        glVertex3f(Vertices[i].x, Vertices[i].y, Vertices[i].z);
    glEnd();
}
void Poligono::imprime()
{
    for (int i = 0; i < Vertices.size(); i++)
        Vertices[i].imprime();
}
unsigned long Poligono::getNVertices()
{
    return Vertices.size();
}

void Poligono::obtemLimites(Ponto &Min, Ponto &Max)
{
    Max = Min = Vertices[0];

    for (int i = 0; i < Vertices.size(); i++)
    {
        Min = ObtemMinimo(Vertices[i], Min);
        Max = ObtemMaximo(Vertices[i], Max);
    }
}

// **********************************************************************
//
// **********************************************************************
void Poligono::LePoligono(const char *nome)
{
    ifstream input;            // ofstream arq;
    input.open(nome, ios::in); // arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << nome << "...";
    string S;
    // int nLinha = 0;
    unsigned int qtdVertices;

    input >> qtdVertices; // arq << qtdVertices

    for (int i = 0; i < qtdVertices; i++)
    {
        double x, y;
        // Le cada elemento da linha
        input >> x >> y; // arq << x  << " " << y << endl
        if (!input)
            break;
        // nLinha++;
        insereVertice(Ponto(x, y));
    }
    cout << "Poligono lido com sucesso!" << endl;
    input.close();
}

void Poligono::getAresta(int n, Ponto &P1, Ponto &P2)
{
    P1 = Vertices[n];
    int n1 = (n + 1) % Vertices.size();
    P2 = Vertices[n1];
}

void Poligono::desenhaAresta(int n)
{
    glBegin(GL_LINES);
    glVertex3f(Vertices[n].x, Vertices[n].y, Vertices[n].z);
    int n1 = (n + 1) % Vertices.size();
    glVertex3f(Vertices[n1].x, Vertices[n1].y, Vertices[n1].z);
    glEnd();
}

// **********************************************************************
// Verifica se um determinado ponto está dentro deste polígono
// **********************************************************************
bool Poligono::pontoEstaDentroPoligono(Ponto &ponto)
{
    int size = getNVertices();
    Ponto P1, P2;

    // Para cada vértice do polígono
    for (int i = 0; i < size; i++)
    {
        getAresta(i, P1, P2);
        Ponto vetorPonto = ponto - P1;
        Ponto vetorAresta = P2 - P1;

        Ponto produtoVetorial;
        ProdVetorial(vetorPonto, vetorAresta, produtoVetorial);

        // Se o ponto esriver à direita da aresta, está fora
        if (produtoVetorial.z < 0)
        {
            return false;
        }
    }
    // O ponto está à esquerda de todas as arestas
    return true;
}
