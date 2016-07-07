#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <stack>
#include <stdexcept>

namespace ED
{

    // direcoes
    enum Direcao {NW = 0, NE, SE, SW, FORA};

    // ponto bidimencional
    struct Ponto
    {
        float x{0};
        float y{0};

        Ponto () {}

        Ponto (const float x, const float y)
        {
            this->x = x;
            this->y = y;
        }

        bool operator==(const Ponto &outro)
        {
            return (this->x == outro.x) && (this->y == outro.y);
        }

        bool operator!=(const Ponto &outro)
        {
            return !((this->x == outro.x) && (this->y == outro.y));
        }
    };

    // regiao determinada pela posicao do centro
    // e o deslocamento horizontal e vertical
    struct Regiao
    {
        Ponto centro;
        float offsetV{0};
        float offsetH{0};

        Regiao () {}

        Regiao (const Ponto &centro, const float offsetV,
                const float offsetH)
        {
            this->centro = centro;
            this->offsetV = offsetV;
            this->offsetH = offsetH;
        }
    };

    // informacao guardade em um
    // determinado ponto
    template <class T>
    struct Info
    {
        Ponto chave;
        T valor;

        Info () {}

        Info (const Ponto &chave, const T &valor)
        {
            this->chave = chave;
            this->valor = valor;
        }
    };

    // no da arvore
    template <class T>
    struct No
    {
        int largura;
        Regiao quad;
        Info<T> *info{nullptr};
        unsigned int qtdPontos{0};
        std::vector<No*> filhos;

        No (const Regiao &quad)
        {
            this->quad = quad;
        }

        void expandir()
        {
            filhos.resize(4);

            filhos[NW] = new No<T>(Regiao(Ponto(quad.centro.x - quad.offsetH/2,
                                                quad.centro.y + quad.offsetV/2),
                                          quad.offsetV/2, quad.offsetH/2));
            filhos[NE] = new No<T>(Regiao(Ponto(quad.centro.x + quad.offsetH/2,
                                                 quad.centro.y + quad.offsetV/2),
                                          quad.offsetV/2, quad.offsetH/2));
            filhos[SE] = new No<T>(Regiao(Ponto(quad.centro.x + quad.offsetH/2,
                                                quad.centro.y - quad.offsetV/2),
                                           quad.offsetV/2, quad.offsetH/2));
            filhos[SW] = new No<T>(Regiao(Ponto(quad.centro.x - quad.offsetH/2,
                                                 quad.centro.y - quad.offsetV/2),
                                          quad.offsetV/2, quad.offsetH/2));
        }

        void comprimir()
        {
            if (int(filhos.size()) == 0)
                return;

            for (No<T>* filho : filhos)
                if (int(filho->filhos.size()) != 0)
                    return;


            if (qtdPontos < 2)
            {
                for (No<T>* filho : filhos)
                {
                    if (filho->info != nullptr)
                    {
                        info = filho->info;
                        filho->info = nullptr;
                    }

                    delete filho;
                }

                filhos.resize(0);
            }
        }

        ~No ()
        {
            if (info != nullptr)
                delete info;
        }
    };

    // arvore
    template <class T>
    class QuadTree
    {
    public:
        QuadTree(const unsigned int altura, const unsigned int largura);
        ~QuadTree();

        void inserir      (const Ponto &chave, const T &valor);
        T&   buscar       (const Ponto &chave);
        void remover      (const Ponto &chave);
        void comprimir    ();
        int  getQtdPontos () const;

        struct iterator {

            std::stack<No<T>*> caminho;
            std::stack<int> quad;

            bool operator!=(iterator outro)
            {
                iterator eu = *this;

                while (!eu.caminho.empty() && !outro.caminho.empty())
                {
                    if (eu.caminho.top() != outro.caminho.top())
                        return true;
                }

                if (eu.caminho.size() != outro.caminho.size())
                    return true;

                return false;
            }

            T & operator*()
            {
                return caminho.top()->info->valor;
            }

            T* operator->()
            {
                return caminho.top()->info->valor;
            }

            iterator operator++(int)
            {
                iterator temp = *this;
                ++(*this);
                return temp;
            }

            iterator operator++()
            {
                caminho.pop();

                while (!caminho.empty())
                {
                    // se eh folha
                    if (caminho.top()->filhos.size() == 0)
                    {
                        // se tem informacao
                        if (caminho.top()->info != nullptr)
                        {
                            break;
                        }
                        else
                        {
                            caminho.pop();
                        }
                    }
                    else
                    {
                        if (quad.size() == caminho.size())
                        {
                            quad.top()++;
                            if (quad.top() < 4)
                            {
                                caminho.push(caminho.top()->filhos[quad.top()]);
                            }
                            else
                            {
                                caminho.pop();
                                quad.pop();
                            }
                        }
                        else
                        {
                            quad.push(0);
                            caminho.push(caminho.top()->filhos[quad.top()]);

                        }
                    }
                }

                return *this;
            }

            void operator--()
            {

            }

        };

        iterator begin();
        iterator end();
        No<T>    *getRaiz() const;

    private:

        // atributos
        No<T>*      raiz       {nullptr};
        std::string excptBusca {"QuadTree::busca::out of range"};

        bool   pertenceARegiao   (const Ponto &ponto, const Regiao &quad);
        No<T>* qualFilhoPertence (const No<T> *no, const Ponto &ponto);
        No<T>* buscar            (No<T> *no, const Ponto &chave, std::stack<No<T>*> &caminho);
        void comprimir(No<T>* no);
        void destruir(No<T>* no);
    };
}


