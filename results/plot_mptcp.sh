#!/usr/bin/gnuplot 
reset
set terminal png
set xlabel "Time (sec)"


set title "Traffic bandwitdh with iperf 3 clients"
set key reverse Left outside
set grid

set style data linespoints
set ylabel "Bandwidth (Mb/s)"
set yrange[0:20]
set xrange[0:60]


set output 'mptcp_3iperf.png'
plot "mptcp_3iperf.dat" using 1:2 title "h1->h3", \
	"mptcp_3iperf.dat" using 1:3 title "h2->h3", \
	"mptcp_3iperf.dat" using 1:4 title "h2->h3"
