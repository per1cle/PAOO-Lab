#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <string>

class Student {
private:
    const int idMatricol; // const - nu poate fi modificat 
    std::string nume;
    int varsta;
    int numarNote;
    int* note;           

public:
    // constructor
    Student(int id, const std::string& nume, int varsta, int numarNote, const int* noteInitiale);

    // deconstructor
    ~Student();

    // copy constructor -- deep copy
    Student(const Student& other);

    // move constructor 
    Student(Student&& other) noexcept;

    // gettere si settere 
    int getIdMatricol() const;
    std::string getNume() const;
    void setNume(const std::string& numeNou);

    int getVarsta() const;
    bool setVarsta(int varstaNoua);

    int getNumarNote() const;
    int getNota(int index) const;
    bool setNota(int index, int notaNoua);

    const int* getAdresaNoteHeap() const;

    void afisare() const;
};

#endif 
