#include <iostream>

using namespace std;

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

class Spojnik {
public:
    char *nazwa;
    int sila;
    Spojnik(char *nazwa, int sila) {
        this->nazwa = nazwa;
        this->sila = sila;
    }
    virtual void wypisz() {
        cout << nazwa;
    }
};

static Spojnik *ZMIENNA = new Spojnik("",100);
static Spojnik *NEGACJA = new Spojnik("~",90);
static Spojnik *KONIUNKCJA = new Spojnik("&",80);
static Spojnik *ALTERNATYWA = new Spojnik("|",70);
static Spojnik *IMPLIKACJA = new Spojnik("->",60);
static Spojnik *ROWNOWAZNOSC = new Spojnik("<->",50);

class Formula {
public:
    Spojnik *spojnik;
    Formula(Spojnik *spojnik) {
        this->spojnik = spojnik;
    }
    virtual void wypisz() = 0;
    virtual bool oblicz() = 0;
    virtual Formula* usunRownowaznosc() = 0;
    virtual Formula* usunImplikacje() = 0;
    virtual Formula* usunAlternatywe() = 0;
    virtual Formula* podwojnaNegacja() = 0;

    Formula* NF() {
        return usunRownowaznosc()->usunImplikacje()->usunAlternatywe()->podwojnaNegacja();
    }
};

class Zmienna: public Formula {
public:
    char *nazwa;
    bool wartosc;
    Zmienna(char *nazwa, bool wartosc) : Formula(ZMIENNA) {
        this->nazwa = nazwa;
        this->wartosc = wartosc;
    }
    Zmienna(char *nazwa) : Formula(ZMIENNA) {
        this->nazwa = nazwa;
        this->wartosc = true;
    }
    void wypisz() {
        cout << nazwa;
    }
    bool oblicz() {
        return wartosc;
    }
    Formula* usunRownowaznosc() {
        return this;
    }
    Formula* usunImplikacje() {
        return this;
    }
    Formula* usunAlternatywe() {
        return this;
    }
    Formula* podwojnaNegacja() {
        return this;
    }
};

class Formula1: public Formula {
public:
    Formula *arg;
    Formula1(Spojnik *spojnik, Formula *arg) : Formula(spojnik) {
        this->arg = arg;
    }
    void wypisz() {
        bool nawiasy = spojnik->sila > arg->spojnik->sila;
        spojnik->wypisz();
        if (nawiasy) cout << "(";
        arg->wypisz();
        if (nawiasy) cout << ")";
    }
};

class Formula2: public Formula {
public:
    Formula *arg1, *arg2;
    Formula2(Spojnik *spojnik, Formula *arg1, Formula *arg2) : Formula(spojnik) {
        this->arg1 = arg1;
        this->arg2 = arg2;
    }
    void wypisz() {
        bool nawiasy1 = spojnik->sila > arg1->spojnik->sila;
        bool nawiasy2 = spojnik->sila > arg2->spojnik->sila;
        if (nawiasy1) cout << "(";
        arg1->wypisz();
        if (nawiasy1) cout << ")";
        spojnik->wypisz();
        if (nawiasy2) cout << "(";
        arg2->wypisz();
        if (nawiasy2) cout << ")";
    }
};

class Negacja: public Formula1 {
public:
    Negacja(Formula *arg) : Formula1(NEGACJA, arg) {
    }
    bool oblicz() {
        return ! arg->oblicz();
    }
    Formula* usunRownowaznosc() {
        return new Negacja(arg->usunRownowaznosc());
    }
    Formula* usunImplikacje() {
        return new Negacja(arg->usunImplikacje());
    }
    Formula* usunAlternatywe() {
        return new Negacja(arg->usunAlternatywe());
    }
    Formula* podwojnaNegacja() {
        if (instanceof<Negacja>(arg)) {
            return ((Negacja*)arg)->arg->podwojnaNegacja();
        } else {
            return new Negacja(arg->podwojnaNegacja());
        }
    }
};