// implementacao da arvore

// construtor
template <class T>
ED::QuadTree<T>::QuadTree(unsigned int altura, unsigned int largura)
{
    altura  = altura + (altura % 2);
    largura = largura + (largura % 2);
    raiz    = new No<T>(Regiao(Ponto(largura/2, altura/2), altura/2, largura/2));
}

template <class T>
ED::QuadTree<T>::~QuadTree()
{
    destruir(raiz);
}

// inserir um novo ponto
template <class T>
void ED::QuadTree<T>::inserir(const Ponto &chave, const T &valor)
{
    if (raiz == nullptr)
        return;

    std::stack<No<T>*> caminho;
    No<T> *no = buscar(raiz, chave, caminho);

    if (no == nullptr)
        return;

    while (no->info != nullptr)
    {
        if (no->info->chave == chave)
        {
            no->info->valor = valor;
            return;
        }

        no->expandir();
        No<T> *novoNo = qualFilhoPertence(no, no->info->chave);
        novoNo->info = no->info;
        novoNo->qtdPontos++;
        no->info = nullptr;
        no = buscar(no, chave, caminho);
    }

    no->info = new Info<T>(chave, valor);
    no->qtdPontos++;

    while (!caminho.empty())
    {
        No<T> * topo = caminho.top();
        topo->qtdPontos++;
        caminho.pop();
    }
}

// buscar publico
template <class T>
T & ED::QuadTree<T>::buscar(const ED::Ponto &chave)
{
    std::stack<No<T>*> caminho;
    No<T> *no = buscar(this->raiz, chave, caminho);

    if (no == nullptr)
        throw std::out_of_range(excptBusca);

    if (no->info == nullptr)
        throw std::out_of_range(excptBusca);

    if (no->info->chave != chave)
        throw std::out_of_range(excptBusca);

    return no->info->valor;
}

// buscar interno
template <class T>
ED::No<T> * ED::QuadTree<T>::buscar(ED::No<T> *no, const ED::Ponto &chave, std::stack<No<T>*> &caminho)
{
    if (no == nullptr)
        return nullptr;

    if (!pertenceARegiao(chave, no->quad))
        return nullptr;

    if (int(no->filhos.size()) == 0)
        return no;

    caminho.push(no);
    return buscar(qualFilhoPertence(no, chave), chave, caminho);
}

// verifica se um determinado
// ponto pertence a uma regiao
template <class T>
bool ED::QuadTree<T>::pertenceARegiao(const ED::Ponto &ponto, const ED::Regiao &quad)
{
    if (ponto.x >= (quad.centro.x - quad.offsetH) &&
        ponto.x <  (quad.centro.x + quad.offsetH) &&
        ponto.y >= (quad.centro.y - quad.offsetV) &&
        ponto.y <  (quad.centro.y + quad.offsetV))
            return true;
    return false;
}

// retorna o filho do qual um
// determinado ponto pertence
template <class T>
ED::No<T> * ED::QuadTree<T>::qualFilhoPertence(const No<T> *no, const Ponto &ponto)
{
    if (int(no->filhos.size()) == 0)
        return nullptr;

    if (ponto.x < no->quad.centro.x)
    {
        if (ponto.y < no->quad.centro.y)
            return no->filhos[SW];
        else
            return no->filhos[NW];
    }
    else
    {
        if (ponto.y < no->quad.centro.y)
            return no->filhos[SE];
        else
            return no->filhos[NE];
    }
}

template <class T>
int ED::QuadTree<T>::getQtdPontos() const
{
    return raiz->qtdPontos;
}

// remove a info de um no com a chave recebida
template <class T>
void ED::QuadTree<T>::remover(const Ponto &chave)
{
    std::stack<No<T>*> caminho;
    No<T> *no = buscar(raiz, chave, caminho);

    if (no == nullptr)
        return;

    if (no->info == nullptr)
        return;

    if (no->info->chave != chave)
        return;

    delete no->info;
    no->qtdPontos--;
    no->info = nullptr;

    while (!caminho.empty())
    {
        No<T> * pai = caminho.top();
        caminho.pop();
        pai->qtdPontos--;

        if (pai->qtdPontos == 0)
        {
            for (int i = 0; i < 4; i++)
                delete pai->filhos[i];
            pai->filhos.resize(0);
        }
    }
}

template <class T>
void ED::QuadTree<T>::comprimir()
{
    comprimir(raiz);
}

template <class T>
void ED::QuadTree<T>::comprimir(No<T>* no)
{
    for (No<T>* filho : no->filhos)
        comprimir(filho);
    no->comprimir();
}


template <class T>
void ED::QuadTree<T>::destruir(No<T>* no)
{
    for (No<T>* filho : no->filhos)
        destruir(filho);

    delete no;
}

template <class T>
typename ED::QuadTree<T>::iterator ED::QuadTree<T>::begin()
{
    iterator it;
    it.caminho.push(raiz);
    it.caminho.push(raiz);
    ++it;
    return it;
}

template <class T>
typename ED::QuadTree<T>::iterator ED::QuadTree<T>::end()
{
    return iterator();
}

template <class T>
ED::No<T> *ED::QuadTree<T>::getRaiz() const
{
    return raiz;
}

#endif // QUADTREE_H
