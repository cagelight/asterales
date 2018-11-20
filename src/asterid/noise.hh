#pragma once

#include <cstdint>

#include "brassica.hh"

namespace asterid {
	
	struct simplex final {
		
		typedef asterid::brassica::vec2_t<double> vec2_t;
		typedef asterid::brassica::vec3_t<double> vec3_t;
		typedef asterid::brassica::vec4_t<double> vec4_t;
		
		constexpr simplex() : perm {}, perm12 {} {
			for (uint_fast16_t i = 0; i < 256; i++) perm[i] = i;
		}
		
		template <typename G> simplex(G & rnd) : simplex() { initialize(rnd); }
		template <typename G> void initialize(G & rnd) {
			
			for (uint_fast16_t i = 0; i < 256; i++) {
				std::uniform_int_distribution<uint8_t> dist {static_cast<uint8_t>(i), 0xFF};
				uint8_t r = dist(rnd) + i;
				uint8_t p = perm[i];
				perm12[i + 256] = perm12[i] = (perm[i + 256] = perm[i] = perm[r]) % 12;
				perm[r] = p;
			}
		}
		
		inline double generate(vec2_t const & pos) const { return generate(pos.x(), pos.y()); }
		double generate(double x, double y) const;
		inline double generate(vec3_t const & pos) const { return generate(pos.x(), pos.y(), pos.z()); }
		double generate(double x, double y, double z) const;
		
	private:
		uint8_t perm[512];
		uint8_t perm12[512];
	};
	
};
