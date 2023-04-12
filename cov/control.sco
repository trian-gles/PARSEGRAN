rtsetparams(44100, 2)
load("../libPARSEGRAN.so")

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
		p12: ampExpr
		p13: ampMin
		p14: ampMax
		p15: panExpr
		p16: panMin
		p17: panMax
		p18: wavetable**
		p19: grainEnv**
		p20: x1* (optional)
		p21: x2* (optional)
		p23: funcA (optional)
		p24: funcB (optional)
		p25: grainLimit=1500 (optional)
	*/
outskip = 0
dur = 10

amp = maketable("line", 1000, 0, 0, 1, 1, 16, 1, 17, 0)
layers = 1

ratefunc = "u1/100"
ratemin = 0.0001
ratemax = 1

durfunc = "u2/50"
durmin = 0.0008
durmax = 1

freqfunc = "2^(u3*5+8)"
freqmin = 20
freqmax = 20000

ampfunc = "1"
ampmin = 0
ampmax = 1

panfunc = "u4"
panmin = 0
panmax = 1


wave = maketable("wave", 1000, "sine")
env = maketable("window", 1000, "hanning")
for (i=0; i<layers;i=i+1){
	PARSEGRAN(outskip, dur,  2000 * amp, ratefunc, ratemin, ratemax, durfunc, durmin, durmax, 
	freqfunc, freqmin, freqmax, ampfunc, ampmin, ampmax, panfunc, panmin, panmax, wave, env)
}



