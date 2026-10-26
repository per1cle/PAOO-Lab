#include "Student.hpp"
#include <iostream>
#include <utility>

// initializarea membrilor din constructor
Student::Student(int id, const std::string& nume, int varsta, int numarNote, const int* noteInitiale)
    : idMatricol(id),
      nume(nume),
      varsta(varsta > 0 ? varsta : 18),
      numarNote(numarNote > 0 ? numarNote : 0),
      note(nullptr) {
    if (this->numarNote > 0 && noteInitiale != nullptr) {
        note = new int[this->numarNote]; // Alocare pe HEAP
        for (int i = 0; i < this->numarNote; ++i) {
            note[i] = (noteInitiale[i] >= 1 && noteInitiale[i] <= 10) ? noteInitiale[i] : 1;
        }
    }
    std::cout << "  [Constructor] Creat Student (ID " << idMatricol << ", \"" << this->nume
              << "\") | Adresa note pe HEAP: " << static_cast<const void*>(note) << "\n";
}

// eliberarea heap-ului în cadrul destructorului 
Student::~Student() {
    std::cout << "  [Destructor] Distrugere Student (ID " << idMatricol << ", \"" << nume
              << "\") | Eliberare HEAP de la adresa: " << static_cast<const void*>(note) << "\n";
    delete[] note;
    note = nullptr;
}

// suprascrierea copy constructor-ului 
Student::Student(const Student& other)
    : idMatricol(other.idMatricol),
      nume(other.nume + " (Copie)"),
      varsta(other.varsta),
      numarNote(other.numarNote),
      note(nullptr) {
    if (numarNote > 0 && other.note != nullptr) {
        note = new int[numarNote]; // alocam zona noua pe heap
        for (int i = 0; i < numarNote; ++i) {
            note[i] = other.note[i]; // copiem valorile notelor de la sursa
        }
    }
    std::cout << "  [Copy Constructor - Deep Copy] Copiat din \"" << other.nume
              << "\" (HEAP vechi: " << static_cast<const void*>(other.note)
              << " -> HEAP nou: " << static_cast<const void*>(note) << ")\n";
}

// crearea unui move constructor 
Student::Student(Student&& other) noexcept
    : idMatricol(other.idMatricol),
      nume(std::move(other.nume)),
      varsta(other.varsta),
      numarNote(other.numarNote),
      note(other.note) { // luam adresa de pe heap de la sursa
    other.numarNote = 0;
    other.note = nullptr;  // sursa nu mai detine resuersele de pe heap

    std::cout << "  [Move Constructor] Resurse mutate pentru \"" << nume
              << "\" | Adresa HEAP păstrată: " << static_cast<const void*>(note)
              << " (sursa a devenit nullptr)\n";
}

// gettere si settere -- INCAPSULARE

int Student::getIdMatricol() const {
    return idMatricol;
}

std::string Student::getNume() const {
    return nume;
}

void Student::setNume(const std::string& numeNou) {
    if (!numeNou.empty()) {
        nume = numeNou;
    } else {
        std::cout << "  [Eroare Validare] Numele nu poate fi gol!\n";
    }
}

int Student::getVarsta() const {
    return varsta;
}

bool Student::setVarsta(int varstaNoua) {
    if (varstaNoua >= 18 && varstaNoua <= 100) {
        varsta = varstaNoua;
        return true;
    }
    std::cout << "  [Eroare Validare] Varsta " << varstaNoua << " este invalida (trebuie 18..100)!\n";
    return false;
}

int Student::getNumarNote() const {
    return numarNote;
}

int Student::getNota(int index) const {
    if (index >= 0 && index < numarNote && note != nullptr) {
        return note[index];
    }
    return -1;
}

bool Student::setNota(int index, int notaNoua) {
    if (index < 0 || index >= numarNote || note == nullptr) {
        std::cout << "  [Eroare Validare] Indexul notei (" << index << ") este invalid!\n";
        return false;
    }
    if (notaNoua < 1 || notaNoua > 10) {
        std::cout << "  [Eroare Validare] Nota " << notaNoua << " este invalida (trebuie 1..10)!\n";
        return false;
    }
    note[index] = notaNoua;
    return true;
}

const int* Student::getAdresaNoteHeap() const {
    return note;
}

void Student::afisare() const {
    std::cout << "  Student [ID: " << idMatricol
              << " | Nume: \"" << (nume.empty() ? "<mutat>" : nume)
              << "\" | Varsta: " << varsta
              << " | Adresa HEAP note: " << static_cast<const void*>(note)
              << " | Note (" << numarNote << "): ";
    if (note != nullptr && numarNote > 0) {
        for (int i = 0; i < numarNote; ++i) {
            std::cout << note[i] << (i + 1 < numarNote ? ", " : "");
        }
    } else {
        std::cout << "fara note (nullptr)";
    }
    std::cout << "]\n";
}
