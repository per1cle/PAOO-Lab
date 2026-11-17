#include "Car.hpp"
#include <iostream>

Car::Car(const std::string& brand, const std::string& model, int horsePower, const int* history, int count)
    : brand(brand),
      model(model),
      horsePower(horsePower),
      serviceHistory(nullptr),
      serviceCount(count > 0 ? count : 0) {
    if (serviceCount > 0 && history != nullptr) {
        serviceHistory = new int[serviceCount];
        for (int i = 0; i < serviceCount; ++i) {
            serviceHistory[i] = history[i];
        }
    }
    std::cout << "  [Constructor Car] Creata masina \"" << this->brand << " " << this->model
              << "\" (" << this->horsePower << " HP) | HEAP: " << static_cast<const void*>(serviceHistory) << "\n";
}

Car::~Car() {
    std::cout << "  [Destructor Car] Distrugere masina \"" << brand << " " << model
              << "\" | Eliberare HEAP: " << static_cast<const void*>(serviceHistory) << "\n";
    delete[] serviceHistory;
    serviceHistory = nullptr;
}

Car::Car(const Car& other)
    : brand(other.brand),
      model(other.model),
      horsePower(other.horsePower),
      serviceHistory(nullptr),
      serviceCount(other.serviceCount) {
    if (serviceCount > 0 && other.serviceHistory != nullptr) {
        serviceHistory = new int[serviceCount];
        for (int i = 0; i < serviceCount; ++i) {
            serviceHistory[i] = other.serviceHistory[i];
        }
    }
    std::cout << "  [Copy Constructor Car] Copiata masina \"" << brand << " " << model
              << "\" (HEAP nou: " << static_cast<const void*>(serviceHistory) << ")\n";
}

// Item 10 & Item 11: Copy Assignment Operator
Car& Car::operator=(const Car& other) {
    // Item 11: Verificare pentru self-assignment (identity test) + exception safety
    if (this == &other) {
        std::cout << "  [operator= Car] Self-assignment detectat (this == &other) pentru \""
                  << brand << " " << model << "\" -> ignorat in siguranta!\n";
        return *this; // Item 10: returnam referinta la *this
    }

    // Item 11 (Ordering of statements / exception safety):
    // Alocam mai intai noua resursa pe HEAP si abia apoi stergem vechea resursa!
    int* newHistory = nullptr;
    if (other.serviceCount > 0 && other.serviceHistory != nullptr) {
        newHistory = new int[other.serviceCount];
        for (int i = 0; i < other.serviceCount; ++i) {
            newHistory[i] = other.serviceHistory[i];
        }
    }

    delete[] serviceHistory; // eliberam vechea memorie abia dupa ce noua alocare a reusit

    // Item 12: Copiem TOTI membrii clasei
    brand = other.brand;
    model = other.model;
    horsePower = other.horsePower;
    serviceHistory = newHistory;
    serviceCount = other.serviceCount;

    std::cout << "  [operator= Car] Atribuire completa pentru \"" << brand << " " << model
              << "\" (HEAP nou: " << static_cast<const void*>(serviceHistory) << ")\n";

    // Item 10: Have assignment operators return a reference to *this
    return *this;
}

// Item 10: Conventia return *this se aplica si operatorilor compusi (+=, -=, etc.)
Car& Car::operator+=(int extraHp) {
    horsePower += extraHp;
    std::cout << "  [operator+= Car] Tuning: +" << extraHp << " HP pentru \""
              << brand << " " << model << "\" (Total: " << horsePower << " HP)\n";
    return *this;
}

void Car::afisare() const {
    std::cout << "  Car [Brand: \"" << brand
              << "\" | Model: \"" << model
              << "\" | Putere: " << horsePower << " HP"
              << " | HEAP: " << static_cast<const void*>(serviceHistory)
              << " | Istoric revizii (" << serviceCount << "): ";
    if (serviceHistory != nullptr && serviceCount > 0) {
        for (int i = 0; i < serviceCount; ++i) {
            std::cout << serviceHistory[i] << (i + 1 < serviceCount ? ", " : "");
        }
    } else {
        std::cout << "fara istoric";
    }
    std::cout << "]\n";
}

// ==================== ElectricCar (Item 12) ====================

ElectricCar::ElectricCar(const std::string& brand, const std::string& model, int horsePower,
                         const int* history, int count, int batteryCapacity, int rangeKm)
    : Car(brand, model, horsePower, history, count),
      batteryCapacity(batteryCapacity),
      rangeKm(rangeKm) {
    std::cout << "  [Constructor ElectricCar] Baterie: " << this->batteryCapacity
              << " kWh, Autonomie: " << this->rangeKm << " km\n";
}

ElectricCar::~ElectricCar() {
    std::cout << "  [Destructor ElectricCar] Distrugere parte derivata pentru \""
              << brand << " " << model << "\"\n";
}

// Item 12: Copy Constructor in clasa derivata
ElectricCar::ElectricCar(const ElectricCar& other)
    : Car(other), // Copiem partea din clasa de baza
      batteryCapacity(other.batteryCapacity),
      rangeKm(other.rangeKm) {
    std::cout << "  [Copy Constructor ElectricCar] Copiata complet masina electrica \""
              << brand << " " << model << "\"\n";
}

// Item 10, 11, 12: Copy Assignment in clasa derivata
ElectricCar& ElectricCar::operator=(const ElectricCar& other) {
    if (this == &other) { // Item 11
        std::cout << "  [operator= ElectricCar] Self-assignment detectat -> ignorat!\n";
        return *this;
    }

    // Item 12: Apelam explicit operator= din clasa de baza pentru a copia membrii mosteniti!
    Car::operator=(other);

    // Copiem membrii specifici clasei derivate
    batteryCapacity = other.batteryCapacity;
    rangeKm = other.rangeKm;

    std::cout << "  [operator= ElectricCar] Atribuite si datele electrice (Baterie: "
              << batteryCapacity << " kWh, Autonomie: " << rangeKm << " km)\n";

    return *this; // Item 10
}

void ElectricCar::afisare() const {
    std::cout << "  ElectricCar [Brand: \"" << brand
              << "\" | Model: \"" << model
              << "\" | Putere: " << horsePower << " HP"
              << " | Baterie: " << batteryCapacity << " kWh"
              << " | Autonomie: " << rangeKm << " km"
              << " | Revizii (" << serviceCount << "): ";
    if (serviceHistory != nullptr && serviceCount > 0) {
        for (int i = 0; i < serviceCount; ++i) {
            std::cout << serviceHistory[i] << (i + 1 < serviceCount ? ", " : "");
        }
    } else {
        std::cout << "fara istoric";
    }
    std::cout << "]\n";
}
