#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <random>

namespace asterid::codon {
	
	namespace mutator {
		
		template <typename E,  std::underlying_type_t<E> enum_count> struct uniform_randomize {
			std::mt19937_64 rnd {std::default_random_engine {}()};
			std::uniform_int_distribution<std::underlying_type_t<E>> rng {0, enum_count - 1};
			E operator ()(E) {
				return static_cast<E>(rng(rnd));
			}
		};
		
		template <typename E,  std::underlying_type_t<E> enum_count> struct weighted_randomize {
			std::mt19937_64 rnd {std::default_random_engine {}()};
			std::uniform_int_distribution<std::underlying_type_t<E>> rng {0, enum_count - 1};
			E operator ()(E) {
				return static_cast<E>(rng(rnd));
			}
		};
		
	}
	
	namespace evolver {
		
	}
	
	template <typename E, template <typename> typename M> struct chain {
		
	};

}
