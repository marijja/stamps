#include <iostream>
#include <vector>
#include <regex>

using Stamp = std::tuple<int, std::string, double, std::string, std::string>;
using Query = std::pair<int, int>;

/**
 * Zczytujemy wejście programu
 */
void readInput(std::vector<std::string>& input) {
    std::string buffer;
    while (getline(std::cin, buffer)) {
        input.push_back(buffer);
    }
}

/**
 * Rzutujemy cenę ze stringa na double
 */
bool parsePrice(const std::string& priceAsString, double& priceValue) {
    std::string priceToConvert = priceAsString;

    std::replace(priceToConvert.begin(), priceToConvert.end(), ',', '.');
    try {
        priceValue = stod(priceToConvert);
        return true;
    } catch (const std::out_of_range& e) {
        return false;
    }
}

/**
 * Oczyszczamy stringa z nadmiarowych whitespace
 */
void clearStringFromRedundantWhitespaces(std::string& text) {
    static const std::regex whitespaceToRemove ("\\s+");
    static const std::regex trailing_whitespace ("[ \\t]+$");
    std::string end = "";
    std::string whitespace = " ";

    text = std::regex_replace(text, whitespaceToRemove, whitespace);
    text = std::regex_replace(text, trailing_whitespace, end);
}

/**
 * Parsujemy stringa zawierającego dane nt znaczka i tworzymy tupla przechowującego je
 */
bool parseStampData(const std::smatch& match, Stamp& newStamp) {
    std::string name = match[1];
    std::string priceAsString = match[2];
    double priceValue;
    int year = stoi(match[4]);
    std::string postOffice = match[6];

    clearStringFromRedundantWhitespaces(name);
    clearStringFromRedundantWhitespaces(postOffice);

    if (parsePrice(priceAsString, priceValue)) {
        newStamp = make_tuple(year, postOffice, priceValue, name, priceAsString);
        return true;
    } else {
        return false;
    }
}

/**
 * Sprawdzamy poprawność daty
 */
bool checkDate(int beginningDate, int endingDate) {
    return beginningDate <= endingDate;
}

/**
 * Parsujemy stringa z danymi nt zapytania i tworzymy parę przechowującą je
 */
bool parseQueryData(const std::smatch& match, Query& newQuery) {
    int beginningDate = std::stoi(match[1]);
    int endingDate = stoi(match[2]);

    if (checkDate(beginningDate, endingDate)) {
        newQuery = std::make_pair(beginningDate, endingDate);
        return true;
    } else {
        return false;
    }
}

/**
 * Dopasowujemy znaczek do wzorca regex
 */
bool matchStamp(const std::string &line, std::smatch* match) {
    static const std::regex correctLine ("\\s*(.+)\\s+(\\d+|\\d+([.]|,)\\d+)\\s+((1|2)\\d{3})\\s+(.+)\\s*");

    return regex_match(line, *match, correctLine);
}

/**
 * Dopasowujemy zapytanie do wzorca regex
 */
bool matchQuery(const std::string &line, std::smatch* match) {
    static const std::regex correctQuery ("\\s*(\\d+)\\s+(\\d+)\\s*");

    return regex_match(line, *match, correctQuery);
}

/**
 * Parsujemy dane znaczka i wrzucamy je do struktury danych
 */
bool parseStamp(const std::string line, std::vector<Stamp>& data, std::vector<Query>& queries) {
    std::smatch stampMatch;

    if (matchStamp(line, &stampMatch) && queries.empty()) {
        Stamp newStamp;
        if (parseStampData(stampMatch, newStamp)) {
            data.push_back(newStamp);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/**
 * Parsujemy zapytanie i wrzucamy je do struktury danych
 */
bool parseQuery(const std::string line, std::vector<Query>& queries) {
    std::smatch queryMatch;

    if (matchQuery(line, &queryMatch)) {
        Query newQuery;
        if (parseQueryData(queryMatch, newQuery)) {
            queries.push_back(newQuery);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/**
 * Wypisujemy błędną linijkę na standardowe wyjście błędów
 */
void erroneousLine(int lineCount, const std::string& line) {
    std::cerr << "Error in line " << lineCount << ": " << line << std::endl;
}

/**
 * Parsujemy kolejne linijki
 */
void parseInput(std::vector<std::string>& inputData, std::vector<Query>& queries, std::vector<Stamp>& data) {
    unsigned int i = 0;

    for (std::string const& currentLine: inputData) {
        if (!parseStamp(currentLine, data, queries)) {
            if (!parseQuery(currentLine, queries)) {
                erroneousLine(i + 1, currentLine);
            }
        }
        i++;
    }
}

/**
 * Wczytujemy i parsujemy wejście programu
 */
void processInput(std::vector<Query>& queries, std::vector<Stamp>& data) {
    std::vector<std::string> inputData;

    readInput(inputData);
    parseInput(inputData, queries, data);
}

/**
 * Wypisujemy dany znaczek na wyjście.
 */
void writeStamp(Stamp const& stamp) {
    std::cout << std::get<0>(stamp) << ' ' << std::get<1>(stamp) << ' ' << std::get<4>(stamp) << ' ' << std::get<3>(stamp) << '\n';
}

/**
 * Funkcja porównująca dla lower_bound
 */
bool comp_lower(Stamp& stamp, int year) {
    return std::get<0>(stamp) < year;
}

/**
 * Funkcja porównująca dla upper_bound
 */
bool comp_upper(int year, Stamp& stamp) {
    return year < std::get<0>(stamp);
}

/**
 * Przetwarzamy dane zapytanie.
 */
void processQuery(std::vector<Stamp>& data, Query& query) {
    std::vector<Stamp>::iterator iter_begin = std::lower_bound(data.begin(), data.end(), query.first, comp_lower); // znajdujemy pierwszy w przedziale
    std::vector<Stamp>::iterator iter_end = std::upper_bound(data.begin(), data.end(), query.second, comp_upper); // znajdujemy ostatni za przedziałem

    for (std::vector<Stamp>::iterator it = iter_begin; it != iter_end; ++it) {
        writeStamp(*it);
    }
}

/**
 * Przetwarzamy wszystkie zapytania.
 */
void processQueries(std::vector<Stamp>& data, std::vector<Query>& queries) {
    for (std::vector<Query>::iterator it = queries.begin(); it != queries.end(); ++it) {
        processQuery(data, *it);
    }
}

int main() {
    std::vector<Query> queries;
    std::vector<Stamp> data;

    processInput(queries, data);
    std::sort(data.begin(), data.end()); //sortujemy
    processQueries(data, queries);
    return 0;
}
