#pragma once

#include "asterid/brassica.hh"

#include <memory>
#include <vector>
#include <numeric>

#define USING_HYENA_TYPES typedef typename BASE::grid_t grid_t; typedef typename BASE::vec2_t vec2_t; typedef typename BASE::rect_t rect_t; typedef std::unique_ptr<BASE> base_ptr;

namespace asterid::hyena {
	
	template <typename G> struct panel {
		typedef G grid_t;
		typedef brassica::vec2_t<grid_t> vec2_t;
		typedef brassica::rect_t<grid_t> rect_t;
		
		virtual ~panel() {};
		
		void layout(rect_t geo_in);
		
		inline vec2_t const & min() const { return min_v; }
		inline vec2_t const & max() const { return max_v; }
	protected:
		rect_t geo_v;
		vec2_t min_v;
		vec2_t max_v;
		
		virtual void layout_eval() = 0;
	};
	
	template <typename BASE> struct hbox : public BASE {
		USING_HYENA_TYPES
		
		void check_bounds() {
			this->min_v = {0};
			this->max_v = {std::numeric_limits<grid_t>::max()};
			
			for (auto const & child : children) {
				child->precheck();
				vec2_t const & min_c = child->min();
				vec2_t const & max_c = child->max();
				if (min_c.x() > this->min_v.x()) this->min_v.x() = min_c.x();
				if (min_c.y() > this->min_v.y()) this->min_v.y() = min_c.y();
				if (max_c.x() > this->max_v.x()) this->max_v.x() = max_c.x();
				if (max_c.y() > this->max_v.y()) this->max_v.y() = max_c.y();
			}
			
			if (this->min_v.x() > this->max_v.x()) this->max_v.x() = this->min_v.x();
			if (this->min_v.y() > this->max_v.y()) this->max_v.y() = this->min_v.y();
		}
	protected:
		std::vector<base_ptr> children;
	};
	
}

/*

#define GRID_MAX std::numeric_limits<grid_t>::max()

void hyena::layout::hbox::precheck() {
	min_v = {0};
	max_v = {GRID_MAX};
	
	for (auto const & child : children) {
		child->precheck();
		vec2_t const & min_c = child->min();
		vec2_t const & max_c = child->max();
		if (min_c.x() > min_v.x()) min_v.x() = min_c.x();
		if (min_c.y() > min_v.y()) min_v.y() = min_c.y();
		if (max_c.x() > max_v.x()) max_v.x() = max_c.x();
		if (max_c.y() > max_v.y()) max_v.y() = max_c.y();
	}
	
	if (min_v.x() > max_v.x()) max_v.x() = min_v.x();
	if (min_v.y() > max_v.y()) max_v.y() = min_v.y();
}
*/
