#include <stdio.h>
#include <stdlib.h>
#include <ugens.h>
#include <math.h>
#include <algorithm>
#include <PField.h>
#include <Instrument.h>
#include "PARSEGRAN.h"
#include <rt.h>
#include <rtdefs.h>
#include <iostream>
#include <vector>
#include <fparser27/fparser.hh>
#include <string.h>

#define MAXGRAINS 1500

PARSEGRAN::PARSEGRAN() : branch(0), lastRate(0), lastDur(0), lastFreq(0), lastPan(0)
{
}



PARSEGRAN::~PARSEGRAN()
{
	if (!configured)
		return;
	for (size_t i = 0; i < grains->size(); i++)
	{
		delete (*grains)[i];
	}
	delete grains;

	delete funcDur;
	delete funcRate;
	delete funcFreq;
	delete funcPan;
}


int PARSEGRAN::init(double p[], int n_args)
{

/* Args:
		p0: outskip
		p1: dur
		p2: amp*
		p3: rateExpr
		p4: rateMin
		p5: rateMax
		p6: durExpr
		p7: durMin
		p8: durMax
		p9: freqExpr
		p10: freqMin
		p11: freqMax
		p12: panExpr
		p13: panMin
		p14: panMax
		p15: wavetable**
		p16: grainEnv**
		p17: x1* (optional)
		p18: x2* (optional)
		p19: grainLimit=1500 (optional)

		* may recieve pfield values
		** must be passed pfield maketables.
*/
	if (rtsetoutput(p[0], p[1], this) == -1)
		return DONT_SCHEDULE;

	if (outputChannels() > 2)
	      return die("PARSEGRAN", "Output must be mono or stereo.");

	if (n_args < 17)
		return die("PARSEGRAN", "21 arguments are required");
	else if (n_args > 20)
		return die("PARSEGRAN", "too many arguments");
	grainEnvLen = 0;
	wavetableLen = 0;
	amp = p[2];

	newGrainCounter = 0;

	// init tables
	wavetable = (double *) getPFieldTable(15, &wavetableLen);
	grainEnv = (double *) getPFieldTable(16, &grainEnvLen);
	

	funcRate = getfunc(3);
	minRate = p[4];
	maxRate = p[5];

	funcDur = getfunc(6);
	minDur = p[7];
	maxDur = p[8];

	funcFreq = getfunc(9);
	minFreq = p[10];
	maxFreq = p[11];

	funcPan = getfunc(12);
	minPan = p[13];
	maxPan = p[14];

	x1 = 0;
	x2 = 0;

	if (n_args > 17)
	{
		x1 = p[17];
		x2 = p[18];

		grainLimit = p[19];
		if (grainLimit > MAXGRAINS)
		{
			rtcmix_advise("STGRAN2", "user provided max grains exceeds limit, lowering to 1500");
			grainLimit = MAXGRAINS;
		}
			
	}
	else
		grainLimit = MAXGRAINS;

	//std::cout << "setup finished" << "\n";
	return nSamps();
}



int PARSEGRAN::configure()
{
	// make the needed grains, which have no values yet as they need to be set dynamically
	grains = new std::vector<Grain*>();
	// maybe make the maximum grain value a non-pfield enabled parameter

	for (int i = 0; i < grainLimit; i++)
	{
		grains->push_back(new Grain());
	}

	configured = true;

	return 0;	// IMPORTANT: Return 0 on success, and -1 on failure.
}

FunctionParser* PARSEGRAN::getfunc(int pfieldNum)
{
	const PField &field = getPField(pfieldNum);
	const char *fieldstr = field.stringValue(0.0);
	//std::cout << "Compiling function:  `" << fieldstr << "`\n";

	FunctionParser* fp = new FunctionParser();
	// use orig string so char offsets in err msg will be right
	int ret = fp->Parse(fieldstr, "u1,u2,u3,u4,l,x1,x2");
	if (ret >= 0)
		printf("Parser error for expression \"%s\" at character %d ('%c'): %s.",
					fieldstr, ret, fieldstr[ret], fp->ErrorMsg());
	fp->Optimize();

	return fp;
}

