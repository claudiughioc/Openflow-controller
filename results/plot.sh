#!/usr/bin/gnuplot 
reset
set terminal png
set ylabel "Time (sec)"


set title "Traffic bandwitdh with iperf"
set key reverse Left outside
set grid

set style data linespoints
set xlabel "Bandwidth (MB)"
set yrange[0:20]
set xrange[0:60]


set output 'bandwidth.png'
plot "out.dat" using 1:2 title "TCP", \
	"out.dat" using 1:3 title "MPTCP"
