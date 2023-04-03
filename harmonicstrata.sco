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

amp = maketable("line", 1000, 0, 0, 1, 1, 16, 1, 17, 0)
x1 = maketable("line", "nonorm", 1000, 0, 0, 1, 16)


ratefunc = "u1/1000"
ratemin = 0.0001
ratemax = 1

durfunc = "0.05"
durmin = 0.0001
durmax = 1

freqfunc = "100 * int(u2 * 16) * (0.95 + u3 / 10)"
freqmin = 20
freqmax = 20000

ampfunc = "((0.5 - abs(0.5 - u3)) * 2)^2"
ampmin = 0
ampmax = 1

panfunc = "(sin((u2+x1) * 2 * 3.1415926) + 1) / 2"
panmin = 0
panmax = 1


wave = maketable("wave", 1000, "sine")
env = maketable("window", 1000, "hanning")

PARSEGRAN(outskip, dur,  2000 * amp, ratefunc, ratemin, ratemax, durfunc, durmin, durmax, 
freqfunc, freqmin, freqmax, ampfunc, ampmin, ampmax, panfunc, panmin, panmax, wave, env, x1)


