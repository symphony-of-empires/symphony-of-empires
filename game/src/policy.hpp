// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      policy.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "objects.hpp"

enum AllowancePolicy {
    ALLOW_NOBODY, // Nobody can enter the country
    ALLOW_ACCEPTED_LANGUAGES, // Only accepted languages can enter the country
    ALLOW_ALL_PAYMENT, // Everyone can enter, but at a cost
    ALLOW_ALL, // Everyone can exit freely
};
template<>
struct Serializer<enum AllowancePolicy> : SerializerMemcpy<enum AllowancePolicy> {};

enum CensorshipPolicy {
    CENSORSHIP_ALL_CENSORED, // All media censored
    CENSORSHIP_ONLY_STATE, // Only state run media allowed
    CENSORSHIP_ONLY_APPROVED, // Only "approved" media is allowed
    CENSORSHIP_ALL_ALLOWED, // All media allowed
};
template<>
struct Serializer<enum CensorshipPolicy> : SerializerMemcpy<enum CensorshipPolicy> {};

enum AutoBuildPolicy {
    AUTO_BUILD_NONE, // POPs cannot auto build stuff
    AUTO_BUILD_ONLY_APPROVED, // POPs can only build with approval
    AUTO_BUILD_ALLOWED, // All POPs can build freely
};
template<>
struct Serializer<enum AutoBuildPolicy> : SerializerMemcpy<enum AutoBuildPolicy> {};

enum TreatmentPolicy {
    TREATMENT_EVERYONE_EQUAL, // Everyone is equal
    TREATMENT_ONLY_ACCEPTED, // Only accepted POPs are treated
    TREATMENT_ENSLAVED, // Non-accepted are enslaved
    TREATMENT_EXTERMINATE, // Exterminate
};
template<>
struct Serializer<enum TreatmentPolicy> : SerializerMemcpy<enum TreatmentPolicy> {};

struct Policies {
    TreatmentPolicy treatment;
    AllowancePolicy migration = ALLOW_ALL;
    AllowancePolicy immigration = ALLOW_ALL;
    CensorshipPolicy censorship;
    AutoBuildPolicy build_infrastructure; // Are POPs able to build infrastructure
    AutoBuildPolicy build_factories; // Are POPs able to build factories
    bool private_property = true;
    bool companies_allowed = true;
    bool public_education = true;
    bool secular_education = true;
    bool public_healthcare = true;
    bool social_security = true; // Goverment pays bought food
    bool slavery = true; // Is slavery allowed?
    bool legislative_parliament = true; // Is there a parliament that is needed to approve laws?
    bool executive_parliament = true; // Is there a parliament that is needed to do executive actions?
    bool constitutional = true; // Is there a constitution to limit the power of the leader?
    bool foreign_trade = true; // Do we allow foreign trade?
    float import_tax = 0.1f; // Tax % for importing products
    float export_tax = 0.1f; // Tax % for exporting products
    float poor_flat_tax = 0.1f; // Flat tax rhs on the low-wage pops
    float med_flat_tax = 0.1f; // Flat tax rhs on the medium-wage pops
    float rich_flat_tax = 0.1f; // Flat tax rhs on the high-wage pops
    float industry_tax = 0.1f; // The tax given to the industrial sector
    float military_spending = 0.1f; // Spending done on military (% of budget)
    // Units can grab anything they want from DOMESTIC provinces
    // Please notice the DOMESTIC part of this, for foreign "free grab"; check diplomatic relations!!
    bool free_supplies = true;
    float min_wage = 1.f; // Minimum wage
    float min_sv_for_parliament = 2.f; // Minimum social value for people to be accounted on the parliament

    inline int difference(const Policies& rhs) const {
        int diff = 0;

        diff += std::abs(static_cast<int>(rhs.migration) - static_cast<int>(this->migration));
        diff += std::abs(static_cast<int>(rhs.immigration) - static_cast<int>(this->immigration));
        diff += std::abs(static_cast<int>(rhs.censorship) - static_cast<int>(this->censorship));
        diff += std::abs(static_cast<int>(rhs.build_factories) - static_cast<int>(this->build_factories));
        diff += std::abs(rhs.build_infrastructure - this->build_infrastructure);
        diff += (rhs.private_property != this->private_property) ? 1 : 0;
        diff += (rhs.companies_allowed != this->companies_allowed) ? 1 : 0;
        diff += (rhs.public_education != this->public_education) ? 1 : 0;
        diff += (rhs.secular_education != this->secular_education) ? 1 : 0;
        diff += (rhs.public_healthcare != this->public_healthcare) ? 1 : 0;
        diff += (rhs.social_security != this->social_security) ? 1 : 0;
        diff += (rhs.slavery != this->slavery) ? 1 : 0;
        diff += (rhs.import_tax != this->import_tax) ? 1 : 0;
        diff += (rhs.export_tax != this->export_tax) ? 1 : 0;
        diff += (rhs.poor_flat_tax != this->poor_flat_tax) ? 1 : 0;
        diff += (rhs.med_flat_tax != this->med_flat_tax) ? 1 : 0;
        diff += (rhs.rich_flat_tax != this->rich_flat_tax) ? 1 : 0;
        diff += (rhs.free_supplies != this->free_supplies) ? 1 : 0;
        diff += std::abs(rhs.min_wage - this->min_wage);
        diff += std::abs(rhs.min_sv_for_parliament - this->min_sv_for_parliament);
        return diff;
    };
};
template<>
struct Serializer<Policies> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Policies& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.free_supplies);
        ::deser_dynamic<is_serialize>(ar, obj.immigration);
        ::deser_dynamic<is_serialize>(ar, obj.import_tax);
        ::deser_dynamic<is_serialize>(ar, obj.industry_tax);
        ::deser_dynamic<is_serialize>(ar, obj.legislative_parliament);
        ::deser_dynamic<is_serialize>(ar, obj.med_flat_tax);
        ::deser_dynamic<is_serialize>(ar, obj.migration);
        ::deser_dynamic<is_serialize>(ar, obj.military_spending);
        ::deser_dynamic<is_serialize>(ar, obj.poor_flat_tax);
        ::deser_dynamic<is_serialize>(ar, obj.private_property);
        ::deser_dynamic<is_serialize>(ar, obj.public_education);
        ::deser_dynamic<is_serialize>(ar, obj.public_healthcare);
        ::deser_dynamic<is_serialize>(ar, obj.rich_flat_tax);
        ::deser_dynamic<is_serialize>(ar, obj.secular_education);
        ::deser_dynamic<is_serialize>(ar, obj.slavery);
        ::deser_dynamic<is_serialize>(ar, obj.social_security);
        ::deser_dynamic<is_serialize>(ar, obj.treatment);
        ::deser_dynamic<is_serialize>(ar, obj.min_wage);
        ::deser_dynamic<is_serialize>(ar, obj.min_sv_for_parliament);
    }
};
