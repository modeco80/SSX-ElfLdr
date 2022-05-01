/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elfldr/ElfLoader.h>
#include <runtime/Assert.h>
#include <utils/utils.h>

#include "patch.h"

// is this include needed?
#include <utils/codeutils.h>

// Autogenerated version header
#include <Version.h>

#include "elfldr/GameVersion.h"

const char* gHostFsPath = "host:";
elfldr::ElfLoader gLoader;

void DoLoadElf(const elfldr::GameVersionData& gdata) {
	char elfPath[elfldr::util::MaxPath] {};
	strncpy(&elfPath[0], gHostFsPath, elfldr::util::MaxPath * sizeof(char));
	strcat(elfPath, gdata.GetGameBinary());

	ELFLDR_VERIFY(gLoader.LoadElf(elfPath));
}

void ApplyPatch(elfldr::Patch* patch) {
	ELFLDR_VERIFY(patch);

	elfldr::util::DebugOut("Applying patch \"%s\"...", patch->GetName());
	if(!patch->IsCompatiable()) {
		elfldr::util::DebugOut("Patch \"%s\" is incompatiable with the current game.", patch->GetName());
		return;
	}

	elfldr::util::gTabLevel++;
	patch->Apply();
	elfldr::util::gTabLevel--;

	elfldr::util::DebugOut("Finished applying patch \"%s\"...", patch->GetName());
}

int main() {
	elfldr::util::DebugOut("SSX-ElfLdr version " ELFLDR_VERSION_TAG);

	// Init loader services.
	elfldr::InitLoader();

	elfldr::util::DebugOut("Probing game version...");
	elfldr::ProbeVersion();

	const auto& gdata = elfldr::GetGameVersionData();

	if(gdata.game == elfldr::Game::Invalid) {
		elfldr::util::DebugOut("No game that is supported could be detected alongside ElfLdr.");
		elfldr::util::DebugOut("Make sure elfldr is in the proper spot.");
		while(true)
			;
	}

	DoLoadElf(gdata);

	ApplyPatch(elfldr::GetPatchById(0x00));
	ApplyPatch(elfldr::GetPatchById(0x01));

	// testing this patch for regular inclusion,
	// it will PROBABLY have a different ID
	ApplyPatch(elfldr::GetPatchById(0xE0));

	char* argv[1];
	argv[0] = elfldr::UBCast<char*>(gHostFsPath); // I hate this

	// Execute the elf
	gLoader.ExecElf(sizeof(argv) / sizeof(argv[0]), argv);

	// elfldr shouldn't return.
	while(true)
		;
}
