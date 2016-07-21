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
    struct No
    {
        T   valor;
        No* filhoEsq;
        No* filhoDir;


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
            delete filhoDir;
            delete filhoEsq;
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
//        difundir(valor);
//        if(this->raiz->valor == valor)
//            return false;
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
        difundir(valor);
        if(this->raiz->valor != valor)
            return false;
        else{
            No* pai = nullptr;
            No* no = this->raiz;
            while(no->valor!=valor && no!=nullptr){
                  pai = no;
                  no = valor < no->valor? no->esq : no->dir;
            }

            if (no->ehFolha()){
                if(pai->filhoDir == no)
                    pai->filhoDir == nullptr;
                else
                    pai->filhoEsq == nullptr;

            }else if(no->filhoDir==nullptr){
                if(pai->filhoDir==no)
                    pai->filhoDir = no->filhoEsq;
                else
                    pai->filhoEsq = no->filhoEsq;

            }else if(no->filhoEsq==nullptr){
                if(pai->filhoDir==no)
                    pai->filhoDir = no->filhoDir;
                else
                    pai->filhoEsq = no->filhoDir;

            }else{
                No* substituto = no->filhoDir;
                No* pai_substituto;
                while(true){
                      if(substituto->filhoEsq != nullptr){
                          pai_substituto = substituto;
                          substituto = substituto->filhoEsq;
                      }
                      else
                          break;
                }

                if(pai->filhoDir == no)
                    pai->filhoDir = substituto;
                else
                    pai->filhoEsq = substituto;

                pai_substituto->filhoEsq = nullptr;
                substituto->filhoDir = no->filhoDir;
                substituto->filhoEsq = no->filhoEsq;
                delete no;
            }
        }
    }

    bool buscar(const T &valor){
        difundir(valor);
        return this->raiz->valor == valor;
    }

    void savetofile(const char* fnome)  {
            if (raiz==0) return;
            queue<No*> fila;
            fila.push(raiz);
            ofstream out(fnome);
            while (!fila.empty()) {
                No* q = fila.front();
                out << q->valor << " ";
                if (q->filhoEsq != 0) {
                    fila.push(q->filhoEsq);
                    out << " [" <<q->filhoEsq->valor << "] ";
                } else
                    out << " [] ";
                if (q->filhoDir != 0) {
                    fila.push(q->filhoDir);
                    out << " [" << q->filhoDir->valor << "] ";
                } else
                    out << " [] ";
                out << endl;
                fila.pop();
            }
            out.close();
        }





private:

    No* raiz;

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

    void difundir(const T &chave){
        vector<No*> ancestrais;
        No* no = this->raiz;
        while(no!=nullptr && no->valor!=chave){
            ancestrais.push_back(no);
            no = chave > no->valor ? no->filhoDir : no->filhoEsq;
        }

        if(no == nullptr){
            no = ancestrais.back();
            ancestrais.pop_back();
        }

        No* pai;
        No* avo;
        while(ancestrais.size()){
            pai = ancestrais.back();
            ancestrais.pop_back();
            if (pai == this->raiz){
                if(this->raiz->filhoEsq == no)
                    rotDir(this->raiz);
                else
                    rotEsq(this->raiz);

            }else{
                cout <<"t2\n";
                avo = ancestrais.back();
                ancestrais.pop_back();
                if(avo->filhoEsq == pai && pai->filhoEsq == no){
                    No* aux = avo;
                    rotDir(aux);
                    aux = pai;
                    rotDir(aux);

                }else if(avo->filhoDir == pai && pai->filhoDir == no){
                    No* aux = avo;
                    rotEsq(avo);
                    aux = pai;
                    rotEsq(pai);
                    this->savetofile("b");
                    break;
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
