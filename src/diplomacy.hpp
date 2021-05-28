#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include "world.hpp"
#include "nation.hpp"

namespace Diplomacy {
	// Determines if the other nation is a friendly potential ally
	bool is_friend(Nation * us, Nation * them);

	// Determines if the other nation is an enemy and potential rival
	bool is_foe(Nation * us, Nation * them);
};

namespace TreatyClause {
	class BaseClause {
	protected:
		// Nation who created this clause
		Nation * sender;

		// Nation who should accept/reject this clause
		Nation * receiver;
	public:
		BaseClause(Nation * _sender, Nation * _receiver) {
			this->sender = _sender;
			this->receiver = _receiver;
		};
		virtual ~BaseClause() {};

		// Function to determine the "political" cost of this clause, and how much willing the AI
		// is to accept this clause, this is only used by the AI
		virtual unsigned cost();

		// Function to enforce the policy per day (or higher time spans)
		virtual void enforce();

		// Determines whenever the clause is in effect or not, when it is not in effect
		// then it's removed permanently
		virtual bool in_effect();
	};

	class WarReparations : BaseClause {
		// Amount to pay per day
		float amount;

		// Number of days this clause lasts
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
		// Amount to reduce prestige to per day
		float amount;

		// Number of days this clause lasts
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