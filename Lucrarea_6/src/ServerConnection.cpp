#include "ServerConnection.hpp"
#include <iostream>

// ==================== ServerConnection ====================

ServerConnection::ServerConnection(int id, const std::string& host, int port)
    : connectionId(id), host(host), port(port), isConnected(true) {
    std::cout << "  [Constructor ServerConnection] Deschisa conexiunea #" << connectionId
              << " catre " << host << ":" << port << "\n";
}

ServerConnection::~ServerConnection() {
    isConnected = false;
    std::cout << "  [Destructor ServerConnection] Inchisa si eliberata conexiunea #" << connectionId
              << " (" << host << ":" << port << ")\n";
}

void ServerConnection::sendData(const std::string& data) const {
    if (isConnected) {
        std::cout << "  [ServerConnection #" << connectionId << "] Trimitere pachet: \"" << data << "\"\n";
    }
}

void ServerConnection::afisare() const {
    std::cout << "  ServerConnection [ID: " << connectionId
              << " | Host: \"" << host << ":" << port
              << "\" | Status: " << (isConnected ? "CONECTAT" : "DECONECTAT") << "]\n";
}

int ServerConnection::getId() const { return connectionId; }
std::string ServerConnection::getHost() const { return host; }
int ServerConnection::getPort() const { return port; }

// Item 13: Factory function (returneaza un pointer brut catre o resursa nou alocata pe HEAP)
ServerConnection* createConnection(int id, const std::string& host, int port) {
    return new ServerConnection(id, host, port);
}

// ==================== Mutex C-style API (Item 14) ====================

void lock(ConnectionMutex* pm) {
    if (pm != nullptr) {
        pm->isLocked = true;
        std::cout << "  [LOCK Mutex] Mutex-ul \"" << pm->name << "\" a fost BLOCAT (isLocked = true)\n";
    }
}

void unlock(ConnectionMutex* pm) {
    if (pm != nullptr) {
        pm->isLocked = false;
        std::cout << "  [UNLOCK Mutex] Mutex-ul \"" << pm->name << "\" a fost DEBLOCAT (isLocked = false)\n";
    }
}

// ==================== Item 14: Strategia 1 - ExclusiveLock (Prohibit copying) ====================

ExclusiveLock::ExclusiveLock(ConnectionMutex* pm) : mutexPtr(pm) {
    lock(mutexPtr); // RAII: blocam resursa in constructor
}

ExclusiveLock::~ExclusiveLock() {
    unlock(mutexPtr); // RAII: eliberam/deblocam automat resursa in destructor
}

// ==================== Item 14: Strategia 2 - SharedLock (Reference counting + custom deleter) ====================

SharedLock::SharedLock(ConnectionMutex* pm)
    : mutexPtr(pm, unlock) { // Pasam 'unlock' drept custom deleter pentru std::shared_ptr!
    lock(mutexPtr.get());
}

long SharedLock::useCount() const {
    return mutexPtr.use_count();
}

// ==================== Item 14: Strategia 3 - PacketBufferManager (Deep Copy) ====================

PacketBufferManager::PacketBufferManager(const std::string& name, const int* packets, int count)
    : bufferName(name), packetIds(nullptr), count(count > 0 ? count : 0) {
    if (this->count > 0 && packets != nullptr) {
        packetIds = new int[this->count];
        for (int i = 0; i < this->count; ++i) {
            packetIds[i] = packets[i];
        }
    }
    std::cout << "  [Constructor PacketBuffer] Creat buffer \"" << bufferName
              << "\" | HEAP: " << static_cast<const void*>(packetIds) << "\n";
}

PacketBufferManager::~PacketBufferManager() {
    std::cout << "  [Destructor PacketBuffer] Eliberare buffer \"" << bufferName
              << "\" | HEAP: " << static_cast<const void*>(packetIds) << "\n";
    delete[] packetIds;
}

PacketBufferManager::PacketBufferManager(const PacketBufferManager& other)
    : bufferName(other.bufferName + " (Copie)"),
      packetIds(nullptr),
      count(other.count) {
    if (count > 0 && other.packetIds != nullptr) {
        packetIds = new int[count];
        for (int i = 0; i < count; ++i) {
            packetIds[i] = other.packetIds[i];
        }
    }
    std::cout << "  [Deep Copy PacketBuffer] Copiata resursa din \"" << other.bufferName
              << "\" la o noua adresa HEAP: " << static_cast<const void*>(packetIds) << "\n";
}

PacketBufferManager& PacketBufferManager::operator=(const PacketBufferManager& other) {
    if (this == &other) {
        return *this;
    }

    int* newPackets = nullptr;
    if (other.count > 0 && other.packetIds != nullptr) {
        newPackets = new int[other.count];
        for (int i = 0; i < other.count; ++i) {
            newPackets[i] = other.packetIds[i];
        }
    }

    delete[] packetIds;
    bufferName = other.bufferName + " (Atribuit)";
    packetIds = newPackets;
    count = other.count;
    return *this;
}

void PacketBufferManager::setPacket(int index, int val) {
    if (index >= 0 && index < count && packetIds != nullptr) {
        packetIds[index] = val;
    }
}

void PacketBufferManager::afisare() const {
    std::cout << "  PacketBuffer [Nume: \"" << bufferName
              << "\" | HEAP: " << static_cast<const void*>(packetIds)
              << " | Pachete (" << count << "): ";
    for (int i = 0; i < count; ++i) {
        std::cout << packetIds[i] << (i + 1 < count ? ", " : "");
    }
    std::cout << "]\n";
}

// ==================== Item 14: Strategia 4 - ExclusiveConnectionOwner (Transfer ownership) ====================

ExclusiveConnectionOwner::ExclusiveConnectionOwner(ServerConnection* conn)
    : connPtr(conn) {
    std::cout << "  [Constructor ExclusiveOwner] A preluat controlul conexiunii #"
              << (connPtr ? connPtr->getId() : -1) << "\n";
}

ExclusiveConnectionOwner::~ExclusiveConnectionOwner() {
    if (connPtr != nullptr) {
        std::cout << "  [Destructor ExclusiveOwner] Elibereaza conexiunea detinuta #"
                  << connPtr->getId() << "\n";
        delete connPtr;
    } else {
        std::cout << "  [Destructor ExclusiveOwner] Obiect gol (nullptr - proprietatea a fost deja transferata)\n";
    }
}

ExclusiveConnectionOwner::ExclusiveConnectionOwner(ExclusiveConnectionOwner&& other) noexcept
    : connPtr(other.connPtr) {
    other.connPtr = nullptr; // sursa cedeaza proprietatea resursei!
    std::cout << "  [Transfer Ownership] Proprietatea conexiunii #"
              << (connPtr ? connPtr->getId() : -1) << " a fost mutata (sursa a devenit nullptr)\n";
}

ExclusiveConnectionOwner& ExclusiveConnectionOwner::operator=(ExclusiveConnectionOwner&& other) noexcept {
    if (this != &other) {
        delete connPtr; // eliberam vechea resursa detinuta
        connPtr = other.connPtr;
        other.connPtr = nullptr;
    }
    return *this;
}

void ExclusiveConnectionOwner::afisare() const {
    if (connPtr != nullptr) {
        std::cout << "  ExclusiveOwner -> detine ";
        connPtr->afisare();
    } else {
        std::cout << "  ExclusiveOwner -> [nullptr - nu mai detine nicio conexiune]\n";
    }
}
