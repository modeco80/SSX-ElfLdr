#include <utils/utils.h>

#include <cstdarg>
#include <cstring>
#include <cstdio>

#ifdef ERL
	#include <GameApi.h>
#endif

namespace elfldr::util {

#ifndef ERL
	std::uint8_t gTabLevel = 0;
#endif

	// Some macro (:() helpers
	// to make this a bit less garbage
#define LITERAL_STRLEN(lit) (sizeof(lit)-1)
#define LITERAL_STRCPY(dst, lit) __builtin_memcpy(dst, lit, LITERAL_STRLEN(lit))
#define VSNPRINTF_OFFSET(buf, size, offset) __builtin_vsnprintf(&buf[offset], (size - offset), format, val)

	// This code is messy since it needs to only use gcc builtins
	// to work across erl/elf boundaries, alongside some Platform Soup
	// to use bx::printf for the ERL.
	//
	// I am so sorry.
	// please don't hate me.

	void DebugOut(const char* format, ...) {
		char buf[256] {};

		__builtin_va_list val;
		__builtin_va_start(val, format);

		// pain. there's a way better way to write this,
		// but honestly, I'm too lazy too, and this *should* work regardless

		LITERAL_STRCPY(&buf[0], "[ElfLdr] ");
		VSNPRINTF_OFFSET(buf, sizeof(buf), LITERAL_STRLEN("[ElfLdr] "));

#ifndef ERL
		// Tab level handling
		for(int i = 0; i < gTabLevel; ++i)
			putc('\t', stdout);

		// use nano newlib puts() where we can
		puts(buf);
#else
		// I could *probably* search through the binary for puts(),
		// but this is fine (and just as safe).
		bx::printf("%s\n", buf);
#endif
		__builtin_va_end(val);
	}

} // namespace elfldr::util