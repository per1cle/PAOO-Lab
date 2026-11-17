#include "Book.hpp"
#include <iostream>
#include <string>


// ITEM 4: Exemple cu variante GRESITE
class UninitBook {   //Membri neinitializati (valori garbage de pe stiva)
public:
    int id;
    int pages;
    UninitBook() {
        // Nu initializam id si pages
    }
};

// ITEM 4: Atribuire in corpul constructorului in loc de Member Initialization List
class BadAssignedBook {
public:
    std::string title;
    std::string author;
    int pages;

    BadAssignedBook(const std::string& t, const std::string& a, int p) {
        // Aici title si author au fost deja construite cu default constructor, iar apoi apelam si operator= peste ele
        title = t;
        author = a;
        pages = p;
        std::cout << "  [BadAssignedBook] Atribuire in corpul constructorului pentru \"" << title << "\"\n";
    }
};

// ITEM 5: Clasa goala - compilatorul genereaza implicit constructor, copy constructor si operator= (assignment)
class EmptyBook {
public:
    std::string title;
    int pages;
};

// ITEM 5: Clasa cu referinta si const - compilatorul nu generezeaza operator= 
class RefConstBook {
public:
    const int id;
    std::string& owner;

    RefConstBook(int id, std::string& owner) : id(id), owner(owner) {}
};

// ITEM 6 - Clasa de baza Uncopyable
// Declaram copy constructor si operator= ca PRIVATE si fara implementare
class Uncopyable {
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:
    Uncopyable(const Uncopyable&);
    Uncopyable& operator=(const Uncopyable&);
};

class UniqueManuscript : private Uncopyable {
private:
    std::string title;
public:
    UniqueManuscript(const std::string& t) : title(t) {
        std::cout << "  [UniqueManuscript] Creat manuscrisul unic: \"" << title << "\"\n";
    }
    void afisare() const {
        std::cout << "  Manuscris Unic: \"" << title << "\" (nu poate fi copiat - mosteneste Uncopyable)\n";
    }
};

// ITEM 6 - Dezactivare explicita cu '= delete'
class RareBook {
private:
    std::string title;
public:
    RareBook(const std::string& t) : title(t) {
        std::cout << "  [RareBook] Creata cartea rara: \"" << title << "\"\n";
    }

    // Interzicem explicit generarea copy constructor-ului si a lui operator=
    RareBook(const RareBook&) = delete;
    RareBook& operator=(const RareBook&) = delete;

    void afisare() const {
        std::cout << "  Carte Rara: \"" << title << "\" (nu poate fi copiata - functii marcate cu = delete)\n";
    }
};

int main() {
    std::cout << "=== ITEM 4: Make sure that objects are initialized before they're used ===\n\n";

    // gresit: folosirea membrilor neinitializati
    UninitBook ub;
    std::cout << "  [Gresit - Neinitializat] ID: " << ub.id << ", Pages: " << ub.pages << " (garbage values)\n";

    // ineficient: atribuire in corpul constructorului
    BadAssignedBook bb("1984", "George Orwell", 328);

    // good : folosirea Member Initialization List in clasa Book
    Book b1(101, "Effective C++", "Scott Meyers", 320);
    b1.afisare();

    std::cout << "\n=== ITEM 5: Know what functions C++ silently writes and calls ===\n\n";

    // functii generate de compilator 
    EmptyBook eb1;                  // constructor default
    eb1.title = "Clean Code";        
    eb1.pages = 464;

    EmptyBook eb2(eb1);         // copy constructor default
    EmptyBook eb3;
    eb3 = eb1;                   // copy assignment operator default
    std::cout << "  [EmptyBook] Copiata cu functiile generate de compilator: \""
              << eb2.title << "\" (" << eb2.pages << " pagini)\n";

    // compilatorul nu genereaza operator=  (pt const si referinta)
    std::string owner1 = "Biblioteca Centrala";
    std::string owner2 = "Biblioteca Facultatii";
    RefConstBook rcb1(1, owner1);
    RefConstBook rcb2(2, owner2);
    // rcb2 = rcb1;   //aici avem eroare

    // atunci definim noi copy constructor si operator=
    Book b2 = b1;                // apeleaza Copy Constructor-ul din Book
    Book b3(102, "The C++ Programming Language", "Bjarne Stroustrup", 1368);
    b3 = b1;                     // apeleaza operator= (pastreaza id=102, copiaza restul)
    b3.afisare();

    std::cout << "\n=== ITEM 6: Explicitly disallow the use of compiler-generated functions you do not want ===\n\n";

    // Varianta 1 : mostenirea clasei Uncopyable 
    UniqueManuscript m1("Codex Gigas");
    m1.afisare();
    // UniqueManuscript m2 = m1; // eroare aici
    // UniqueManuscript m3("Alt Manuscris");
    // m3 = m1;                  // eroare si aici

    // Varianta 2 : '= delete'
    RareBook rb1(" Gutenberg Bible");
    rb1.afisare();
    // RareBook rb2 = rb1;       // eroare
    // RareBook rb3("Alta Carte Rara");
    // rb3 = rb1;                // eroare

    return 0;
}
