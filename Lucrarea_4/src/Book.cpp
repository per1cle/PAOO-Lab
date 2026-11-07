#include "Book.hpp"
#include <iostream>

// Item 4: Member Initialization List
// toate variabilele sunt initializate direct ( assignement != initialization )
// initializam membrii in exact aceeasi ordine in care sunt declarate in Book.hpp.
Book::Book(int id, const std::string& title, const std::string& author, int pages)
    : id(id),
      title(title),
      author(author),
      pages(pages) {
    std::cout << "  [Constructor Book - Init List] Creata cartea \"" << this->title << "\" (ID: " << this->id << ")\n";
}

Book::~Book() {
    std::cout << "  [Destructor Book] Distrugere carte \"" << title << "\" (ID: " << id << ")\n";
}

// Item 5: Copy Constructor
Book::Book(const Book& other)
    : id(other.id),
      title(other.title),
      author(other.author),
      pages(other.pages) {
    std::cout << "  [Copy Constructor Book] Copiata cartea \"" << title << "\" (ID: " << id << ")\n";
}

// Item 5: Copy Assignment Operator (operator=)
// compilatorul nu il poate genera din cauza membrului 'const int id', deci Il scriem noi
Book& Book::operator=(const Book& other) {
    if (this != &other) {
        title = other.title;
        author = other.author;
        pages = other.pages;
    }
    std::cout << "  [Copy Assignment operator= Book] Atribuire continut din \"" << other.title
              << "\" in cartea cu ID: " << id << "\n";
    return *this;
}

void Book::afisare() const {
    std::cout << "  Book [ID: " << id
              << " | Titlu: \"" << title
              << "\" | Autor: \"" << author
              << "\" | Pagini: " << pages << "]\n";
}
