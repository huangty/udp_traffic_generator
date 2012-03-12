#!/usr/bin/env python

from rpy2.robjects.vectors import DataFrame
from rpy2.robjects.packages import importr
import rpy2.robjects as R
import getopt
import sys
import pylab
import matplotlib.pyplot as plt
from types import *

##Print usage guide
def usage():
    """ Display usage
    """
    print "Usage "+sys.argv[0]+" [options] <filename>"
    print "\t pcap file should contain packets for parsing"
    print 
    print "Options:"
    print "-H/--host\n\t Specify the host ip for pcap filter"
    print "-p/--port\n\t Specify the host port for pcap filter"
    print "-h/--help\n\t Print this usage guide"



##Parse options and arguments
try:
    opts, args = getopt.getopt(sys.argv[1:], "H:p:h",
                               ["host", "port", "help"])
except getopt.GetoptError:
    print "Option error!"
    usage()
    sys.exit(2)

host_ip = ""
host_port = ""
#Parse options
for opt, arg in opts:
    if opt == "":
        break
    elif (opt in ("-h","--help")):
        usage()
        sys.exit(0)
    elif (opt in ("-H", "--host")):
        host_ip = arg
    elif (opt in ("-p", "--port")):
        host_port = arg
    else:
        print "Unhandled option :"+opt
        sys.exit(2)

if (len(args) < 1):
    print "Missing filename"
    usage()
    sys.exit(2)

fileroot = args[0]

R.r("data <- read.table(\"%s\", header=TRUE)" % (fileroot))
R.r("seq <- data[,3]")
R.r("diff <- seq[-1] - seq[1:length(seq)-1]")

diff = list(R.globalenv['diff'])
plt.plot(diff, '+-')

plt.show()
