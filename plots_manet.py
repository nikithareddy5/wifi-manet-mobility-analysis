
import matplotlib.pyplot as plt

speeds = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]  # m/s

throughput = [513.33, 513.30, 513.33, 505.07, 351.65, 295.37, 505.67, 218.02, 510.42, 507.80, 505.53,
              271.12, 500.49, 497.55, 300.93, 483.32, 344.97, 176.14, 333.25, 376.42, 220.55,
              234.49, 420.54, 350.53, 341.34]  # kbps
pdr = [98.66, 98.65, 98.66, 97.07, 67.59, 56.77, 97.19, 41.90, 98.10, 97.60, 97.16, 52.11, 96.19,
       95.63, 57.84, 92.89, 66.30, 33.85, 64.05, 72.35, 42.39, 45.07, 80.83, 67.37, 65.60]            # %
avg_delay = [1.28, 1.04, 0.97, 1.06, 0.99, 1.02, 1.45, 1.10, 1.57, 2.65, 1.34, 0.98, 5.67, 5.87, 
             3.94, 4.99, 9.76, 6.41, 8.83, 5.69, 1.80, 10.49, 8.82, 8.48, 6.83]        # ms

#  Plot Throughput vs Speed
plt.figure(figsize=(7,5))
plt.plot(speeds, throughput, marker='o', color='blue', linewidth=2)
plt.title('Speed vs Throughput')
plt.xlabel('Speed (m/s)')
plt.ylabel('Throughput (kbps)')
plt.grid(True)
plt.xticks(speeds)
plt.show()

# Plot PDR vs Speed
plt.figure(figsize=(7,5))
plt.plot(speeds, pdr, marker='s', color='green', linewidth=2)
plt.title('Speed vs Packet Delivery Ratio (PDR)')
plt.xlabel('Speed (m/s)')
plt.ylabel('PDR (%)')
plt.grid(True)
plt.xticks(speeds)
plt.show()

# Plot Avg Delay vs Speed
plt.figure(figsize=(7,5))
plt.plot(speeds, avg_delay, marker='^', color='red', linewidth=2)
plt.title('Speed vs Average End-to-End Delay')
plt.xlabel('Speed (m/s)')
plt.ylabel('Average Delay (ms)')
plt.grid(True)
plt.xticks(speeds)
plt.show()