#ifndef SPLAY_H
#define SPLAY_H

#include<iostream>
#include<vector>
#include<queue>
#include<fstream>
//#include<string>

using namespace std;


template <class T>
class Splay{
public:
    struct No
    {
        T   valor;
        No* filhoEsq;
        No* filhoDir;
        int largura;


        No(){
            this->filhoDir = nullptr;
            this->filhoEsq = nullptr;
        }

        No(T valor){
            this->valor = valor;
            this->filhoDir = nullptr;
            this->filhoEsq = nullptr;
        }

        ~No(){
        }

        void setValor(T valor){
            this->valor = valor;
        }

        bool ehFolha(){
            return this->filhoDir == nullptr && this->filhoEsq == nullptr;
        }
    };




public:
    Splay(){
        this->raiz = nullptr;
    }


    bool inserir(const T &valor){
        if(this->raiz==nullptr)
            this->raiz = new No(valor);
        if(__difundir(raiz, valor))
            return false;
        else{
            No* no = this->raiz;
            while(true){
                if(valor > no->valor){
                    if (no->filhoDir == nullptr){
                        no->filhoDir = new No(valor);
                        return true;
                    }
                    no = no->filhoDir;
                }else{
                    if (no->filhoEsq == nullptr){
                        no->filhoEsq = new No(valor);
                        return true;
                    }
                    no = no->filhoEsq;
                }
            }

        }
    }



    bool remover(const T &valor){
        return remover(raiz, valor);
    }

    bool buscar(const T &valor){
        return __difundir(raiz, valor);
    }

private:
    No* raiz;

    bool remover(No *&no, const T &valor){
        if (no == nullptr){
            return false;
        }
        else if (no->valor == valor){
            if (no->ehFolha()){
                delete no;
                no = nullptr;
            }
            else if (no->filhoDir == nullptr){
                No *aux = no;
                no = no->filhoEsq;
                delete aux;

            }else if (no->filhoEsq == nullptr){
                No *aux = no;
                no = no->filhoDir;
                delete aux;

            }else{
                No *auxPai = no;
                No *aux = no->filhoDir;
                while (aux->filhoEsq != nullptr){
                    auxPai = aux;
                    aux = aux->filhoEsq;
                }

                no->valor = aux->valor;

                if (auxPai != no)
                    remover(auxPai->filhoEsq, aux->valor);
                else
                    remover(no->filhoDir, aux->valor);
            }

            return true;

        }else if (no->valor < valor){
            return remover(no->filhoDir, valor);

        }else{
            return remover(no->filhoEsq, valor);
        }
    }

    void rotDir(No*& p) {
        No* q = p->filhoEsq;
        if (q!=nullptr) {
            p->filhoEsq = q->filhoDir;
            q->filhoDir = p;
            p = q;
        }
    }
    void rotEsq(No*& p) {
        No* q = p->filhoDir;
        if (q!=nullptr) {
            p->filhoDir = q->filhoEsq;
            q->filhoEsq = p;
            p = q;
        }
    }
    void rotDupDir(No*& p) {
        rotEsq(p->filhoEsq);
        rotDir(p);
    }
    void rotDupEsq(No*& p) {
        rotDir(p->filhoDir);
        rotEsq(p);
    }

    bool __difundir(No *&no, const T &chave){
        if (no == nullptr)
            return false;
        if (no->valor == chave){
            return true;
        }
        else if (no->valor < chave){
            if (__difundir(no->filhoDir, chave)){
                if (no == this->raiz){
                    if (no->filhoDir->valor != chave)
                        if (no->filhoDir->filhoEsq != nullptr &&
                            no->filhoDir->filhoEsq->valor == chave)
                            rotDir(raiz->filhoDir);
                        else
                            rotEsq(this->raiz);
                    rotEsq(this->raiz);
                } else {
                    if (no->filhoDir != nullptr && no->filhoDir->filhoDir != nullptr &&
                        no->filhoDir->filhoDir->valor == chave) {
                        rotEsq(no);
                        rotEsq(no);
                    } else if (no->filhoDir != nullptr && no->filhoDir->filhoEsq != nullptr &&
                               no->filhoDir->filhoEsq->valor == chave) {
                        rotDir(no->filhoDir);
                        rotEsq(no);
                    }

                }

                return true;
            }

        }else{
            if (__difundir(no->filhoEsq, chave)){
                if (no == this->raiz){
                    if (no->filhoEsq->valor != chave)
                        if (no->filhoEsq->filhoDir!= nullptr &&
                            no->filhoEsq->filhoDir->valor == chave)
                            rotEsq(raiz->filhoEsq);
                        else
                            rotDir(this->raiz);
                    rotDir(this->raiz);
                } else {
                    if (no->filhoEsq != nullptr && no->filhoEsq->filhoEsq != nullptr &&
                            no->filhoEsq->filhoEsq->valor == chave) {
                        rotDir(no);
                        rotDir(no);

                    } else if (no->filhoEsq != nullptr && no->filhoEsq->filhoDir != nullptr &&
                               no->filhoEsq->filhoDir->valor == chave) {
                        rotEsq(no->filhoEsq);
                        rotDir(no);
                    }
                }
                return true;
            }
        }

        return false;
    }

    void difundir(const T &chave){
        vector<No*> ancestrais;
        No* no = this->raiz;
        while(no!=nullptr && no->valor!=chave){
            ancestrais.push_back(no);
            no = chave > no->valor ? no->filhoDir : no->filhoEsq;
        }

        if(no == nullptr){
            return;
            no = ancestrais.back();
            ancestrais.pop_back();
        }

        No* chaveNo = no;
        No* pai;
        No* avo;
        while(ancestrais.size()){
            pai = ancestrais.back();
            if (pai->valor < no->valor)
            {
                pai->filhoDir = chaveNo;
            }
            else
            {
                pai->filhoEsq = chaveNo;
            }

            ancestrais.pop_back();
            if (pai == this->raiz){
                if(this->raiz->filhoEsq == no)
                    rotDir(this->raiz);
                else
                    rotEsq(this->raiz);

            }else{
                avo = ancestrais.back();
                ancestrais.pop_back();
                if(avo->filhoEsq == pai && pai->filhoEsq == no){
                    No* aux = avo;
                    rotDir(avo);
                    aux = pai;
                    rotDir(pai);

                }else if(avo->filhoDir == pai && pai->filhoDir == no){
                    No* aux = avo;
                    rotEsq(avo);
                    aux = pai;
                    rotEsq(pai);
                }else if(avo->filhoEsq == pai && pai->filhoDir == no){
                    rotDupEsq(avo);

                }else{
                    rotDupDir(avo);
                }

            }
        }

    }

};

#endif // SPLAY_H


