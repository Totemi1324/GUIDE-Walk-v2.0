import matplotlib.pyplot as plt
import csv
import numpy as np


colors = ["dimgrey", "darkgray", "lightgrey", "firebrick", "orangered", "salmon", "goldenrod", "gold", "khaki", "forestgreen", "limegreen", "chartreuse", "darkblue", "royalblue", "cornflowerblue"]

def plot_data(csv_file, x_range, values):
    y_data = []
    for i in range(15):
        y_data.append([])
    x_data = []
    for i in range(2700):
        x_data.append(i)
    labels = []
    line_counter = 0

    with open(csv_file, "r") as csv_f:
        reader = csv.reader(csv_f, delimiter=",")
        for row in reader:
            #print("Checking row " + str(line_counter))
            for i in range(len(row)):
                #print("Checking element " + str(i))
                if line_counter == 0:
                    labels.append(str(row[i]))
                else:
                    if i in values:
                        #print("Current i (" + str(i) + ") is in values; appending.")
                        y_data[i].append(float(row[i]))
                    
            line_counter += 1
    '''
    accel_sum = []
    for i in range(len(y_data[3])):
        accel_sum.append(y_data[3][i] + y_data[4][i] + y_data[5][i])
    accel_mean = []
    for i in range(len(y_data[3])):
        accel_mean.append((y_data[3][i] + y_data[4][i] + y_data[5][i]) / 3)
    '''
    for i in range(15):
        if len(y_data[i]) >= 1:
            plt.plot(x_data, y_data[i], color=colors[i], label=labels[i])
    
    plt.xlabel("Time [frames]")
    plt.ylabel("Sensor data [*]")
    plt.xlim((x_range[0], x_range[1]))
    plt.legend()
    plt.show()
    
    return y_data[14][500:515], y_data[14][950:965]

def empirical_deviation(values):
    mean = np.mean(values)
    var_sum = 0
    for i in range(len(values)):
        var_sum += np.square(values[i] - mean)
    return np.sqrt((1 / (len(values) - 1)) * var_sum)

def mean_abs(values):
    mean = 0
    for i in range(len(values)):
        mean += abs(values[i])
    return mean / len(values)

def normalization(values):
    mean = np.mean(values)
    stdev = empirical_deviation(values)

    for i in range(len(values)):
        values[i] = (values[i] - mean) / stdev

    return values

if __name__ == "__main__":
    s1, s2 = plot_data("sensor_data_indoor.csv", [0, 2700], [14])
    print(s2)
    print(normalization(s2))
