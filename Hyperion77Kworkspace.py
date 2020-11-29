import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from scipy.interpolate import interp1d
#from HyperionUtilies import HyerionDataset
#%%
#Temperature
T =   [21.4 ,   20.5,   17.6,  14.3,   12.2,   11.0,    6.0,    1.1,    1.2,  -14.0,  -21.0,  -27.0,  -34.0,  -42.0, -46.5,
       -51.0,  -57.0,  -61.6,  -64.0,  -72.4,  -85.0,   -121, -131.7,  -148.0,   -194.6,  -186.4,  -183.6,  -170.3,  -164.2,
       -153.3, -141.0, -128.6, -117.6, -107.8,  -99.0,  -91.1,  -84.2,  -77.2,  -70.7,   -64.5,  -59.1,  -54.1,  -49.6,  -45.2,
       -40.8,  -36.8,  -33.2,  -30.0,  -26.7,  -23.7,  -20.7,  -17.3,  -14.9,   -11.4,   -9.7,   -6.8,   -4.5,   -2.2,   -6.3,
        1.9,    3.6,    5.1,    6.4,    7.5,   10.2,  11.6]
#%%
#Recorded time in (minutes, seconds)
t_ms =   [(0,0), (0,30), (0,50), (1,0), (2,20), (2,30), (2,50), (3,10), (3,40), (4,10), (4,30), (4,50), (5,10), (5,40), (6,0),
       (6,20), (6,40), (7,10), (7,30), (8,00), (9,20), (9,45), (10,0), (10,15), (10,30), (13,20), (13,40), (15,10), (16,00),
       (17,0), (18,0), (19,0), (20,0), (21,0), (22,0), (23,0), (24,0), (25,0), (26,0), (27,0), (28,0), (29,0), (30,0), (31,0),
       (32,0), (33,0), (34,0), (35,0), (36,0), (37,0), (38,0), (39,0), (40,0), (41,22), (42,0), (43,0), (44,0), (45,0), (46,0),
       (48,0), (49,0), (50,0), (51,0), (52,0), (55,0), (57,0)]
#lambda function to convert minutes:seconds to 60*minutes + seconds
ms = lambda m,s: float(m)*60.0 + float(s)
t = [ms(m,s) for (m,s) in t_ms]
#%%
plt.figure()
plt.plot(t, T)#plot time vs Temp
#%%
tempFunc = interp1d(t, T, bounds_error=None, fill_value='extrapolate')
#%%
#hyperion data:
fname = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\10\Responses.20201021164957.txt"
q = HyperionDataset(fname)
#%%
# plt.figure()
# dotRng = [1544,5553]
# dots0 = np.dot(q.data[0,dotRng[0]:dotRng[1] ],q.data[0,dotRng[0]:dotRng[1]])
# dots = []
# legends = [str(x) for x in range(len(q.data))]
# for i in range(len(q.data)):
#     dots.append( np.dot(q.data[0,dotRng[0]:dotRng[1]], q.data[i,dotRng[0]:dotRng[1]]) / dots0 )
# for i in range(len(dots)):
#     plt.plot(q.time[i],dots[i],'b.',label=legends[i])

# #plt.legend()

# plt.ylabel("Dimensionless Temp Detection Signal", fontsize = 14)
# plt.xlabel("Time (s)",fontsize = 14)
# print(dots)
#%%
def calcDetSignal(q, dotRng_ind = [4200,5100], toPlot=True):
    dots0 = np.dot(q.data[0,dotRng_ind[0]:dotRng_ind[1] ],q.data[0,dotRng_ind[0]:dotRng_ind[1]])
    dots = []
    legends = [str(x) for x in range(len(q.data))]
    for i in range(len(q.data)):
        dots.append( np.dot(q.data[0,dotRng_ind[0]:dotRng_ind[1]], q.data[i,dotRng_ind[0]:dotRng_ind[1]]) / dots0 )

    if toPlot:
        plt.figure()
        for i in range(len(dots)):
            plt.plot(q.time[i],dots[i],'b.',label=legends[i])
        plt.ylabel("Dimensionless Temp Detection Signal", fontsize = 14)
        plt.xlabel("Time (s)",fontsize = 14)
    return dots




#%%
plt.figure()
plt.plot(t,T)
plt.xlabel('Time (s)', fontsize = 14)
plt.ylabel('Temperature (C)', fontsize = 14)
#%%

#%%

#def colorSpecPlot(specData, skip = 1, labels = None, wavelengthRng = [1548,1552]):
#    numPlots = specData.data.shape[0] // skip
#    color = cm.rainbow( np.linspace(0,1,numPlots) )
#    if labels is None:
#        labels = [None]*numPlots
#    for (spec,c,label) in zip(specData.data[:-1:skip], color, labels):
#       plt.plot( specData.waveLengths, spec.data)
#    plt.xlim([wavelengthRng[0], wavelengthRng[1]])



#%%

