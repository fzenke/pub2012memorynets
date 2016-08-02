# pub2012memorynets

This is the simulation code which I used to generate data that Richard Naud
plotted in Figure 17.11 of the spiking Hopfield network in Gerstner, W.,
Kistler, W.M., Naud, R., and Paninski, L. (2014). Neuronal dynamics: from
single neurons to networks and models of cognition (Cambridge: Cambridge
University Press).

## Files

It includes
* This file README.md
* The simulation files `sim_hopfield.cpp` and `sim_hopfield2.cpp`
* A `Makefile` to build them
* A `run.sh` file which illstrates the usage of the simulation
* A simple script to visualize output `plot_activity.gnu`
* The patterns I used in the book in the directory `./stim`
* Some additional files `history.txt` and `params_sim2.txt` with some parameter values that worked for me to make the original figure in the book


## Compile

The version I am publishing here has been adapted to compile with Auryn v0.8,
but you find older revisions of the code in the commit history. 

Either way you will need Auryn (fzenke.net/auryn) installed for this to run.
You can build the simulation code with `make` after having updated the
`Makefile` you point to your Auryn library.  


## Example

Here is an example
```
fzenke@grumpy:~/archive/2012-book-wulfram/spiking_hopfield/src$ make && mpirun -n 2 ./sim_hopfield2 --load ../stim/wulfp3.pat --chi 0.3 --gamma 0.2  --wei1 0.15 --wei2 0.1 --simtime 30
make: Nothing to be done for `all'.
load from matrix ./stim/wulfp3.pat.
chi from matrix 0.3.
gamma factor 0.2.
wei1 factor 0.15.
wei2 factor 0.1.
simtime 30.
load from matrix ./stim/wulfp3.pat.
chi from matrix 0.3.
gamma factor 0.2.
wei1 factor 0.15.
wei2 factor 0.1.
simtime 30.
( 0) Setting up neuron groups ...
( 0) Setting up external connections ...
( 0) Setting up EE connections ...
( 0) Setting up EI connections ...
( 0) Setting up IE connections ...
(!!) on rank 1: I1E (Connection.cpp): Some elements of pattern 0 are larger than the underlying NeuronGroups!
(!!) on rank 0: I1E (Connection.cpp): Some elements of pattern 0 are larger than the underlying NeuronGroups!
( 0) Setting up II connections ...
( 0) Setting up stimulus ...
( 0) Setting up monitors ...
( 0) Simulating ...
[=========================] 100%      t=30.0s  f=10.9 Hz in NeuronGroup   
( 0) Freeing ...
```

which will produce a bunch of output files. With `plot_activity.gnu you see how this indeed switches between different
patterns. 
```
fzenke@grumpy:~/archive/2012-book-wulfram/spiking_hopfield/src$ gnuplot 

	G N U P L O T
	Version 5.0 patchlevel 1    last modified 2015-06-07 

	Copyright (C) 1986-1993, 1998, 2004, 2007-2015
	Thomas Williams, Colin Kelley and many others

	gnuplot home:     http://www.gnuplot.info
	faq, bugs, etc:   type "help FAQ"
	immediate help:   type "help"  (plot window: hit 'h')

Terminal type set to 'wxt'
gnuplot> load 'plot_activity.gnu'
```

The output should look somewhat like this.
![Example plot](/images/plot_activity.png)

You can see that there is a pattern switch happening in those 30s.
Firing rates become "less crazy" with paramter tuning and as you load more
patterns (there are files with different numbers of random patterns in
``./stim``) which unfortunately always requires retuning.

## Tuning parameters

`run.sh` gives you an idea of how to run the simulation code. 

Moreover, `./sim_hopfield2 --help` will show you which paramters are exposed via CLI commands.

And `history.txt` as well ass `params_sim2.txt` includes some parameter with
which the code worked. However, There were some changes to Auryn over the
years.One included a bug which made the connecting probability slightly smaller
that was fixed. By using a newer version of Auryn where these problems are
fixed will thus also necessitate to  retuned the critical synaptic strength
parameters. However, the ones in `history.txt` and `params_sim2.txt` might be good starting points.


fzenke, Aug 02, 2016
