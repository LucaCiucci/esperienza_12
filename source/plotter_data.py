import pylab
import numpy
from scipy.optimize import curve_fit
from scipy import stats
from scipy.odr import odrpack
import math
#-------



number = '65_1.txt'

#s, x = pylab.loadtxt('../../dati/45_1.txt', unpack = True) # schifo sulle creste
#s, x = pylab.loadtxt('../../dati/60_1.txt', unpack = True) # cambio velocità, dft interessante
#s, x = pylab.loadtxt('../../dati/65_1.txt', unpack = True) # il miglior esempio, fft interessante con picchi multipli
#s, x = pylab.loadtxt('../../dati/70_1.txt', unpack = True) # la velocità aumenta
#s, x = pylab.loadtxt('../../dati/71_1.txt', unpack = True) # la velocità supera il picco DFT interessante (forse)
#s, x = pylab.loadtxt('../../dati/72_1.txt', unpack = True) # interferenza fra oscillazione a w e w0, dft interessante
#s, x = pylab.loadtxt('../../dati/73_1.txt', unpack = True) # interferenza fra oscillazione a w e w0, poi aumenta velocità, dft interessante

s, x = pylab.loadtxt('../../dati/' + number, unpack = True)
w, A = pylab.loadtxt('dft/' + number, unpack = True) # DFT

gridsize = (3, 1)

ax1 = pylab.subplot2grid(gridsize,(0,0),colspan = 1, rowspan = 2)
ax1.set_title("run " + number + "")
#ax1.set_xlabel("t [s]")
ax1.set_ylabel("x [u.a.]")
ax1.grid(color = "gray")
ax1.grid(b=True, which='major', color='#666666', linestyle='-')
ax1.minorticks_on()
ax1.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

ax1.errorbar(s, x, 1.0, 0.001, color = "black", fmt = '.', label='Data')


#---------------------------------------------------------------
ax1.legend = pylab.legend(loc='upper left', shadow=True, fontsize='medium')


ax2 = pylab.subplot2grid(gridsize,(2,0), colspan = 2)
ax2.set_xlabel("omega")
ax2.set_ylabel("DFT [u.a.]")
ax2.grid(color = "gray")
ax2.grid(b=True, which='major', color='#666666', linestyle='-')
ax2.minorticks_on()
ax2.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

ax2.plot(w, A, color = "black", ls = '-', label='DFT')
ax1.legend = pylab.legend(loc='upper left', shadow=True, fontsize='medium')

pylab.show()