double PARSEGRAN::callfunc(FunctionParser* func, double min, double max, double last, double u1, double u2, double u3, double u4)
        // Returns a value within a range close to a preferred value
                    // tightness: 0 max away from mid
                     //               1 even distribution
                      //              2+amount closeness to mid
                      //              no negative allowed
{
	double vars[] = {u1, u2, u3, u4, last, x1, x2};
	double num;
	num = func->Eval(vars);
	if (num < min)
		num = min;

	if (num > max)
		num = max;

	return(num);
}

// set new parameters and turn on an idle grain
void PARSEGRAN::resetgrain(Grain* grain)
{
	//std::cout << "making grain" << "\n";

	double u1 = ((double) rand() / (RAND_MAX));
	double u2 = ((double) rand() / (RAND_MAX));
	double u3 = ((double) rand() / (RAND_MAX));
	double u4 = ((double) rand() / (RAND_MAX));
	float freq = callfunc(funcFreq, minFreq, maxFreq, lastFreq, u1, u2, u3, u4);
	lastFreq = freq;
	//std::cout << "setting dur" << "\n";
	float grainDurSamps =  callfunc(funcDur, minDur, maxDur, lastDur, u1, u2, u3, u4) * SR;
	lastDur = grainDurSamps / SR;
	//std::cout << "setting pan" << "\n";
	float panR = (float) callfunc(funcPan, minPan, maxPan, lastPan, u1, u2, u3, u4);
	lastPan = panR;
	//std::cout << "setting counter" << "\n";
	newGrainCounter = (int)round(SR * callfunc(funcRate, minRate, maxRate, lastRate, u1, u2, u3, u4));
	lastRate = newGrainCounter / SR;
	//std::cout << "setting freq" << "\n";
	
	grain->waveSampInc = wavetableLen * freq / SR;
	grain->ampSampInc = ((float)grainEnvLen) / grainDurSamps;
	grain->currTime = 0;
	grain->isplaying = true;
	grain->wavePhase = 0;
	grain->ampPhase = 0;
	grain->panR = panR;
	grain->panL = 1 - panR; // separating these in RAM means fewer sample rate calculations
	(*grain).dur = (int)round(grainDurSamps);
	//std::cout<<"sending grain with freq : " << freq << " dur : " << grain->dur << " panR " << panR << "\n";

}


// update pfields
void PARSEGRAN::doupdate()
{
	double p[3];
	update(p, 3);
	amp =(float) p[2];

}


int PARSEGRAN::run()
{
	
	float out[2];
	for (int i = 0; i < framesToRun(); i++) {
		if (--branch <= 0)
		{
		doupdate();
		branch = getSkip();
		}

		out[0] = 0;
		out[1] = 0;
		for (size_t j = 0; j < grains->size(); j++)
		{
			Grain* currGrain = (*grains)[j];
			if (currGrain->isplaying)
			{
				if (++(*currGrain).currTime > currGrain->dur)
				{
					currGrain->isplaying = false;
				}
				else
				{
					// should include an interpolation option at some point
					float grainAmp = oscili(1, currGrain->ampSampInc, grainEnv, grainEnvLen, &((*currGrain).ampPhase));
					float grainOut = oscili(grainAmp,currGrain->waveSampInc, wavetable, wavetableLen, &((*currGrain).wavePhase));
					out[0] += grainOut * currGrain->panL;
					out[1] += grainOut * currGrain->panR;
				}
			}
			// this is not an else statement so a grain can be potentially stopped and restarted on the same frame

			if ((newGrainCounter <= 0) && !currGrain->isplaying)
			{
				resetgrain(currGrain);

			}
		}

		// if all current grains are occupied, we skip this request for a new grain
		if (newGrainCounter <= 0)
		{
			newGrainCounter = 1;
		}

		out[0] *= amp;
		out[1] *= amp;
		rtaddout(out);
		newGrainCounter--;
		increment();
	}

	// Return the number of frames we processed.

	return framesToRun();
}


Instrument *makePARSEGRAN()
{
	PARSEGRAN *inst = new PARSEGRAN();
	inst->set_bus_config("PARSEGRAN");

	return inst;
}

#ifndef EMBEDDED
void rtprofile()
{
	RT_INTRO("PARSEGRAN", makePARSEGRAN);
}
#endif

