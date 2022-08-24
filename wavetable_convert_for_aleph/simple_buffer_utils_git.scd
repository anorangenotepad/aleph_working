
//First we need to laod the sample in a buffer. We need to give the buffer a name ("c" in this case) to be able to reference it in our SynthDef.
//NOTE: you can do this with the wavetables, but it wont sound like much because just one cycle...
c=Buffer.read(s,"/path/to/directory/AKWF_flute_0010.wav")

//The sample has a total number of 67618 frames.
c.query

//what does it look ike?
c.plot

//Here we can test our sample (plays once, no loop)
c.play

//free the buffer
c.free

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

// Convert it to a Wavetable
a = a.asWavetable;
a.size; // 512 ok, (wavetable format is signal.size * 2

// END wavetable conversion code



//BEGIN example to play sound from a single buffer
// Server side
//s.boot;
b = Buffer.loadCollection(s, a);
x = b.play(loop: true); // ok sounds
//e = Env([1.0, 0.0], [0.5]);
//g = EnvGen.ar(e, doneAction: 2);
//x = { Osc.ar(b, 60.midicps, 5.6.mod(2pi), 0.3) }.play;

//for looping
x = {Osc.ar(b, MouseX.kr(100, 1000, 1), 0, 0.2!2)}.play;

//for oneshot
x = { Osc.ar(b, 45.midicps, 0, 1) * EnvGen.ar(Env.perc(0.1, 0.7)) !2}.play;

x.free;

b.free;




