#include "Car.hpp"
#include <iostream>
#include <string>

// ============================================================================
// ITEM 10: Varianta GRESITA - operator= returneaza void (nu permite inlantuirea)
// ============================================================================
class BadReturnCar {
public:
    std::string model;
    int hp;

    BadReturnCar(const std::string& m, int h) : model(m), hp(h) {}

    // Gresit: returneaza void in loc de BadReturnCar&
    void operator=(const BadReturnCar& other) {
        model = other.model;
        hp = other.hp;
        std::cout << "  [BadReturnCar operator= (void)] Atribuit \"" << model << "\", dar nu se poate face c1 = c2 = c3!\n";
    }
};

// ============================================================================
// ITEM 11: Varianta GRESITA - operator= FARA verificare de self-assignment
// Daca facem c = c, isi sterge propriul pointer inainte sa il copieze!
// ============================================================================
class UnsafeSelfCar {
public:
    std::string model;
    int* serviceYear;

    UnsafeSelfCar(const std::string& m, int year) : model(m) {
        serviceYear = new int(year);
    }

    ~UnsafeSelfCar() {
        delete serviceYear;
    }

    UnsafeSelfCar& operator=(const UnsafeSelfCar& other) {
        // GRESIT: Nu verificam if (this == &other)!
        // Daca this == &other, delete serviceYear sterge si other.serviceYear!
        delete serviceYear;
        // Aici other.serviceYear este deja eliberat (dangling pointer / undefined behavior)!
        serviceYear = new int(*other.serviceYear);
        model = other.model;
        return *this;
    }

    void afisare() const {
        std::cout << "  [UnsafeSelfCar] Model: \"" << model
                  << "\" | An revizie la adresa " << static_cast<const void*>(serviceYear)
                  << ": " << *serviceYear << "\n";
    }
};

// ============================================================================
// ITEM 12: Varianta GRESITA 1 - Uitam sa copiem un membru nou adaugat in clasa
// ============================================================================
class IncompleteCopyCar {
public:
    std::string model;
    int hp;
    int mileageKm; // Membru adaugat ulterior, uitat in copy constructor si operator=

    IncompleteCopyCar(const std::string& m, int h, int km)
        : model(m), hp(h), mileageKm(km) {}

    // GRESIT: Copiaza model si hp, dar uita de mileageKm!
    IncompleteCopyCar(const IncompleteCopyCar& other)
        : model(other.model), hp(other.hp) /* mileageKm ramane neinitializat! */ {
        std::cout << "  [Gresit - IncompleteCopyCar] Copy constructor a uitat sa copieze mileageKm!\n";
    }

    IncompleteCopyCar& operator=(const IncompleteCopyCar& other) {
        if (this != &other) {
            model = other.model;
            hp = other.hp;
            // GRESIT: am uitat mileageKm = other.mileageKm;
        }
        return *this;
    }

    void afisare() const {
        std::cout << "  IncompleteCopyCar [Model: \"" << model
                  << "\" | HP: " << hp
                  << " | Kilometraj: " << mileageKm << " km]\n";
    }
};

// ============================================================================
// ITEM 12: Varianta GRESITA 2 - Clasa derivata care uita sa copieze clasa de baza
// ============================================================================
class BadDerivedCar : public Car {
private:
    int batteryKwh;

public:
    BadDerivedCar(const std::string& b, const std::string& m, int hp, int bat)
        : Car(b, m, hp, nullptr, 0), batteryKwh(bat) {}

    // GRESIT: Nu apeleaza Car::operator=(other), deci brand, model si hp NU se copiaza!
    BadDerivedCar& operator=(const BadDerivedCar& other) {
        if (this != &other) {
            batteryKwh = other.batteryKwh;
            std::cout << "  [Gresit - BadDerivedCar operator=] S-a copiat DOAR batteryKwh ("
                      << batteryKwh << " kWh), dar partea de baza Car a ramas nemodificata!\n";
        }
        return *this;
    }

    void afisare() const override {
        std::cout << "  BadDerivedCar [Brand: \"" << brand
                  << "\" | Model: \"" << model
                  << "\" | HP: " << horsePower
                  << " | Baterie: " << batteryKwh << " kWh]\n";
    }
};

