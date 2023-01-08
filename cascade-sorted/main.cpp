#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>


struct Entry
{
    std::string name;
    std::string lastName;
    std::string patronymic;
};

template<typename CharType, class Traits>
std::basic_ostream<CharType, Traits>&
operator<<(std::basic_ostream<CharType, Traits>& stream, const Entry& entry)
{
    std::basic_ostringstream<CharType, Traits> builder;
    builder.flags(stream.flags());
    builder.imbue(stream.getloc());
    builder.precision(stream.precision());
    builder << '('
        << entry.name << ',' << entry.lastName << ',' << entry.patronymic << ')';
    return stream << builder.str();
}


int main()
{
    std::vector<Entry> collection =
    {
        Entry{ "Sarah", "Connor" },
        Entry{ "Reese", "Kyle" },
        Entry{ "John", "Connor", "Reese" },
        Entry{ "James", "Cameron", "Francis" },
        Entry{ "Arnold", "Schwarzenegger", "Alois" },
    };

    std::cout << "\nInitial data:" << std::endl;
    for (const Entry& entry : collection)
        std::cout << '\t' << entry << std::endl;

    std::sort(collection.begin(), collection.end(),
        [](const Entry& lhs, const Entry& rhs) noexcept
        {
            const int lastNameOrder = lhs.lastName.compare(rhs.lastName);
            if (lastNameOrder < 0)
                return true;
            if (lastNameOrder > 0)
                return false;
            return lhs.name < rhs.name;
        });

    std::cout << "\nResult data:" << std::endl;
    for (const Entry& entry : collection)
        std::cout << '\t' << entry << std::endl;

    std::string ignore;
    std::getline(std::cin, ignore);

    return EXIT_SUCCESS;
}
