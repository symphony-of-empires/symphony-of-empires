#ifndef PRODUCT_H
#define PRODUCT_H
#include <string>
#include "company.hpp"
#include "province.hpp"
#include "good.hpp"
#include "building.hpp"

// A product (based off a Good) which can be bought by POPs, converted by factories and transported
// accross the world
class Product : public IdEntity<uint16_t> {
public:
    void close_market(void);

    // Onwer (companyId) of this product
    Company* owner;

    // Origin province (where this product was made)
    Province* origin;

    // Building that made this product
    Building* building;

    // Good that this product is based on
    Good* good;

    // Price of the product
    float price;

    // Velocity of change of price of the product
    float price_vel;

    // Quality of the product
    float quality;

    // Total supply (worldwide) of the product
    size_t supply;

    // Total demand (worldwide) of the product
    size_t demand;

    // History of price, supply and demand for the past 30 days
    std::deque<float> price_history;
    std::deque<size_t> supply_history;
    std::deque<size_t> demand_history;
};

#endif