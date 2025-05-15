import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import sawtooth


def convert_from_amp_to_coeff_frottement(hap2u_amp):
    amp = (hap2u_amp/255)*2 # en um
    mu0 = 1.2
    tau = 1.32
    return mu0*np.exp(amp**2/(2*tau**2))


def average(hap2u_amp):
    return (convert_from_amp_to_coeff_frottement(hap2u_amp) - convert_from_amp_to_coeff_frottement(hap2u_amp-201))/2


print("highest (255) : ", average(255))
print("2nd highest (228) : ", average(228))
print("3rd highest (201) : ", average(201))

# Define time vector
t = np.linspace(0, 1, 500)  # 1 second duration with 500 points

# Generate sawtooth waves with different average values
signal1 = sawtooth(2 * np.pi * 5 * t)  # 5 Hz frequency
signal2 = sawtooth(2 * np.pi * 10 * t) + 1  # 10 Hz frequency, shifted up

# Compute average values
avg1 = np.mean(signal1)
avg2 = np.mean(signal2)

# Plot the signals
plt.plot(t, signal1, label="5 Hz Sawtooth", linestyle="solid")
plt.plot(t, signal2, label="10 Hz Sawtooth (Shifted)", linestyle="solid")

# Draw horizontal lines for average values
plt.axhline(avg1, color='blue', linestyle='dotted', label="Avg 5 Hz")
plt.axhline(avg2, color='orange', linestyle='dotted', label="Avg 10 Hz")

# Add a horizontal line with arrows to indicate one period of the 5 Hz sawtooth
period = 1 / 5  # Period of 5 Hz signal
ax = plt.gca()

# Position the arrow below the x-axis
plt.annotate('', xy=(0, -0.15), xytext=(period, -0.15), 
             arrowprops=dict(arrowstyle='<->', color='black'), 
             xycoords=('data', 'axes fraction'), textcoords=('data', 'axes fraction'))
plt.text(period / 2, -0.18, "Period of 5 Hz", ha='center', va='center', transform=ax.transAxes)

plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.title("Sawtooth Waves with Different Periods and Average Values")
plt.ylim(min(signal1) - 0.3, None)  # Adjust y-axis to fit the annotation
plt.legend()
plt.show()
