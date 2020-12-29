/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

/* third-party library includes */

/* project-specific includes */
#include <Mod2ToModpExamples.h>

/* same module include */

int main(int argc, char ** argv) {

  /* we must have atleast 1 argument */
  if (argc < 2) {
    return -1;
  }

  /* all single-parameter commands */
  if (argc == 2) {
    if (strcmp(argv[1], "-MOD2TOMODP") == 0) {
      ConvertMod2ToModPExample();
    }
  }

  return 0;
}
