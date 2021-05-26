#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include "world.hpp"
#include "nation.hpp"

namespace Diplomacy {
	bool is_friend(Nation * us, Nation * them);
	bool is_foe(Nation * us, Nation * them);
};

namespace TreatyClause {
	class BaseClause {
	protected:
		Nation * sender;
		Nation * receiver;
	public:
		BaseClause(Nation * _sender, Nation * _receiver) {
			this->sender = _sender;
			this->receiver = _receiver;
		};
		virtual ~BaseClause() {};
		virtual unsigned cost();
		virtual void enforce();
		virtual bool in_effect();
	};

	class WarReparations : BaseClause {
		size_t amount;
		size_t days_duration;
	public:
		unsigned cost() {
			return (this->receiver->economy_score * (this->amount * this->days_duration)) / 100;
		}
		void enforce() {
			this->sender->prestige += 0.0001f;
			this->receiver->prestige -= 0.0001f;
			this->sender->budget -= amount;
			this->receiver->budget += amount;
			this->days_duration--;
		}
		bool in_effect() {
			return (this->days_duration != 0);
		}
	};

	class Humiliate : BaseClause {
		size_t amount;
		size_t days_duration;
	public:
		unsigned cost() {
			return (this->receiver->prestige * (this->amount * this->days_duration)) / 100;
		}
		void enforce() {
			this->sender->prestige += this->amount;
			this->receiver->prestige -= this->amount;
			this->days_duration--;
		}
		bool in_effect() {
			return (this->days_duration != 0);
		}
	};

	/*class LiberateNation : BaseClause {
		Nation * nation;
	public:
		
	};*/
};

class Treaty {
public:
	std::string name;
	std::vector<TreatyClause::BaseClause *> clauses;
};

#endif