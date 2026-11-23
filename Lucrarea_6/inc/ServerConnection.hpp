#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include <string>
#include <memory>

// Resursa de baza (ex: o conexiune la un server / baza de date)
class ServerConnection {
private:
    int connectionId;
    std::string host;
    int port;
    bool isConnected;

public:
    ServerConnection(int id, const std::string& host, int port);
    ~ServerConnection();

    void sendData(const std::string& data) const;
    void afisare() const;

    int getId() const;
    std::string getHost() const;
    int getPort() const;
};

// Item 13: Functie factory care aloca o resursa pe HEAP (ca "createInvestment()" din Effective C++)
ServerConnection* createConnection(int id, const std::string& host, int port);

// ============================================================================
// ITEM 14: Resursa de tip Mutex (sincronizare) si clase RAII care o gestioneaza
// ============================================================================
struct ConnectionMutex {
    std::string name;
    bool isLocked;

    ConnectionMutex(const std::string& n) : name(n), isLocked(false) {}
};

void lock(ConnectionMutex* pm);
void unlock(ConnectionMutex* pm);

// Item 14 - Strategia 1: Interzicerea copierii (Prohibit copying)
// Are sens cand o clasa RAII gestioneaza un Mutex pe durata unui bloc (scoped lock)
class ExclusiveLock {
private:
    ConnectionMutex* mutexPtr;

public:
    explicit ExclusiveLock(ConnectionMutex* pm);
    ~ExclusiveLock();

    // Interzicem copierea (nu are sens sa ai 2 "lacate" identice pe acelasi thread/scope)
    ExclusiveLock(const ExclusiveLock&) = delete;
    ExclusiveLock& operator=(const ExclusiveLock&) = delete;
};

// Item 14 - Strategia 2: Reference-count the underlying resource (cu custom deleter)
// Folosim std::shared_ptr<ConnectionMutex> cu functia custom 'unlock' ca deleter!
// Astfel, mutex-ul NU este sters cu 'delete' la final, ci doar deblocat cu 'unlock' cand refCount ajunge la 0.
class SharedLock {
private:
    std::shared_ptr<ConnectionMutex> mutexPtr;

public:
    explicit SharedLock(ConnectionMutex* pm);
    // Nu avem nevoie de destructor custom sau copy constructor custom:
    // shared_ptr incrementeaza automat contorul la copiere si apeleaza unlock(pm) cand contorul devine 0!
    long useCount() const;
};

// Item 14 - Strategia 3: Copy the underlying resource (Deep Copy)
// Clasa RAII care detine un buffer de pachete pe HEAP si face deep copy la copiere
class PacketBufferManager {
private:
    std::string bufferName;
    int* packetIds;
    int count;

public:
    PacketBufferManager(const std::string& name, const int* packets, int count);
    ~PacketBufferManager();

    // Deep copy: copiem si resursa gestionata de pe HEAP
    PacketBufferManager(const PacketBufferManager& other);
    PacketBufferManager& operator=(const PacketBufferManager& other);

    void setPacket(int index, int val);
    void afisare() const;
};

// Item 14 - Strategia 4: Transfer ownership of the underlying resource
// La transferul de proprietate, doar un singur obiect detine resursa la un moment dat (ca unique_ptr / move)
class ExclusiveConnectionOwner {
private:
    ServerConnection* connPtr;

public:
    explicit ExclusiveConnectionOwner(ServerConnection* conn);
    ~ExclusiveConnectionOwner();

    // Interzicem copierea obisnuita si permitem transferul explicit al resursei (move)
    ExclusiveConnectionOwner(const ExclusiveConnectionOwner&) = delete;
    ExclusiveConnectionOwner& operator=(const ExclusiveConnectionOwner&) = delete;

    ExclusiveConnectionOwner(ExclusiveConnectionOwner&& other) noexcept;
    ExclusiveConnectionOwner& operator=(ExclusiveConnectionOwner&& other) noexcept;

    void afisare() const;
};

#endif
