




#use kelvin temperature algo.

#4.7k pullup res @ 3.3v
# def adc2ntc_kohm(raw):
#     r1 = 4.7
#     rntc = 100
#     # adc = rntc / (rntc + r1) * 4096
#     # rntc = adc / 4096
#     a = raw / 4096
#     rntc = r1 / ( 1-a )
#     return rntc

# def Rntc2temp(Rntc):
#     lnr = math.log(R25) - math.log(Rntc) 
#     a = lnr / B
#     #1/T25 - 1/Tn = a / B
#     tn = a - 1 / T25
#     return 1/tn

import math

T25 = 298.15
R25 = 100
B = 3950    

def ADC2temp(adc):
    v = adc / 4096
    # v = r/(r+4.7)
    # rv + 4.7v = r
    # r = 4.7v / (1-v)
    r = 4.7 * v / (1-v)
    ln = math.log(r) - math.log(R25)
    t1 = ln / B + 1 / T25
    return 1/t1

def RntcAtTemp(temp):

    # R = R0 * exp(B(1/T - 1/T0))
    R0 = 100
    dt1 = 1 / temp
    dt2 = 1 / T25
    return R0 * math.exp(B * (dt1 - dt2))

def ADCAtTemp(temp):
    r = RntcAtTemp(temp)
    v = r/(r+4.7)
    return v * 4096

# Rntc = adc2ntc_kohm(4095)

temp_zero_k = 273.15
temp = temp_zero_k + 30
print("30 = %d K" % (T25+5))

Rntc = RntcAtTemp(temp)
print('Rntc value = %f' % Rntc)

raw = ADCAtTemp(temp)
print("raw = %f" % raw)

temp = ADC2temp(3.13/3.3*4096)
print('3.13v = temp = %fK' % temp)


