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

class GoodId : public EntityId<uint8_t> {};
template<>
struct Serializer<GoodId>: public SerializerMemcpy<GoodId> {};

class ProductId : public EntityId<uint8_t> {};
template<>
struct Serializer<ProductId>: public SerializerMemcpy<ProductId> {};

class LanguageId : public EntityId<uint16_t> {};
template<>
struct Serializer<LanguageId>: public SerializerMemcpy<LanguageId> {};

class ReligionId : public EntityId<uint8_t> {};
template<>
struct Serializer<ReligionId>: public SerializerMemcpy<ReligionId> {};

class PopTypeId : public EntityId<uint8_t> {};
template<>
struct Serializer<PopTypeId>: public SerializerMemcpy<PopTypeId> {};

class BuildingTypeId : public EntityId<uint8_t> {};
template<>
struct Serializer<BuildingTypeId>: public SerializerMemcpy<BuildingTypeId> {};

class WarId : public EntityId<uint8_t> {};
template<>
struct Serializer<WarId>: public SerializerMemcpy<WarId> {};

class DecisionId : public EntityId<uint8_t> {};
template<>
struct Serializer<DecisionId>: public SerializerMemcpy<DecisionId> {};

class EventId : public EntityId<uint16_t> {};
template<>
struct Serializer<EventId>: public SerializerMemcpy<EventId> {};

class NationModifierId : public EntityId<uint8_t> {};
template<>
struct Serializer<NationModifierId>: public SerializerMemcpy<NationModifierId> {};

class NationId : public EntityId<uint16_t> {};
template<>
struct Serializer<NationId>: public SerializerMemcpy<NationId> {};

class BattleId : public EntityId<uint16_t> {};
template<>
struct Serializer<BattleId>: public SerializerMemcpy<BattleId> {};

class ProvinceId : public EntityId<uint16_t> {};
template<>
struct Serializer<ProvinceId>: public SerializerMemcpy<ProvinceId> {};

class TechnologyId : public EntityId<uint8_t> {};
template<>
struct Serializer<TechnologyId>: public SerializerMemcpy<TechnologyId> {};

class TerrainTypeId : public EntityId<uint8_t> {};
template<>
struct Serializer<TerrainTypeId>: public SerializerMemcpy<TerrainTypeId> {};

class TreatyId : public EntityId<uint16_t> {};
template<>
struct Serializer<TreatyId>: public SerializerMemcpy<TreatyId> {};

class UnitTypeId : public EntityId<uint8_t> {};
template<>
struct Serializer<UnitTypeId>: public SerializerMemcpy<UnitTypeId> {};

class UnitId : public EntityId<uint16_t> {};
template<>
struct Serializer<UnitId>: public SerializerMemcpy<UnitId> {};

class IdeologyId : public EntityId<uint8_t> {};
template<>
struct Serializer<IdeologyId>: public SerializerMemcpy<IdeologyId> {};

class BuildingId : public EntityId<uint8_t> {};
template<>
struct Serializer<BuildingId>: public SerializerMemcpy<BuildingId> {};

class World;
