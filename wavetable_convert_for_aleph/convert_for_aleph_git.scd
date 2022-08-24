
// 1) SAMPLE PREP CODE

//MORE COMPLEX FOR BUILDING WAVETABLE (NEED TO START WITH 1 CYCLE, SO USE AKWF wavetables)
f = SoundFile.openRead("/path/to/directory/epiano_0001.wav".standardizePath);

//for wavetables converted with wttool
//f = SoundFile.openRead("/path/to/directory/AKWF_theremin_0001sc.wav".standardizePath);


f.path // ok

// An array to load the data
a = FloatArray.newClear(f.numFrames);
f.readData(a);
f.close; // close the file
a.size; // 169 in my file


// resamp the table to have a pow of 2 (bigger to avoid aliassing)
// if u read many diff samples choose a bigger pow of 2
//DONT NEED TO DO IF ALREADY A POWER OF TWO...
a = a.resamp1(1024);



// Conver it to a Signal
a = a.as(Signal);
a.size; // 256 ok

// DONT Convert it to a Wavetable!
// JUST NEED IT AS SIGNAL!
//a = a.asWavetable;
//a.size; // 512 ok, (wavetable format is signal.size * 2


// END conversion code






// 2) SAMPLE CONVERT CODE

Routine {


n = 1024;
b = Buffer.alloc(s, n, 1);
0.25.wait;
// print
p = {
	b.getToFloatArray(wait:0.25, action:{|a, j|

		{ a.plot(j.asString) }.defer;

		a.do({ |x, i|
			"0x".post;
			(x * 0x7fffffff).floor.asInteger.asHexString.post;
			", ".post;
		});
	});
};

// sine
postln(" // 0: SAMPLE TABLE : ");
//NOTE! COMMENT THESE ( "{", "} ,") OUT OR WIL GET SCALAR ERROR DURING C COMPILE OF ALEPH MODULE
//ALSO DELETE LAST COMMA AT END!
//MAY NEED FOR WAVES MULTITABLE, BUT WILL JUST CONFUSE table_voice.c
//postln(" { ");
//b.sine1([1], asWavetable:false);
b.loadCollection(a);
0.25.wait;
p.value;
//" } , ".postln;
"".postln;



}.play;

//CHECK TOOLS

//for looping
//x = {Osc.ar(b, MouseX.kr(100, 1000, 1), 0, 0.2!2)}.play;

//for oneshot
//x = { Osc.ar(b, 35.midicps, 0, 1) * EnvGen.ar(Env.perc(0.1, 0.7)) !2}.play;

//x.free;

b.free;

