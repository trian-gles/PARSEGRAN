rtsetparams(44100, 2)
load("./libPARSEGRAN.so")

        /* NEW Args:
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
	*/
outskip = 0
dur = 20
layers = 20

amp = maketable("line", 1000, 0, 0, 1, 1, 16, 1, 17, 0)

ratefunc = "l * (1 + (.5 - u1))"
ratemin = 0.0003
ratemax = 1

durfunc = "u2/30"
durmin = 0.0001
durmax = 1

freqfunc = "l * 2^(0.25 - u2 / 2)"
freqmin = 20
freqmax = 20000

ampfunc = "l * 2^(u4 - 0.5)"
ampmin = 0.1
ampmax = 1

panfunc = "u2"
panmin = 0
panmax = 1


wave = maketable("wave", 1000, "sine")
env = maketable("window", 1000, "hanning")
for (i = 0; i < layers; i=i+1)
{
	PARSEGRAN(outskip, dur,  1000 * amp, ratefunc, ratemin, ratemax, durfunc, durmin, durmax, 
	freqfunc, freqmin, freqmax, ampfunc, ampmin, ampmax, panfunc, panmin, panmax, wave, env)
}

