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

probon = 0

amp = maketable("line", 1000, 0, 0, 1, 1, 16, 1, 17, 0)
layers = 10
ratefunc = "0.01 + (1-u1) / 10"
ratemin = 0.001
ratemax = 1

durfunc = "0.01 + (1-u1) / 10"
durmin = 0.0001
durmax = 1

freqfunc = "2^(7.781 + b)"


freqmin = 20
freqmax = 20000

ampfunc = "1.0"
ampmin = 0
ampmax = 1.1

panfunc = "0.5"
panmin = 0
panmax = 1
if (probon){
    x1 = 20 // tightness
    x2 = 5 // range
}
else
{
    x1 = 0.2 // std dev
    x2 = 0
}


afunc = "if(u4>0.5, 1, 0-1)"
if (probon){
    bfunc = "a*x2*u3^x1"
    print("prob on")
}
else {
    bfunc = "sqrt(-2*log(u3)*sin(2 * 3.1415 * u4)) * x1" // box muller transform
    print("prob off")
}



wave = maketable("wave", 1000, "sine")
env = maketable("window", 1000, "hanning")
for (i=0; i<layers;i=i+1){
	PARSEGRAN(outskip, dur,  2000 * amp, ratefunc, ratemin, ratemax, durfunc, durmin, durmax, 
	freqfunc, freqmin, freqmax, ampfunc, ampmin, ampmax, panfunc, panmin, panmax, wave, env, x1, x2, afunc, bfunc)
}