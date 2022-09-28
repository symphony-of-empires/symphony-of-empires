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

struct GoodId : EntityId<uint8_t> {};
template<>
struct Serializer<GoodId> : SerializerMemcpy<GoodId> {};

struct ProductId : EntityId<uint8_t> {};
template<>
struct Serializer<ProductId> : SerializerMemcpy<ProductId> {};

struct LanguageId : EntityId<uint16_t> {};
template<>
struct Serializer<LanguageId> : SerializerMemcpy<LanguageId> {};

struct ReligionId : EntityId<uint8_t> {};
template<>
struct Serializer<ReligionId> : SerializerMemcpy<ReligionId> {};

struct PopTypeId : EntityId<uint8_t> {};
template<>
struct Serializer<PopTypeId> : SerializerMemcpy<PopTypeId> {};

struct BuildingTypeId : EntityId<uint8_t> {};
template<>
struct Serializer<BuildingTypeId> : SerializerMemcpy<BuildingTypeId> {};

struct WarId : EntityId<uint8_t> {};
template<>
struct Serializer<WarId> : SerializerMemcpy<WarId> {};

struct DecisionId : EntityId<uint8_t> {};
template<>
struct Serializer<DecisionId> : SerializerMemcpy<DecisionId> {};

struct EventId : EntityId<uint16_t> {};
template<>
struct Serializer<EventId> : SerializerMemcpy<EventId> {};

struct NationId : EntityId<uint16_t> {};
template<>
struct Serializer<NationId> : SerializerMemcpy<NationId> {};

struct BattleId : EntityId<uint16_t> {};
template<>
struct Serializer<BattleId> : SerializerMemcpy<BattleId> {};

struct ProvinceId : EntityId<uint16_t> {};
template<>
struct Serializer<ProvinceId> : SerializerMemcpy<ProvinceId> {};

struct TechnologyId : EntityId<uint8_t> {};
template<>
struct Serializer<TechnologyId> : SerializerMemcpy<TechnologyId> {};

struct TerrainTypeId : EntityId<uint8_t> {};
template<>
struct Serializer<TerrainTypeId> : SerializerMemcpy<TerrainTypeId> {};

struct TreatyId : EntityId<uint16_t> {};
template<>
struct Serializer<TreatyId> : SerializerMemcpy<TreatyId> {};

struct UnitTypeId : EntityId<uint8_t> {};
template<>
struct Serializer<UnitTypeId> : SerializerMemcpy<UnitTypeId> {};

struct UnitId : EntityId<uint16_t> {};
template<>
struct Serializer<UnitId> : SerializerMemcpy<UnitId> {};

struct IdeologyId : EntityId<uint8_t> {};
template<>
struct Serializer<IdeologyId> : SerializerMemcpy<IdeologyId> {};

struct BuildingId : EntityId<uint8_t> {};
template<>
struct Serializer<BuildingId> : SerializerMemcpy<BuildingId> {};

class World;
