import pylab
import numpy
from scipy.optimize import curve_fit
from scipy import stats
from scipy.odr import odrpack
import math
#-------






#s, x = pylab.loadtxt('dft_data.txt', unpack = True)
s, x = pylab.loadtxt('dft_data_10s_001.txt', unpack = True)

gridsize = (3, 1)

ax1 = pylab.subplot2grid(gridsize,(0,0),colspan = 1, rowspan = 2)
ax1.set_title("grafico")
ax1.set_xlabel("t [s]")
ax1.set_ylabel("x [u.a.]")
ax1.grid(color = "gray")
ax1.grid(b=True, which='major', color='#666666', linestyle='-')
ax1.minorticks_on()
ax1.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

ax1.errorbar(s, x, 0, 0, color = "black", fmt = '.', label='Data')

def fit_function(w, F0, w0, gamma):
    return F0/pylab.sqrt((w0**2 - w**2)**2 + 4.0 * (gamma**2) * (w**2))
def fit_function_ODR(pars, w):
    return pars[0]/pylab.sqrt((pars[1]**2 - w**2)**2 + 4.0 * (pars[2]**2) * (w**2))



# Run the actual ODR.
model = odrpack.Model(fit_function_ODR)
data = odrpack.RealData(s, x)
odr = odrpack.ODR(data, model, beta0=(55.0, 4.68, 0.07))
out = odr.run()
popt, pcov = out.beta, out.cov_beta
F0, w0, gamma = popt
dF0, dw0, dgamma = numpy.sqrt(pcov.diagonal())
chisq = out.sum_square
# !Run the actual ODR.
# Run the actual ODR.
model = odrpack.Model(fit_function_ODR)
data = odrpack.RealData(s, x)
odr = odrpack.ODR(data, model, beta0=(F0, w0, gamma))
out = odr.run()
popt, pcov = out.beta, out.cov_beta
F0, w0, gamma = popt
dF0, dw0, dgamma = numpy.sqrt(pcov.diagonal())
chisq = out.sum_square
# !Run the actual ODR.


print('F0 = %f +- %f' %(F0, dF0))
print('w0 = %f +- %f' %(w0, dw0))
print('gamma = %f +- %f' %(gamma, dgamma))
print('chi2 = %f' %(chisq))

div = 100000
bucket = numpy.array([0.0 for i in range(div)])
retta = numpy.array([0.0 for i in range(div)])
inc = (s.max() - s.min())/div 
for i in range(len(bucket)):
        bucket[i]=float(i)*inc + s.min()
        retta[i] = fit_function(bucket[i], F0, w0, gamma)

ax1.plot(bucket, retta, color = "red", linestyle="--", label = "Fit con retta")

#------------------------------------------------------------


#---------------------------------------------------------------

#---------------------------------------------------------------
ax1.legend = pylab.legend(loc='upper left', shadow=True, fontsize='medium')

ax2 = pylab.subplot2grid(gridsize,(2,0), colspan = 2)
ax2.set_xlabel("1/p [1/m]")
ax2.set_ylabel("Residui [1/m]")
ax2.grid(color = "gray")
ax2.grid(b=True, which='major', color='#666666', linestyle='-')
ax2.minorticks_on()
ax2.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

ax2.errorbar(s, x - fit_function(s, F0, w0, gamma), 0, 0, color = "black", fmt = '.', label='residui (fit retta)')
ax2.plot(pylab.array([s.min(), s.max()]),pylab.array([0 , 0]),color = "red", linestyle="--", label = "zero")
ax1.legend = pylab.legend(loc='upper left', shadow=True, fontsize='medium')

pylab.show()
