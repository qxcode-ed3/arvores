#ifndef SCAPEGOATTREE_H
#define SCAPEGOATTREE_H

#include <stack>
#include <queue>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>

#define NULL 0

using namespace std;

template <typename T>
class ScapeGoatTree{

    struct no{
        T valor;
        no *esq;
        no *dir;

    };
    int altura;
    float alfa;
    int maxTamanho;
    int tamanho;
    no *raiz;

    no* novo_no(T valor){
        no *novo_no = new no;
        novo_no->valor = valor;
        novo_no->esq= NULL;
        novo_no->dir = NULL;
    }

    no *menorEsquerda(no **no_raiz){
        if((*no_raiz)->esq != NULL)
            return menorEsquerda(&(*no_raiz)->esq);
        else{
            no *aux = *no_raiz;
            if((*no_raiz)->dir!= NULL) // se nao houver essa verificacao, esse nó vai perder todos os seus filhos da direita!
                *no_raiz = (*no_raiz)->dir;
            else
                *no_raiz = NULL;
            return aux;
        }
    }

    void createBackBone(no* root){
        no* tail = root;
        no* rest = tail->dir;
        while(rest!=NULL){
            if(rest->esq==NULL){
                tail = rest;
                rest = rest->dir;

            }
            else{
                no * temp = rest->esq;
                rest->esq = temp->dir;
                temp->dir = rest;
                rest = temp;
                tail->dir = temp;
            }
        }
    }
    void backBoneToTree(no * root, int size){
        int leaves =  size + 1 - pow(2,floor(log2(size+1)) ) ;
        compress(root, leaves);
        size = size - leaves;
        while (size > 1) {
            compress(root, floor( size/2 ) );
            size = floor( size/2);
        }
    }

    void compress(no* root, int count){
        no * scanner = root;
        for (int i=0; i< count; i++){
            no * child = scanner->dir;
            scanner->dir = child->dir;
            scanner = scanner->dir;
            child->dir = scanner->esq;
            scanner->esq = child;
        }
    }

    int size_no(no * node){
        if(node==NULL)
            return 0;
        else
            return size_no(node->esq) + size_no(node->dir) + 1;
    }

    no* rebuild(no * raiz_rebuild, int size){
        no* dummy = novo_no(0);
        dummy->dir = raiz_rebuild;
        createBackBone(dummy);
        raiz_rebuild = dummy;
        backBoneToTree(raiz_rebuild,size);
        raiz_rebuild = dummy->dir;
        return raiz_rebuild;
    }



public:

    ScapeGoatTree(){
        raiz = NULL;
        alfa = 0.75;
        tamanho=0;
        maxTamanho = 0;
        altura = 0;

    }

    ScapeGoatTree(float valor_alfa){
        raiz = NULL;
        alfa = valor_alfa;
        tamanho=0;
        maxTamanho = 0;
        altura = 0;

    }

    void rebuild_tree(){
        no* dummy = novo_no(0);
        dummy->dir = raiz;
        createBackBone(dummy);
        raiz = dummy;
        backBoneToTree(raiz,tamanho);
        raiz = raiz->dir;
    }

    bool inserir(T valor){
        int nivel = 0;
        vector<no*> ancestrais;
        no * novo = novo_no(valor);

        if(raiz==NULL){
            raiz = novo;
        }
        else{
            no* pai = NULL;
            no* cao = raiz;

            while(cao!=NULL){
                nivel++;
                if(cao->valor== valor)
                    return false;
                pai = cao;
                ancestrais.push_back(pai);
                cao = valor < cao->valor? cao->esq : cao->dir;
            }
            no *filho = novo;
            if(filho->valor< pai->valor)
                pai->esq = filho;
            else
                pai->dir = filho;
        }

        tamanho++;

        if( nivel > (log(tamanho)/log(1/alfa)) ){
            //buscar por o nó scapeGoat
            no * scapeGoatNode = NULL;
            int size = 1;
            int irmaoSize = 0;
            int totalSize = 0;
            no* parent = NULL;
            T valor_filho = valor;

            while(!ancestrais.empty()){
                parent = ancestrais.back();
                ancestrais.pop_back();
                //Calcula o size da sub árvore com raiz no nó irmão
                irmaoSize = valor_filho > parent->valor ? size_no(parent->esq) : size_no(parent->dir);
                //Calcula o size para o parent
                totalSize = size + irmaoSize + 1;
                //verificar se o parente é peso-ala-balanceado, se não ele é o scapeGoat
                float alfa_size = alfa* totalSize;

                if(! ( (size <= alfa_size) & (irmaoSize <= alfa_size) ) ){
                    scapeGoatNode = parent;
                    size = totalSize;
                    break;
                }
                size = totalSize;
            }

            if(scapeGoatNode==raiz)
                 maxTamanho = tamanho;

            //Rebuild árvore com raiz no nó scapeGoat
            no * nova_raiz =  rebuild(scapeGoatNode,size);
            if(ancestrais.empty()){
                raiz = nova_raiz;
            }
            else{
                no* pai_raiz = ancestrais.back();
                if(pai_raiz->valor > nova_raiz->valor)
                    pai_raiz->esq = nova_raiz;
                else
                    pai_raiz->dir = nova_raiz;
            }


        }

        maxTamanho = tamanho > maxTamanho ? tamanho : maxTamanho;

        return true;
    }

    bool remover(T valor){
        no *pai = NULL;
        no *no_r = raiz;
        while(no_r->valor!=valor && no_r!=NULL){
            pai = no_r;
            no_r = valor < no_r->valor? no_r->esq : no_r->dir;
        }
        //Nó não encontrado
        if(no_r==NULL)
            return false;
        else{
            //Nó folha
            if(no_r->esq==NULL && no_r->dir==NULL){
                if(pai->esq==no_r)
                    pai->esq = NULL;
                else
                    pai->dir = NULL;

                no_r->esq= NULL;
                no_r->dir= NULL;
                free(no_r);
                no_r = NULL;
            }
            //Nó tem apenas filho direito
            else if(no_r->esq==NULL){
                if(pai->esq==no_r)
                    pai->esq = no_r->dir;
                else
                    pai->dir = no_r->dir;
                no_r->esq= NULL;
                no_r->dir= NULL;
                free(no_r);
                no_r = NULL;
            }
            //Nó tem apenas filho esquerdo
            else if(no_r->dir==NULL){
                if(pai->esq==no_r)
                    pai->esq = no_r->esq;
                else
                    pai->dir = no_r->esq;
                no_r->esq= NULL;
                no_r->dir= NULL;
                free(no_r);
                no_r = NULL;
            }
            // Nó tem os dois filhos
            else{
                //Busca menor filho esquerdo da sub arvore direita
                no* menor_esquerda = menorEsquerda(&no_r->dir);
                no_r->valor = menor_esquerda->valor;
            }

            tamanho--;
            cout << tamanho <<" <= " << alfa << "*" <<maxTamanho << endl;
            if(tamanho <= alfa*maxTamanho){
                cout<< "rebuilda da remoção" << endl;
                maxTamanho = tamanho;
                rebuild_tree();
            }

            return true;
        }


    }

    bool buscar(T valor){
        no* no_busca = raiz;

        while(no_busca!=NULL){
            if(no_busca->valor== valor)
                return true;
            no_busca = valor < no_busca->valor? no_busca->esq : no_busca->dir;
        }
        return false;


    }
};

#endif // SCAPEGOATTREE_H
