//
//  DiagramaVoronoi.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 23/08/23.
//  Copyright © 2023 Márcio Sarroglia Pinho. All rights reserved.
//

#include "DiagramaVoronoi.h"

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
    for (int i = 1; i < qtdDePoligonos; i++)
    {
        Diagrama[i] = LeUmPoligono();
        Diagrama[i].obtemLimites(A, B); // obtem o envelope do poligono

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

//------------------------------------------------------------------------------------
// calcula a intersecao de 3 pontos com produto vetorial
// int Voronoi::orientacao(Ponto p, Ponto q, Ponto r)
// {
//     double valor = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

//     if (valor == 0.0)   // se os pontos são colineares
//         return 0;
//     return (valor > 0.0) ? -1 : 1; // 1 se for sentido horário e -1 se for antihorário
// }

// bool Voronoi::intersecaoArestas(Ponto p1, Ponto p2, Ponto q1, Ponto q2)
// {
//     // verifica se os pontos de inicio ou fim são iguais
//     if (p1 == q1 || p1 == q2 || p2 == q1 || p2 == q2)
//     {
//         return false;
//     }

//     // produto vetorial para orientação dos pontos
//     int orientacao1 = orientacao(p1, p2, q1);
//     int orientacao2 = orientacao(p1, p2, q2);
//     int orientacao3 = orientacao(q1, q2, p1);
//     int orientacao4 = orientacao(q1, q2, p2);

//     // se orientações diferentes então as arestas cruzam
//     if (orientacao1 != orientacao2 || orientacao3 != orientacao4)
//     {
//         return true;
//     }
//     // não cruzam (podem ser paralelas)
//     return false;
// }

void Voronoi::obtemVizinhosDasArestas()
{
    for (int i = 0; i < qtdDePoligonos; i++) // percorre todos os poligonos no diagrama
    {
        Poligono &polignoAtual = Diagrama[i];
        for (int j = 0; j < polignoAtual.getNVertices(); j++) // percorre todas as arestas do poligono atual
        {
            Ponto p1, p2;
            polignoAtual.getAresta(j, p1, p2);

            vector<int> vizinhos; // inicializa lista de vizinhos para a aresta

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


                            vizinhos.push_back(k); // se intersectam vai adicionar o indice do poligono em vizinhos

                            // Adiciona poligonos vizinhos das arestas
                            // polignoAtual.adicionarVizinho(j, &outroPoligono);
                            // outroPoligono.adicionarVizinho(l, &polignoAtual);

                            break; // não precisa verificar os demais :)
                        }
                    }
                }
            }
            cout << "Aresta " << j << " do poligono " << i << " tem vizinhos: ";

            for (int i = 0; i < vizinhos.size(); i++)
            {
                cout << vizinhos[i] << " ";
            }
            cout << endl;
        }
    }
}
