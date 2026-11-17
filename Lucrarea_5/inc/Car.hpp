#ifndef CAR_HPP
#define CAR_HPP

#include <string>

class Car {
protected:
    std::string brand;
    std::string model;
    int horsePower;
    int* serviceHistory; // ani de revizie alocati pe HEAP
    int serviceCount;

public:
    Car(const std::string& brand, const std::string& model, int horsePower, const int* history = nullptr, int count = 0);

    virtual ~Car();

    // Copy Constructor
    Car(const Car& other);

    // Item 10 & Item 11: Copy Assignment Operator
    Car& operator=(const Car& other);

    // Item 10: se aplica si pentru operator+=, -=, *=, etc.
    Car& operator+=(int extraHp);

    virtual void afisare() const;
};

// Item 12: Clasa derivata pentru a demonstra copierea completa (inclusiv partea din clasa de baza)
class ElectricCar : public Car {
private:
    int batteryCapacity; // kWh
    int rangeKm;         // autonomie in km

public:
    ElectricCar(const std::string& brand, const std::string& model, int horsePower,
                const int* history, int count, int batteryCapacity, int rangeKm);

    ~ElectricCar() override;

    // Item 12: Copy Constructor care apeleaza si Copy Constructor-ul clasei de baza Car
    ElectricCar(const ElectricCar& other);

    // Item 10, 11, 12: operator= care returneaza *this, verifica self-assignment si apeleaza Car::operator=
    ElectricCar& operator=(const ElectricCar& other);

    void afisare() const override;
};

#endif
