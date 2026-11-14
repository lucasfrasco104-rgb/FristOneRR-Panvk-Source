/*
 * Per-game tuning profile (Resident Evil 4 Ultimate HD on Mali-G57/Helio G99).
 *
 * Opt-in only. The profile is active when PANVK_PROFILE=re4 is set, or when
 * PANVK_PROFILE=auto is set and the process command line mentions bio4.exe.
 * Without PANVK_PROFILE the driver behaves exactly like the stock build.
 *
 * It only changes DEFAULTS. Any explicit PANVK_* variable still wins.
 *
 * Currently it changes no default: forcing early ZS was removed from it
 * because it breaks alpha-tested/discarding shaders (RE4 briefcase screen).
 * PANVK_FORCE_EARLYZS=1 remains an explicit debugging switch.
 */
#ifndef PANVK_PROFILE_H
#define PANVK_PROFILE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline bool
panvk_profile_re4(void)
{
   static int on = -1;
   if (on >= 0)
      return on;

   on = 0;
   const char *p = getenv("PANVK_PROFILE");
   if (!p)
      return on;
   if (strcmp(p, "re4") == 0) {
      on = 1;
      return on;
   }
   if (strcmp(p, "auto") != 0)
      return on;

   /* Auto-detect: look for bio4.exe in our own command line. */
   FILE *f = fopen("/proc/self/cmdline", "rb");
   if (f) {
      char buf[4096];
      size_t n = fread(buf, 1, sizeof(buf) - 1, f);
      fclose(f);
      for (size_t i = 0; i < n; i++) {
         if (buf[i] == '\0')
            buf[i] = ' ';
      }
      buf[n] = '\0';
      for (char *c = buf; *c; c++)
         *c = (*c >= 'A' && *c <= 'Z') ? *c + 32 : *c;
      if (strstr(buf, "bio4.exe") || strstr(buf, "bio4_ultimate"))
         on = 1;
   }
   return on;
}

#endif /* PANVK_PROFILE_H */
