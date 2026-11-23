#include "ServerConnection.hpp"
#include <iostream>
#include <memory>


// ITEM 13: Varianta GRESITA - Gestionarea manuala a resursei cu raw pointer
void procesareConexiuneGresit(bool eroareRetea) {
    ServerConnection* conn = createConnection(404, "192.168.1.10", 8080);
    conn->sendData("HELLO_SERVER");

    if (eroareRetea) {
        std::cout << "  [Gresit - Raw Pointer] Eroare de retea! Iesire prematura cu 'return' inainte de 'delete conn' -> MEMORY LEAK!\n";
        return; // Aici pierdem referinta la conexiune 
    }

    delete conn; // Nu se ajunge niciodata aici daca eroareRetea == true!
}

// ITEM 13: Varianta CORECTA - Folosirea obiectelor RAII (Smart Pointers)
void procesareConexiuneCorectUnique(bool eroareRetea) {
    // Regula 1 (Item 13): Punem resursa imediat intr-un obiect de administrare (RAII)
    std::unique_ptr<ServerConnection> conn(createConnection(101, "10.0.0.1", 443));
    conn->sendData("AUTH_TOKEN_SECURE");

    if (eroareRetea) {
        std::cout << "  [Corect - std::unique_ptr] Eroare de retea! Iesim din functie, dar unique_ptr va distruge automat conexiunea:\n";
        return; // Regula 2 (Item 13): Destructorul lui unique_ptr apeleaza automat delete!
    }
}

int main() {
    std::cout << "=== ITEM 13: Use objects to manage resources ===\n\n";

    // 1. Gresit: Raw pointer cu iesire prematura (Memory Leak)
    std::cout << "--- 1. Exemplu GRESIT (Raw pointer + return prematur = Memory Leak) ---\n";
    procesareConexiuneGresit(true);

    // 2. Corect: RAII cu std::unique_ptr (inlocuitorul modern pentru std::auto_ptr din carte)
    std::cout << "--- 2. Exemplu CORECT cu std::unique_ptr (RAII la iesire prematura) ---\n";
    procesareConexiuneCorectUnique(true);

    // 3. Corect: RAII cu std::shared_ptr (Reference-Counting Smart Pointer - RCSP)
    std::cout << "\n--- 3. Exemplu CORECT cu std::shared_ptr (Reference-Counting Smart Pointer) ---\n";
    {
        std::shared_ptr<ServerConnection> sp1(createConnection(202, "db.internal.ro", 5432));
        std::cout << "  [shared_ptr] sp1 creat | refCount = " << sp1.use_count() << "\n";
        {
            std::shared_ptr<ServerConnection> sp2 = sp1; // copiere permisa -> creste contorul de referinte
            std::shared_ptr<ServerConnection> sp3;
            sp3 = sp2;                                   // atribuire permisa -> toti 3 impart aceeasi conexiune
            std::cout << "  [shared_ptr] In blocul intern (sp1, sp2, sp3) | refCount = " << sp1.use_count() << "\n";
            sp3->afisare();
        }
        std::cout << "  [shared_ptr] Dupa iesirea din blocul intern (sp2, sp3 distruse) | refCount = " << sp1.use_count() << "\n";
    } // Aici moare si sp1 -> refCount ajunge la 0 -> conexiunea este distrusa automat!

    // Nota Item 13 despre array-uri pe HEAP (new[] vs delete[])
    std::cout << "\n--- 4. Nota Item 13: Gestionarea tablourilor alocate dinamic (unique_ptr<int[]>) ---\n";
    {
        std::unique_ptr<int[]> porturiActive(new int[3]{80, 443, 8080});
        std::cout << "  [unique_ptr<int[]>] Porturi gestionate automat (apeleaza delete[] la final): "
                  << porturiActive[0] << ", " << porturiActive[1] << ", " << porturiActive[2] << "\n";
    }

    std::cout << "\n=== ITEM 14: Think carefully about copying behavior in resource-managing classes ===\n";

    ConnectionMutex dbMutex("DB_Critical_Mutex");

    // Strategia 1: Interzicerea copierii (Prohibit copying)
    std::cout << "--- Strategia 1: Interzicerea copierii (Prohibit copying - clasa ExclusiveLock) ---\n";
    {
        ExclusiveLock lock1(&dbMutex);
        // ExclusiveLock lock2 = lock1; // EROARE de compilare: copy constructor este '= delete'!
        std::cout << "  [ExclusiveLock] In sectiunea critica (isLocked = " << (dbMutex.isLocked ? "true" : "false") << ")\n";
    } // La iesirea din scope, destructorul ExclusiveLock apeleaza automat unlock(&dbMutex)!
    std::cout << "  [ExclusiveLock] Dupa iesirea din scope (isLocked = " << (dbMutex.isLocked ? "true" : "false") << ")\n\n";

    // Strategia 2: Contorizarea referintelor cu Custom Deleter (Reference-count the underlying resource)
    std::cout << "--- Strategia 2: Reference Counting cu Custom Deleter (SharedLock + shared_ptr<Mutex>(pm, unlock)) ---\n";
    {
        SharedLock sLock1(&dbMutex);
        std::cout << "  [SharedLock] sLock1 activ | useCount = " << sLock1.useCount() << "\n";
        {
            SharedLock sLock2 = sLock1; // Copiem lacatul -> ambele mentin mutex-ul blocat!
            std::cout << "  [SharedLock] sLock2 copiat din sLock1 | useCount = " << sLock1.useCount() << "\n";
        }
        std::cout << "  [SharedLock] sLock2 a iesit din scope, dar mutex-ul ramane BLOCAT (useCount = "
                  << sLock1.useCount() << ", isLocked = " << (dbMutex.isLocked ? "true" : "false") << ")\n";
    } // Aici useCount devine 0 -> shared_ptr apeleaza automat custom deleter-ul 'unlock(&dbMutex)' (NU delete!)
    std::cout << "  [SharedLock] Toate copiile au iesit din scope (isLocked = " << (dbMutex.isLocked ? "true" : "false") << ")\n\n";

    // Strategia 3: Copierea efectiva a resursei (Copy the underlying resource - Deep Copy)
    std::cout << "--- Strategia 3: Copierea resursei subiacente (Deep Copy - PacketBufferManager) ---\n";
    {
        int pacheteInitiale[] = {1001, 1002, 1003};
        PacketBufferManager buf1("Buffer_Principal", pacheteInitiale, 3);
        PacketBufferManager buf2 = buf1; // Deep copy: aloca un nou vector pe HEAP si copiaza pachetele
        buf2.setPacket(0, 9999);         // Modificam copia fara sa afectam originalul
        buf1.afisare();
        buf2.afisare();
    }

    // Strategia 4: Transferul proprietatii asupra resursei (Transfer ownership)
    std::cout << "\n--- Strategia 4: Transferul proprietatii (Transfer Ownership - ExclusiveConnectionOwner) ---\n";
    {
        ExclusiveConnectionOwner owner1(createConnection(303, "api.cloud.com", 9000));
        owner1.afisare();

        std::cout << "  -> Transferam proprietatea resursei din owner1 in owner2:\n";
        ExclusiveConnectionOwner owner2 = std::move(owner1);
        owner1.afisare(); // owner1 a devenit nullptr
        owner2.afisare(); // owner2 detine acum conexiunea #303 si o va elibera la final
    }

    return 0;
}
