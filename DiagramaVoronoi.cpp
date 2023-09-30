//
//  DiagramaVoronoi.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 23/08/23.
//  Copyright © 2023 Márcio Sarroglia Pinho. All rights reserved.
//

#include "DiagramaVoronoi.h"

#include <set>

ifstream input; // ofstream arq;

Voronoi::Voronoi()
{
}
Poligono Voronoi::LeUmPoligono()
{
    Poligono P;
    unsigned int qtdVertices;
    input >> qtdVertices; // arq << qtdVertices
    for (int i = 0; i < qtdVertices; i++)
    {
        double x, y;
        // Le um ponto
        input >> x >> y;
        Ponto(x, y).imprime();
        if (!input)
        {
            cout << "Fim inesperado da linha." << endl;
            break;
        }
        P.insereVertice(Ponto(x, y));
    }
    cout << "Poligono lido com sucesso!" << endl;
    return P;
}

void Voronoi::LePoligonos(const char *nome)
{
    input.open(nome, ios::in); // arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    string S;

    input >> qtdDePoligonos;
    cout << "qtdDePoligonos:" << qtdDePoligonos << endl;
    Ponto A, B;
    Diagrama[0] = LeUmPoligono();
    Diagrama[0].obtemLimites(Min, Max); // obtem o envelope do poligono
    Diagrama[0].setId(0);
    for (int i = 1; i < qtdDePoligonos; i++)
    {
        Diagrama[i] = LeUmPoligono();
        Diagrama[i].obtemLimites(A, B); // obtem o envelope do poligono
        Diagrama[i].setId(i);

        Min = ObtemMinimo(A, Min);
        Max = ObtemMaximo(B, Max);
    }
    cout << "Lista de Poligonos lida com sucesso!" << endl;
}

Poligono Voronoi::getPoligono(int i)
{
    if (i >= qtdDePoligonos)
    {
        cout << "Nro de Poligono Inexistente" << endl;
        return Diagrama[0];
        Poligono *encontraPoligonoQueContemPonto(Ponto & ponto);
    }
    return Diagrama[i];
}

unsigned int Voronoi::getNPoligonos()
{
    return qtdDePoligonos;
}

void Voronoi::obtemLimites(Ponto &min, Ponto &max)
{
    min = this->Min;
    max = this->Max;
}

// **********************************************************************
// Métodos para gerenciar Envelopes
// **********************************************************************
void Voronoi::geraEnvelopesPoligonos()
{
    for (int i = 0; i < getNPoligonos(); i++)
    {
        Diagrama[i].armazenaEnvelope();
    }
}

void Voronoi::imprimePontosEnvelopes()
{
    for (int i = 0; i < getNPoligonos(); i++)
    {
        Diagrama[i].getEnvelope().imprime();
    }
}

void Voronoi::desenhaEnvelopesPoligonos()
{
    for (int i = 0; i < getNPoligonos(); i++)
    {
        Diagrama[i].getEnvelope().Desenha();
    }
}
void Voronoi::desenhaEnvelopesInterseccao()
{
    for (int i = 0; i < PoligonosInterseccao.size(); i++)
    {
        PoligonosInterseccao[i].getEnvelope().Desenha();
    }
}
void Voronoi::desenhaEnvelopesComPonto()
{
    for (int i = 0; i < EnvelopesComPonto.size(); i++)
    {
        EnvelopesComPonto[i].Desenha();
    }
    
}
// **********************************************************************
void Voronoi::desenhaArestaSaida()
{
    glColor3f(1, 0, 0);
    poligonoAnterior.pintaAresta(arestaSaida);
}

void Voronoi::inserePoligonoInterseccao(Poligono &poligono)
{
    PoligonosInterseccao.push_back(poligono);
}

void Voronoi::desenhaPoligonosInterseccao(){
    glColor3f(0.9f, 0.9f, 0.9f); // Tom de cinza quase branco
    for (int i = 0; i < PoligonosInterseccao.size(); i++)
    {
        PoligonosInterseccao[i].pintaPoligono();
    }
}

