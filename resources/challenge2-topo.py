from mininet.topo import Topo
from mininet.link import TCLink
from mininet.util import irange

class Challenge2Topo( Topo ):

	def __init__( self ):
		# Initialize topology
		Topo.__init__( self )
		
		hosts    = [None] * 4
		switches = [None] * 5
		
		# Add hosts and switches
		for i in irange(1, 3):
			hosts[i]    = self.addHost('h%s' % i)
			switches[i] = self.addSwitch('s%s' % i)
		switches[4] = switches[1] #for the purpose of wrapping around
		
		# Add links
		for i in irange(1, 3):
			# Link 1 (host to switch)
			self.addLink(hosts[i], switches[i], port1 = 0, port2 = 0, bw = 20)
			#Links 2 (link to the next switch) and 3 (link to the previous switch)
			self.addLink(switches[i], switches[i + 1], port1 = 1, port2 = 2, bw = 10)

topos = { 'challenge2_topo': ( lambda: Challenge2Topo() ) }