def colorTempPlot(specData, labelFunction, skip = 1, wavelengthRng = [1548,1552], startOffset = 0):
    plt.figure()
    numPlots = (specData.data.shape[0] -startOffset) // skip
    color = cm.rainbow( np.linspace(0,1,numPlots+1) )
    #for (i,cindex) in zip(range(0,specData.data.shape[0], skip), range(numPlots)):
    for i in range( numPlots-1):
        specInd = i*skip + startOffset
        theLabel = labelFunction(specData.time[specInd])
        label = '{:3.2f} C'.format(theLabel)
        plt.plot( specData.waveLengths, specData.data[specInd], color = color[i], label = label)
    if labelFunction(0) != -1: #disable if not passing labels
        plt.legend(prop={'size':10})
    plt.xlim([wavelengthRng[0], wavelengthRng[1]])
    #plt.tight_layout()



#%%
indRng = [4200,5200]
#%%
#print(  np.dot( q.waveLengths[ indRng[0]:indRng[1] ], -d[ indRng[0]:indRng[1] ] ) / np.sum(-d[ indRng[0]:indRng[1] ])  )

centerOfMass = []
peakWavelength = []
for d in q.data[110:]:
    cm = np.dot( q.waveLengths[ indRng[0]:indRng[1] ], -d[ indRng[0]:indRng[1] ] ) / np.sum(-d[ indRng[0]:indRng[1] ])
    centerOfMass.append(cm)
    peakWavelength.append( np.argmax(d) / 100.0 + 1500.0 )

centerOfMass = np.array(centerOfMass)
peakWavelength = np.array(peakWavelength)
#%%
plt.close('all')
#%%
plt.figure()
plt.subplot(2,1,1)
plt.plot(q.time[110:], q.centerOfMass[110:] )
plt.title("Center of Mass")
plt.ylabel("Wavelength (nm)", fontsize = 14)
plt.xlabel("Time (s)", fontsize = 14)
plt.tight_layout()
plt.subplot(2,1,2)
plt.plot(tempFunc(q.time[110:]), q.centerOfMass[110:], 'x')
plt.ylabel("Wavelength (nm)", fontsize = 14)
plt.xlabel("Temp (C)", fontsize = 14)
plt.title("Center of Mass")
plt.tight_layout()
#%%
plt.figure()
plt.subplot(2,1,1)
plt.plot(q.time[130:], q.peakWavelength[130:])
plt.title("Peak Wavelength")
plt.ylabel("Wavelength (nm)", fontsize = 14)
plt.xlabel("Time (s)", fontsize = 14)
plt.tight_layout()
plt.subplot(2,1,2)
plt.plot(tempFunc(q.time[130:]), q.peakWavelength[130:], 'x')
plt.ylabel("Wavelength (nm)", fontsize = 14)
plt.xlabel("Temp (C)", fontsize = 14)
plt.title("Peak Wavelength")
plt.tight_layout()
#%%
lambda_coeffs = np.polyfit( tempFunc(q.time[130:]), q.peakWavelength[130:], 2)
tempFit = np.linspace(tempFunc(q.time[130:]),10,100)
fitLine = np.polyval( lambda_coeffs, tempFit )

#plt.subplot(2,1,1)
plt.plot(tempFit, fitLine, 'k')
plt.title('lambda(T) = -2.13e-05 T^2 + 5.84 e-03 T + 1500 nm \n\n ~ 5.8 pm / K ')
#%%%%%%%%%%%%%%%%%
#fname = r"C:\Users\dmeichle\CFS Dropbox\CFS_Internal\R&D\CSMC\Fiberoptics\VPI with fibers\Spectrum collection 20201030\Responses.20201030160612_Solderflow+cooldown.txt"
plt.close('all')
q = HyperionDataset(channels=[1])
#q.plotSpec(wavelengthRng=[1500,1600])
colorTempPlot(q, lambda x: -1, wavelengthRng=[1540,1560],skip=10)
#%%
#cool down:
fname = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106180508.txt"
q = HyperionDataset(fname, channels=[1]); q.plotSpec(wavelengthRng=[1500,1600], skip=10);
colorTempPlot(q, lambda x: -1, wavelengthRng=[1540,1560],skip=10)
plt.plot(q.peakWavelength)


#%%
#next was ~1-2 min idle, then turn heater on 30V @ .19A = 5.7 W




#then turned heater off to cool back down.
fname = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106183148.txt"
q = HyperionDataset(fname, channels=[1]); q.plotSpec(wavelengthRng=[1500,1600], skip=10);
colorTempPlot(q, lambda x: -1, wavelengthRng=[1540,1560],skip=10)
plt.figure()
plt.plot(q.peakWavelength)
#%%
print( q.peakWavelength[0] - q.peakWavelength[-1], (q.peakWavelength[0] - q.peakWavelength[-1]) / .0058 )