Poligono Voronoi::inclusaoPoligonosConcavos(Ponto &ponto)
{
    Ponto Pinicial, Pfinal;
    Poligono result;
    int contadorInterseccoes = 0;
    PoligonosInterseccao.clear();

    for (int i = 0; i < getNPoligonos(); i++)
    {
        Poligono &poligonoAtual = Diagrama[i];
        Envelope envelopeAtual = poligonoAtual.getEnvelope();

        // Pontos da reta horizontal
        Pinicial = Ponto(Min.x, ponto.y);
        Pfinal = Ponto(Max.x, ponto.y);

        // Gera Pontos do direto do envelope
        Ponto envDireitoMin = Ponto(envelopeAtual.Max.x, envelopeAtual.Min.y);
        Ponto envDireitoMax = envelopeAtual.Max;

        if (HaInterseccao(envDireitoMin, envDireitoMax, Pinicial, Pfinal))
        {
            contadorInterseccoes += 1;
            inserePoligonoInterseccao(poligonoAtual);

            cout << "     - Tem interseccção com o poligono (" << poligonoAtual.getId() << ")"<< endl;
            if(poligonoAtual.pontoEstaDentroPoligono(ponto))
                result = poligonoAtual;
        }
    }
        cout << "     *Total de Intersecções com polígonos: " << contadorInterseccoes << endl;

    return result;
}


Poligono Voronoi::inclusaoPoligonosConvexos(Ponto &ponto)
{
    EnvelopesComPonto.clear();
    Poligono result;
    for (int i = 0; i < getNPoligonos(); i++)
    {
        Poligono poligonoAtual = Diagrama[i];
        Envelope envelopeAtual = poligonoAtual.getEnvelope();
        if (envelopeAtual.pontoEstaDentro(ponto))
        {
            EnvelopesComPonto.push_back(envelopeAtual);
            if (poligonoAtual.pontoEstaDentroPoligono(ponto))
            {
                result = poligonoAtual;
            }
        }
    }
    return result;
}

Poligono Voronoi::inclusaoPoligonosConvexosViz(Ponto &ponto, Poligono &poligonoAtual)
{
    Poligono result;
    Ponto P1, P2;
    int nVertices = poligonoAtual.getNVertices();

    for (int i = 0; i < nVertices; i++)
    {
        poligonoAtual.getAresta(i, P1, P2);
        Ponto vetorPonto = ponto - P1;
        Ponto vetorAresta = P2 - P1;

        Ponto produtoVetorial;
        ProdVetorial(vetorPonto, vetorAresta, produtoVetorial);

        // Se o ponto esriver à direita da aresta, está fora e passou pela aresta atual
        if (produtoVetorial.z < 0)
        {
            int indiceNovoPoligono = poligonoAtual.getVizinhoAresta(i);
            if (indiceNovoPoligono != -1)
            {
                cout << "    - O Ponto passou pela aresta: " << i << endl;

                poligonoAnterior = poligonoAtual;
                arestaSaida = i;

                result = getPoligono(indiceNovoPoligono);
            } else
            {
                cout << "    - Vizinho não encontado na aresta: " << i << endl;
            }
            break;
        }
    }

    return result;
}

void Voronoi::obtemVizinhosDasArestas()
{
    for (int i = 0; i < qtdDePoligonos; i++) // percorre todos os poligonos no diagrama
    {
        Poligono &polignoAtual = Diagrama[i];
        for (int j = 0; j < polignoAtual.getNVertices(); j++) // percorre todas as arestas do poligono atual
        {
            Ponto p1, p2;
            polignoAtual.getAresta(j, p1, p2);

            for (int k = 0; k < qtdDePoligonos; k++) // percorre os demais poligonos
            {
                if (i != k)
                {
                    Poligono &outroPoligono = Diagrama[k];

                    for (int l = 0; l < outroPoligono.getNVertices(); l++) // percorre as arestas do outro poligono
                    {
                        Ponto q1, q2;
                        outroPoligono.getAresta(l, q1, q2);

                        if ((p1 == q1 && p2 == q2) || (p2 == q1 && p1 == q2)) // verifica a intersecao
                        {
                            // Adiciona poligono vizinho da aresta j
                            polignoAtual.adicionarVizinho(j, k);

                            break; // não precisa verificar os demais :)
                        }
                    }
                }
            }

            // cout << "Aresta " << j << " do poligono " << i << " tem vizinhos: ";

            // if (polignoAtual.getVizinhoAresta(j) != nullptr)
            //     cout << "sim"
            //          << " ";
            // cout << endl;
        }
    }
}
