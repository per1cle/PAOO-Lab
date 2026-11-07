#ifndef BOOK_HPP
#define BOOK_HPP

#include <string>

class Book {
private:
    const int id;
    std::string title;
    std::string author;
    int pages;

public:
    Book(int id, const std::string& title, const std::string& author, int pages);

    Book(const Book& other);
    
    Book& operator=(const Book& other);

    ~Book();

    void afisare() const;
};

#endif
