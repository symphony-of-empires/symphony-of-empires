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
//      objects.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/entity.hpp"
#include "eng3d/serializer.hpp"

struct GoodId : EntityId<uint8_t> {
    GoodId() = default;
    GoodId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<GoodId> : SerializerMemcpy<GoodId> {};

struct ProductId : EntityId<uint8_t> {
    ProductId() = default;
    ProductId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<ProductId> : SerializerMemcpy<ProductId> {};

struct LanguageId : EntityId<uint16_t> {
    LanguageId() = default;
    LanguageId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<LanguageId> : SerializerMemcpy<LanguageId> {};

struct ReligionId : EntityId<uint8_t> {
    ReligionId() = default;
    ReligionId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<ReligionId> : SerializerMemcpy<ReligionId> {};

struct PopTypeId : EntityId<uint8_t> {
    PopTypeId() = default;
    PopTypeId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<PopTypeId> : SerializerMemcpy<PopTypeId> {};

struct PopId : EntityId<uint8_t> {
    PopId() = default;
    PopId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<PopId> : SerializerMemcpy<PopId> {};

struct BuildingTypeId : EntityId<uint8_t> {
    BuildingTypeId() = default;
    BuildingTypeId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<BuildingTypeId> : SerializerMemcpy<BuildingTypeId> {};

struct WarId : EntityId<uint8_t> {
    WarId() = default;
    WarId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<WarId> : SerializerMemcpy<WarId> {};

struct DecisionId : EntityId<uint8_t> {
    DecisionId() = default;
    DecisionId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<DecisionId> : SerializerMemcpy<DecisionId> {};

struct EventId : EntityId<uint16_t> {
    EventId() = default;
    EventId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<EventId> : SerializerMemcpy<EventId> {};

struct NationId : EntityId<uint16_t> {
    NationId() = default;
    NationId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<NationId> : SerializerMemcpy<NationId> {};

struct ProvinceId : EntityId<uint16_t> {
    ProvinceId() = default;
    ProvinceId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<ProvinceId> : SerializerMemcpy<ProvinceId> {};

struct TechnologyId : EntityId<uint8_t> {
    TechnologyId() = default;
    TechnologyId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<TechnologyId> : SerializerMemcpy<TechnologyId> {};

struct TerrainTypeId : EntityId<uint8_t> {
    TerrainTypeId() = default;
    TerrainTypeId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<TerrainTypeId> : SerializerMemcpy<TerrainTypeId> {};

struct TreatyId : EntityId<uint16_t> {
    TreatyId() = default;
    TreatyId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<TreatyId> : SerializerMemcpy<TreatyId> {};

struct UnitTypeId : EntityId<uint8_t> {
    UnitTypeId() = default;
    UnitTypeId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<UnitTypeId> : SerializerMemcpy<UnitTypeId> {};

struct UnitId : EntityId<uint16_t> {
    UnitId() = default;
    UnitId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<UnitId> : SerializerMemcpy<UnitId> {};

struct IdeologyId : EntityId<uint8_t> {
    IdeologyId() = default;
    IdeologyId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<IdeologyId> : SerializerMemcpy<IdeologyId> {};

struct BuildingId : EntityId<uint8_t> {
    BuildingId() = default;
    BuildingId(size_t _id) : EntityId(_id) {}
};
template<>
struct Serializer<BuildingId> : SerializerMemcpy<BuildingId> {};

class World;