class Koniunkcja: public Formula2 {
public:
    Koniunkcja(Formula *arg1, Formula *arg2) : Formula2(KONIUNKCJA,arg1,arg2){
    }
    bool oblicz() {
        return arg1->oblicz() && arg2->oblicz();
    }
    Formula* usunRownowaznosc() {
        return new Koniunkcja(arg1->usunRownowaznosc(),arg2->usunRownowaznosc());
    }
    Formula* usunImplikacje() {
        return new Koniunkcja(arg1->usunImplikacje(),arg2->usunImplikacje());
    }
    Formula* usunAlternatywe() {
        return new Koniunkcja(arg1->usunAlternatywe(),arg2->usunAlternatywe());
    }
    Formula* podwojnaNegacja() {
        return new Koniunkcja(arg1->podwojnaNegacja(),arg2->podwojnaNegacja());
    }
};

class Alternatywa: public Formula2 {
public:
    Alternatywa(Formula *arg1, Formula *arg2) : Formula2(ALTERNATYWA,arg1,arg2){
    }
    bool oblicz() {
        return arg1->oblicz() || arg2->oblicz();
    }
    Formula* usunRownowaznosc() {
        return new Alternatywa(arg1->usunRownowaznosc(),arg2->usunRownowaznosc());
    }
    Formula* usunImplikacje() {
        return new Alternatywa(arg1->usunImplikacje(),arg2->usunImplikacje());
    }
    Formula* usunAlternatywe() {
        return new Negacja(new Koniunkcja(new Negacja(arg1->usunAlternatywe()),new Negacja(arg2->usunAlternatywe())));
    }
    Formula* podwojnaNegacja() {
        return new Alternatywa(arg1->podwojnaNegacja(),arg2->podwojnaNegacja());
    }
};

class Implikacja: public Formula2 {
public:
    Implikacja(Formula *arg1, Formula *arg2) : Formula2(IMPLIKACJA,arg1,arg2){
    }
    bool oblicz() {
        return ! arg1->oblicz() | arg2->oblicz();
    }
    Formula* usunRownowaznosc() {
        return new Implikacja(arg1->usunRownowaznosc(),arg2->usunRownowaznosc());
    }
    Formula* usunImplikacje() {
        return new Alternatywa(new Negacja(arg1->usunImplikacje()),arg2->usunImplikacje());
    }
    Formula* usunAlternatywe() {
        return new Implikacja(arg1->usunAlternatywe(),arg2->usunAlternatywe());
    }
    Formula* podwojnaNegacja() {
        return new Implikacja(arg1->podwojnaNegacja(),arg2->podwojnaNegacja());
    }
};

class Rownowaznosc: public Formula2 {
public:
    Rownowaznosc(Formula *arg1, Formula *arg2) : Formula2(ROWNOWAZNOSC,arg1,arg2){
    }
    bool oblicz() {
        return arg1->oblicz() == arg2->oblicz();
    }
    Formula* usunRownowaznosc() {
        Formula *f1 = arg1->usunRownowaznosc();
        Formula *f2 = arg2->usunRownowaznosc();
        return new Koniunkcja(new Implikacja(f1,f2),new Implikacja(f2,f1));
    }
    Formula* usunImplikacje() {
        return new Rownowaznosc(arg1->usunImplikacje(),arg2->usunImplikacje());
    }
    Formula* usunAlternatywe() {
        return new Rownowaznosc(arg1->usunAlternatywe(),arg2->usunAlternatywe());
    }
    Formula* podwojnaNegacja() {
        return new Rownowaznosc(arg1->podwojnaNegacja(),arg2->podwojnaNegacja());
    }
};


int main() {
    Zmienna *p = new Zmienna("p", false);
    Zmienna *q = new Zmienna("q");
    Negacja *n = new Negacja(p);
    Negacja *n1 = new Negacja(n);
    Koniunkcja *k1 = new Koniunkcja(n,n1);
    Alternatywa *a1 = new Alternatywa(k1,n1);
    Koniunkcja *k2 = new Koniunkcja(a1,n1);
    Rownowaznosc *r1 = new Rownowaznosc(p,q);

    n1->wypisz();
    cout << endl;

    k1->wypisz();
    cout << endl;

    a1->wypisz();
    cout << endl;

    k2->wypisz();
    cout << endl;
    cout << k2->oblicz() << endl;

    r1->wypisz();
    cout << endl;

    r1->usunRownowaznosc()->wypisz();
    cout << endl;

    r1->NF()->wypisz();
    cout << endl;

    Rownowaznosc *rr = new Rownowaznosc(r1,r1);

    rr->NF()->wypisz();
    cout << endl;

    rr->NF()->podwojnaNegacja()->wypisz();
    cout << endl;

    return 0;
}
