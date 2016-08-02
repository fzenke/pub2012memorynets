#!/usr/bin/gnuplot

# Simple plotting file which relies on the peep.sh script to crop out bits of ras files.
# The script can be found here https://github.com/fzenke/malleable

set multiplot layout 2,1
set key horizontal
set yrange [*:*]
plot for [i=1:11] '< peep.sh ./spikehopf.0.pact 50' using 1:2*i title sprintf("%i",i) w l

unset key
set yrange [:100]
set xlabel "Time"
plot '< peep.sh ./spikehopf.0.e.ras 50 5000000' w p pt 7 lc -1 ps 0.2

# unset key
# set yrange [*:*]
# plot '< peep.sh ./spikehopf.0.e.ras 1 5000000' w d lc -1

unset multiplot
