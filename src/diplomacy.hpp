#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include "world.hpp"
#include "nation.hpp"

namespace Diplomacy {
	// Determines if the other nation is a friendly potential ally
	bool is_friend(Nation* us, Nation* them);

	// Determines if the other nation is an enemy and potential rival
	bool is_foe(Nation* us, Nation* them);
};

namespace TreatyClause {
	class BaseClause {
	protected:
		// Nation who created this clause
		Nation* sender;

		// Nation who should accept/reject this clause
		Nation* receiver;
	public:
		BaseClause(Nation* _sender, Nation* _receiver) {
			sender = _sender;
			receiver = _receiver;
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
	
	/**
	* Makes loser to pay war reparations to the winner
	 */
	class WarReparations : BaseClause {
		// Amount to pay per day
		float amount;

		// Number of days this clause lasts
		size_t days_duration;
	public:
		unsigned cost() {
			return (receiver->economy_score* (amount* days_duration)) / 100;
		}
		void enforce() {
			sender->prestige += 0.0001f;
			receiver->prestige -= 0.0001f;
			sender->budget -= amount;
			receiver->budget += amount;
			days_duration--;
		}
		bool in_effect() {
			return (days_duration != 0);
		}
	};
	
	/**
	* Reduces prestige of loser and increments prestige from winner
	 */
	class Humiliate : BaseClause {
		// Amount to reduce prestige to per day
		float amount;

		// Number of days this clause lasts
		size_t days_duration;
	public:
		unsigned cost() {
			return (receiver->prestige* (amount* days_duration)) / 100;
		}
		void enforce() {
			sender->prestige += amount;
			receiver->prestige -= amount;
			days_duration--;
		}
		bool in_effect() {
			return (days_duration != 0);
		}
	};
	
	/**
	* Liberates a nation from another
	 */
	class LiberateNation : BaseClause {
		Nation* liberated;
		std::vector<Province *> provinces;
		bool done = false;
	public:
		unsigned cost() {
			size_t value = 0;
			for(const auto& province: provinces) {
				value += province->budget* province->total_pops();
			}
			return value* 0.00001f;
		}
		void enforce() {
			// Reduce prestige due to lost lands
			sender->prestige += cost()* 0.0000025f;
			receiver->prestige -= cost()* 0.000005f;
			
			// Give provinces to this liberated nation
			for(auto& province: provinces) {
				province->owner = liberated;
			}
			
			// One-time clause
			done = true;
		}
		bool in_effect() {
			return done;
		}
	};
	
	/**
	* Imposes a policy to be put in action on a nation
	 */
	class ImposePolicies : BaseClause {
		Policies imposed;
		bool done;
	public:
		unsigned cost() {
			return imposed.difference(receiver->current_policy);
		}
		void enforce() {
			memcpy(&receiver->current_policy, &imposed, sizeof(Policies));
			
			done = true;
		}
		bool in_effect() {
			return done;
		}
	};
	
	/**
	* Anexxes territory from the loser
	 */
	class AnexxProvince : BaseClause {
		std::vector<Province *> provinces;
		bool done = false;
	public:
		unsigned cost() {
			size_t value = 0;
			for(const auto& province: provinces) {
				value += province->budget + province->total_pops();
			}
			return value* 0.000001f;
		}
		void enforce() {
			sender->prestige += cost()* 0.0000025f;
			receiver->prestige -= cost()* 0.000005f;
			
			// Give provinces to the winner
			for(auto& province: provinces) {
				province->owner = sender;
			}
			
			// One-time clause
			done = true;
		}
		bool in_effect() {
			return done;
		}
	};
	
	/**
	* Calls for a ceasefire
	 */
	class Ceasefire : BaseClause {
		// Number of days this clause lasts
		size_t days_duration;
	public:
		unsigned cost() {
			return receiver->military_score + receiver->naval_score;
		}
		void enforce() {
			NationId receiver_id = g_world->get_id(receiver);
			NationId sender_id = g_world->get_id(sender);
			
			sender->relations[receiver_id].has_war = false;
			sender->relations[receiver_id].has_true = true;
			
			receiver->relations[sender_id].has_war = false;
			receiver->relations[sender_id].has_truce = true;
			
			days_duration--;
		}
		bool in_effect() {
			return (days_duration != 0);
		}
	};
};

class Treaty {
public:
	std::string name;
	std::vector<TreatyClause::BaseClause *> clauses;
};

#endif
