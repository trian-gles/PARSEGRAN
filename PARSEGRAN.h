#include <Ougens.h>
#include <vector>
#include <fparser27/fparser.hh>
		  // the base class for this instrument
typedef struct {
	float waveSampInc; 
	float ampSampInc; 
	float wavePhase; 
	float ampPhase;
	float amp;	
	int dur; 
	float panR; 
	float panL; 
	int currTime; 
	bool isplaying;
	} Grain;


class PARSEGRAN : public Instrument {

public:
	PARSEGRAN();
	virtual ~PARSEGRAN();
	virtual int init(double *, int);
	virtual int configure();
	virtual int run();
	void addgrain();
	FunctionParser* getfunc(int index);
	FunctionParser* getabfunc(int index);
	double callfunc(FunctionParser* fp, double max, double min, double last, double u1, double u2, double u3, double u4, double a, double b);
	double callabfunc(FunctionParser* fp, double last, double u1, double u2, double u3, double u4, double a, double b);
	void resetgrain(Grain* grain);

private:
	bool configured;
	int branch;

	double x1;
	double x2;

	FunctionParser* funcA;
	double lastA;

	FunctionParser* funcB;
	double lastB;

	FunctionParser* funcRate;
	double minRate;
	double maxRate;
	FunctionParser* funcDur;
	double minDur;
	double maxDur;
	FunctionParser* funcFreq;
	double minFreq;
	double maxFreq;
	FunctionParser* funcAmp;
	double minAmp;
	double maxAmp;
	FunctionParser* funcPan;
	double minPan;
	double maxPan;

	double lastAmp;
	double lastRate;
	double lastDur;
	double lastFreq;
	double lastPan;

	float amp;

	std::vector<Grain*>* grains;
	int grainLimit;
	int newGrainCounter;


	double* wavetable;
	int wavetableLen;
	double* grainEnv;
	int grainEnvLen;
	float grainRate;
	void doupdate();
};