#%%
#unknown: 
fname = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106175305.txt"
q = HyperionDataset(fname, channels=[1]); q.plotSpec(wavelengthRng=[1500,1600], skip=10);
colorTempPlot(q, lambda x: -1, wavelengthRng=[1540,1560],skip=10)
plt.figure()
plt.plot(q.peakWavelength)
#(1547.15 - 1546.96)/.0058 (cooling)
#32.75862068966458


#%%
plt.title("Detection signal vs time \n 5.7 Watts (4 kJ net) heat applied at 77 K initial", fontsize = 14)
plt.tight_layout()
#%%
indRng = [4200,5100]
plt.figure()
plt.plot(q.waveLengths[4200:5100], q.data[-1, 4200:5100],color = 'b', label = '77 Kelvin')
plt.plot(q.waveLengths[4200:5100], q.data[ 0, 4200:5100],color = 'r', label = '5.7 W after 700 s (4 kJ)')
plt.xlabel('Wavelength (nm)', fontsize = 14)
plt.ylabel('Reflected Power (dB)', fontsize = 14)
plt.title("Spectrum before and after heat applied", fontsize = 14)
plt.legend()
plt.tight_layout()
#%%

#%%
plt.close('all')
dataList = []
for o in range(0,5):
    q = HyperionDataset(channels=[1], recentFileOffset = -o);# q.plotSpec(wavelengthRng=[1500,1600], skip=10);
    plt.figure()
    plt.plot(q.peakWavelength)
    print(q.peakWavelength[0], q.peakWavelength[-1], q.peakWavelength[0] - q.peakWavelength[-1], (q.peakWavelength[0] - q.peakWavelength[-1]) / .0058 )
    plt.title([str(a) for a in [o, q.peakWavelength[0], q.peakWavelength[-1], q.peakWavelength[0] - q.peakWavelength[-1], (q.peakWavelength[0] - q.peakWavelength[-1]) / .0058 ]] )
    dataList.append(q)
    print(str(o), q.dataFile)
#%%:
ok so here is whats what: 
    

#Cool back down after heated
o=0
fname = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106183148.txt"


#Unknown - maybe hanging out at 77 or with heater saturated. will check spec
o=1
fname = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106174709.txt"


#Heater applied at 5.7 Watts for 700 seconds (only dataset with peak rising)
o=2
fname = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106175305.txt"

o=3
junk

#cool from room to 77K
o=4
fname = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106180508.txt"
#%%

coolDown216   = HyperionDataset(dataFileName = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106180508.txt", channels=[1])
heaterOn      = HyperionDataset(dataFileName = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106175305.txt", channels=[1])
coolAfterHeat = HyperionDataset(dataFileName = r"C:\Users\dmeichle\Documents\Micron Optics\ENLIGHT\Data\2020\11\Responses.20201106183148.txt", channels=[1])
#%%
data = [coolDown216, heaterOn, coolAfterHeat ]
titles = ['Cool from 293K to 77K', 'Heater on 5.7 W for 700 s', 'Heater Off  Cooling back to 77 K']
#%%
plt.close('all)')
#%%
for (q,theTitle) in zip(data, titles):
    plt.figure()
    #plt.plot(q.time, q.peakWavelength)
    plt.subplot(2,1,1)
    plt.plot( q.time, q.peakWavelength)
    plt.xlabel("Time (s)", fontsize = 14)
    plt.ylabel("Peak Wavelength", fontsize = 14)
    diagStr = 'Initial {:3.2f}  nm Final {:3.2f}  nm Change {:3.2f}  nm'.format( q.peakWavelength[0], q.peakWavelength[-1], -( q.peakWavelength[0] - q.peakWavelength[-1] ) )
    plt.title(theTitle + '\n' + diagStr)
    plt.grid()
    plt.tight_layout()

    plt.subplot(2,1,2)
    plt.plot(q.waveLengths, q.data[0, :], label = 'Initial')
    plt.plot(q.waveLengths, q.data[-1, :], label = 'Final')
    plt.xlabel('Wavelength (nm)', fontsize = 14)
    plt.ylabel('Reflected Power (dB)', fontsize = 14)
    plt.xlim([1542, 1551])
    plt.title(theTitle + '\n' + diagStr)
    plt.grid()
    plt.tight_layout()
#%%

colorTempPlot(data[1], lambda x:-1, skip = 25)
plt.title("Heater On")
plt.xlabel('Wavelength (nm)', fontsize = 14)
plt.ylabel('Reflected Power (dB)', fontsize = 14)
plt.grid()

#%%


colorTempPlot(data[0], lambda x:-1, skip = 10)
plt.title("Cool from 293K")
plt.xlabel('Wavelength (nm)', fontsize = 14)
plt.ylabel('Reflected Power (dB)', fontsize = 14)
plt.grid()
#%%


colorTempPlot(data[2], lambda x:-1, skip = 10)
plt.title("Heater Off")
plt.xlabel('Wavelength (nm)', fontsize = 14)
plt.ylabel('Reflected Power (dB)', fontsize = 14)
plt.grid()
