int main() {
    std::cout << "=== ITEM 10: Have assignment operators return a reference to *this ===\n\n";

    // Gresit: operator= returneaza void -> nu putem face atribuire inlantuita (c1 = c2 = c3)
    BadReturnCar bc1("Logan", 90);
    BadReturnCar bc2("Sandero", 100);
    bc2 = bc1;
    // BadReturnCar bc3("Duster", 130);
    // bc3 = bc2 = bc1; // EROARE de compilare: operator= returneaza void!

    // Corect: clasa Car returneaza Car& (*this), permitand inlantuirea ca la tipurile int (x = y = z = 15)
    int revizii1[] = {2021, 2022, 2023};
    int revizii2[] = {2020};
    Car c1("Dacia", "Duster", 150, revizii1, 3);
    Car c2("Ford", "Focus", 125, revizii2, 1);
    Car c3("Mazda", "3", 186, nullptr, 0);

    std::cout << "\n  -> Executam atribuirea inlantuita: c3 = c2 = c1;\n";
    c3 = c2 = c1; // se evalueaza c3 = (c2 = c1)

    std::cout << "  -> Executam si operator+= inlantuit: (c1 += 20) += 10;\n";
    (c1 += 20) += 10;
    c1.afisare();
    c2.afisare();
    c3.afisare();

    std::cout << "\n=== ITEM 11: Handle assignment to self in operator= ===\n\n";

    // Gresit: self-assignment fara protectie distruge propria resursa de pe HEAP
    UnsafeSelfCar usc("Golf 7", 2022);
    usc.afisare();
    // usc = usc; // Pericol! Sterge serviceYear inainte sa-l copieze (use-after-free / dangling pointer)
    std::cout << "  [Gresit - UnsafeSelfCar] Apelul 'usc = usc' ar face delete pe propriul pointer inainte de copiere!\n\n";

    // Corect: Car gestioneaza in siguranta self-assignment (direct sau prin alias/referinta/pointer)
    std::cout << "  -> Self-assignment direct (c1 = c1):\n";
    c1 = c1;

    std::cout << "  -> Self-assignment ascuns prin referinta/alias (refCar = c1):\n";
    Car& refCar = c1;
    refCar = c1; // this si &other pointeaza la acelasi obiect!
    c1.afisare();

    std::cout << "\n=== ITEM 12: Copy all parts of an object ===\n\n";

    // Gresit 1: Copiere partiala a membrilor dintr-o clasa
    IncompleteCopyCar ic1("Audi A4", 190, 85000);
    IncompleteCopyCar ic2("BMW 320", 184, 120000);
    ic2 = ic1; // copiaza model si hp, dar uita sa copieze mileageKm (ramane 120000 in loc de 85000)!
    std::cout << "  [Gresit - Membru uitat la operator=] Dupa ic2 = ic1, ic2 are kilometrajul vechi:\n";
    ic2.afisare();

    // Gresit 2: Clasa derivata care uita sa apeleze operator= din clasa de baza
    std::cout << "\n  [Gresit - Clasa derivata uita clasa de baza]:\n";
    BadDerivedCar bdc1("Tesla", "Model 3", 283, 60);
    BadDerivedCar bdc2("Renault", "Zoe", 135, 52);
    bdc2 = bdc1; // copiaza doar bateria (60 kWh), dar brandul ramane Renault Zoe!
    bdc2.afisare();

    // Corect: ElectricCar copiaza atat partea din clasa de baza Car, cat si membrii proprii
    std::cout << "\n  [Corect - ElectricCar copiaza atat baza Car, cat si membrii derivati]:\n";
    int reviziiTesla[] = {2023, 2024};
    ElectricCar ec1("Tesla", "Model S", 670, reviziiTesla, 2, 100, 634);
    ElectricCar ec2 = ec1; // Copy constructor complet (apeleaza Car(other))
    ec2.afisare();

    int reviziiTaycan[] = {2024};
    ElectricCar ec3("Porsche", "Taycan", 408, reviziiTaycan, 1, 79, 431);
    std::cout << "\n  -> Apelam ec3 = ec1 (operator= complet):\n";
    ec3 = ec1; // Apeleaza Car::operator=(other) + copiaza batteryCapacity si rangeKm
    ec3.afisare();

    std::cout << "\n=== Finalul main() - Eliberarea automata a obiectelor ===\n";
    return 0;
}
