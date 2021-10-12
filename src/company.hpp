#ifndef COMPANY_HPP
#define COMPANY_HPP

#include "entity.hpp"
#include <string>
#include <set>
#include <algorithm>
#include "nation.hpp"
#include "province.hpp"

// A company that operates one or more factories and is able to build even more factories
class Company : public IdEntity<uint16_t> {
public:
    // Name of this company
    std::string name;

    // How many money this company has
    float money;

    bool is_transport;
    bool is_retailer;
    bool is_industry;

    // Nation that controls this company, essentially making it a state-controlled company (can be null)
    Nation* nation = nullptr;

    // List of province IDs where this company operates (mostly used for transport companies)
    std::set<Province*> operating_provinces;

    inline bool in_range(Province* province) {
        return (std::find(operating_provinces.begin(), operating_provinces.end(), province)
            != operating_provinces.end());
    }

    void name_gen() {
        size_t r = (rand() % 12) + 1;
        for(size_t i = 0; i < r; i++) {
            name += 'a' + (rand() % 25);
        }
    }
};

#endif