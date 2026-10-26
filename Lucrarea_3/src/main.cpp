#include "Student.hpp"
#include <iostream>

// pt 2.
class Neinitializat{
    public:
        int varsta;
        int *note;
        Neinitializat() {}
};

// pt 4.
class MemoryLeakStudent{
    public:
        int* note;
        MemoryLeakStudent(int numarNote) {
            note = new int[numarNote]; // alocare pe HEAP
        }
        ~MemoryLeakStudent() {
            // nu punem delete[] note; -> memoria ramane alocata pe HEAP -> memory leak
        }
};

// pt 5.
class ShallowStudent{
    public:
        int *note;
        ShallowStudent(int nota) {note = new int[1]{nota};}
        ~ShallowStudent() {delete[] note;}
        void afisare() const {
            std::cout << "  ShallowStudent [Nota : " << note[0] << "]\n";
        }
};

int main(){

    //2. Incapsulare
    int note[] = {10, 8, 9};
    Student s1(101, "Popescu Ion", 20, 3, note);
    s1.afisare();

    //s1.varsta = -5;          //eroare compilare - membru privat

    // testari cu date invalide
    s1.setVarsta(-4);
    s1.setNota(0, 15);

    // testari cu date valide
    s1.setVarsta(19);
    s1.afisare();


    //3. Initializarea membrilor din constructor
    Neinitializat sn1;
    std::cout << "  [Neinitializat] Varsta: " << sn1.varsta;    // garbage value de pe stiva

    //4. Eliberarea heap-ului în cadrul destructorului 
    {   
        MemoryLeakStudent mls1(3);
        // aici se termina scope-ul -> se apeleaza destructorul DAR nu se elibereaza memoria de pe HEAP  -> check cu valgrind ./lab3_paoo
    }

    //5. Suprascrierea copy constructor-ului
    // deep copy
    Student s2 = s1; // apel copy constructor
    s2.setNota(0, 7); // modificam copia, nu afectam sursa

    s1.afisare();
    s2.afisare();

    //shallow copy
    ShallowStudent ss1(10);
    ShallowStudent ss2 = ss1; // apel copy constructor
    ss1.afisare();
    ss2.afisare();
    ss2.note[0] = 5; // modificam copia, afectam sursa
    ss1.afisare();
    ss2.afisare();
    // la iesire din scope se da delete[] de doua ori -> double free -> crash

    // 6. Crearea unui move constructor
    Student s3 = std::move(s1); // apel move constructor
    s3.afisare();
    s1.afisare(); // s1 a devenit invalid (note = nullptr)
    return 0;
}