#include "product.hpp"

// "Close" the product for this specific day, which means the equilibrium of price
// will change according to the changes in the market
void Product::close_market(void) {
    if(this->demand > this->supply) {
        this->price_vel += 0.0001f * (this->demand - this->supply);
    }
    else if(this->demand < this->supply) {
        this->price_vel -= 0.0001f * (this->supply - this->demand);
    }
    else {
        // Gravitate towards absolute zero due to volatility decay
        // (i.e, product price becomes stable without market activity)
        if(this->price_vel > 0.1f) {
            this->price_vel -= 0.001f;
        }
        else if(this->price_vel < -0.1f) {
            this->price_vel += 0.001f;
        }
        else {
            this->price_vel = -0.001f;
        }
    }

    this->price += this->price_vel;
    this->price = std::max<float>(0.01f, this->price);

    // Save prices and stuff onto history (for the charts!)
    this->demand_history.push_back(this->demand);
    if(this->demand_history.size() > 60)
        this->demand_history.pop_front();

    this->supply_history.push_back(this->supply);
    if(this->supply_history.size() > 60)
        this->supply_history.pop_front();

    this->price_history.push_back(this->price);
    if(this->price_history.size() > 60)
        this->price_history.pop_front();
}