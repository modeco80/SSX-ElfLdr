#include "patch.h"
#include "BinaryMap.h"
#include <cassert>

// This is the max amount of patches the system can take.
// Right now this number is kinda overkill,
// but you can bump it up if you're going to use
// this for something more serious.
constexpr static uint32_t MAX_PATCHES = 4;

static elfldr::BinaryMap<elfldr::PatchId, elfldr::Patch*, MAX_PATCHES> gPatchMap;

namespace elfldr {

	namespace detail {
		void RegisterPatch(PatchId id, Patch* patch) {
			// Don't allow a null patch
			if(patch == nullptr)
				return;
			
			gPatchMap.Insert(id, patch);
		}
	} // namespace detail
	
	Patch* GetPatchById(PatchId id) {
		auto** patch = gPatchMap.MaybeGetValue(id);
		
		if(!patch)
			return nullptr;
		
		return *patch;
	}
	
} // namespace elfldr