#ifndef _SQUAWKSD_H_
#define _SQUAWKSD_H_
#include <Squawk.h>
#include "Fat16.h"

class SquawkSynthSD : public SquawkSynth {
  private:
  	Fat16 f;
	public:
	  inline void play() { Squawk.play(); };
		void play(Fat16 file);
		//void convert(Fat16 in, Fat16 out);
};

extern SquawkSynthSD SquawkSD;

#endif