#pragma once

#include <ctgmath>

namespace asterales {

	template <typename Q = unsigned int> struct bspacker {
		
		typedef int flags_t;
		struct flags {
			static constexpr flags_t none = 0;
			static constexpr flags_t maintain_square_ratio = 1;
		};
		
		bspacker(Q init_width, Q init_height, flags_t fl = flags::none) : _flags(fl) {
			root = new bspacker_node(false, init_width, init_height);
		}
		bspacker(flags_t fl = flags::none) : bspacker(0, 0, fl) {}
		
		~bspacker() {
			delete root;
		}
		
		struct bsp_coords {
			bsp_coords(Q x, Q y) : x(x), y(y) {}
			bsp_coords() : bsp_coords(0, 0) {}
			Q x;
			Q y;
		};
		
		bsp_coords pack (Q width, Q height) {
			bsp_coords c;
			bool ok;
			root->pack_iter(ok, width, height, c);
			
			if (!ok) { // expand
				if (root->width + width > root->height + height) { // expand by height
					bspacker_node * new_root = new bspacker_node {false, std::max(root->width, width), root->height + height};
					new_root->child_a = root;
					new_root->child_b = new bspacker_node {false, std::max(root->width, width), height, 0, root->height};
					new_root->pack_iter(ok, width, height, c);
					// assert(ok);
					root = new_root;
				} else { // expand by width
					bspacker_node * new_root = new bspacker_node {false, root->width + width, std::max(root->height, height)};
					new_root->child_a = root;
					new_root->child_b = new bspacker_node {false, width, std::max(root->height, height), root->width, 0};
					new_root->pack_iter(ok, width, height, c);
					// assert(ok);
					root = new_root;
				}
			}
			
			return c;
		}
		
		inline Q width() const { return root->width; }
		inline Q height() const { return root->height; }
		
	private:
		
		flags_t _flags;

		struct bspacker_node {
			
			bspacker_node(bool is_endpoint, Q width, Q height, Q x, Q y) : width(width), height(height), coords(x, y), is_endpoint(is_endpoint) {}
			bspacker_node(bool is_endpoint, Q width, Q height) : bspacker_node(is_endpoint, width, height, 0, 0) {}
			
			~bspacker_node() {
				if (child_a && child_b) {delete child_a; delete child_b;}
			}
			
			Q const width;
			Q const height;
			bsp_coords coords;
			
			bool is_endpoint = false;
			bspacker_node * child_a = nullptr;
			bspacker_node * child_b = nullptr;
			
			void pack_iter (bool & ok, Q const & width, Q const & height, bsp_coords & pos) {
				if (is_endpoint || (width > this->width || height > this->height)) { //node already has value or new node is too big to fit
					ok = false;
					return;
				}
				
				if (child_a && child_b) { // if have children, delegate to them
					child_a->pack_iter(ok, width, height, pos);
					if (ok) return;
					child_b->pack_iter(ok, width, height, pos);
					return;
				}
				
				// have space but no children...
					
				if (width == this->width && height == this->height) { // don't need to make children, we fit the new node perfectly, assume the value and good to go
					is_endpoint = true;
					ok = true;
					pos = coords;
					return;
				}
				
				if (width != this->width && height != this->height) { // children won't be enough, one of the children needs to have further children
					child_a = new bspacker_node { false, width, this->height, coords.x, coords.y };
					child_b = new bspacker_node { false, this->width - width, this->height, coords.x + width, coords.y };
					child_a->pack_iter(ok, width, height, pos);
					// assert(ok);
					return;
				}
				
				if (width == this->width) { // width matches, single split
					child_a = new bspacker_node { true, width, height, coords.x, coords.y };
					child_b = new bspacker_node { false, width, this->height - height, coords.x, coords.y + height };
					ok = true;
					pos = child_a->coords;
					return;
				}
				
				if (height == this->height) { // height matches, single split
					child_a = new bspacker_node { true, width, height, coords.x, coords.y };
					child_b = new bspacker_node { false, this->width - width, this->height, coords.x + width, coords.y };
					ok = true;
					pos = child_a->coords;
					return;
				}
				
				// should never get here
			}
			
		};
		
		bspacker_node * root;
		
	};

}
