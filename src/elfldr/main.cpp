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

// Autogenerated version header
#include <stdio.h>
#include <Version.h>

#include "utils/GameVersion.h"
#include "utils/VersionProbe.h"

elfldr::ElfLoader gLoader;

int main() {
#ifndef NDEBUG
	elfldr::util::DebugOut("SSX-ElfLdr version " ELFLDR_VERSION_TAG " (" __DATE__ " " __TIME__ ")");
#else
	elfldr::util::DebugOut("SSX-ElfLdr version " ELFLDR_VERSION_TAG);
#endif

	// Initialize loader services & probe the game version.
	// TODO: If config file found, parse it, load in the game version, and verify that
	// 		that binary exists. If it doesn't exist, bail back to OSD.
	elfldr::InitLoader();
	elfldr::util::AutodetectGameVersion();

	const auto& gdata = elfldr::util::GetGameVersionData();

	if(gdata.game == elfldr::util::Game::Invalid) {
		// TODO: if config file, change this message to state "Invalid game configured in config file {config}".
		elfldr::util::DebugOut("No supported game could be detected alongside ElfLdr.");
		elfldr::util::DebugOut("Make sure elfldr is in the proper spot.");
		while(true)
			;
	}

	// TODO: only display if autodetecting
	elfldr::util::DebugOut("ElfLdr GameID: \"%s\"", gdata.GameID().CStr());

	// Load the ELF.
	{
		static char elfPath[elfldr::util::MaxPath] {};
		snprintf(&elfPath[0], elfldr::util::MaxPath * sizeof(char), "host:%s", gdata.GetGameBinary().CStr());
		ELFLDR_VERIFY(gLoader.LoadElf(elfPath));
	}

	auto ApplyPatch = [](elfldr::Patch* patch) {
		if(patch == nullptr)
			return;

		elfldr::util::DebugOut("[Patch %s] Applying patch...", patch->GetName());
		patch->Apply();
		elfldr::util::DebugOut("[Patch %s] Finished applying.", patch->GetName());
	};

	ApplyPatch(elfldr::GetPatchById(0x00));
	ApplyPatch(elfldr::GetPatchById(0x01));

	// testing this patch for regular inclusion,
	// it will PROBABLY have a different ID
	ApplyPatch(elfldr::GetPatchById(0xE0));

	char* argv[1];
	argv[0] = elfldr::UBCast<char*>("host:"); // I hate this

	// Execute the elf
	gLoader.ExecElf(sizeof(argv) / sizeof(argv[0]), argv);

	// elfldr shouldn't return.
	while(true)
		;
}
