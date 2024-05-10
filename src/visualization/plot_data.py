import matplotlib.pyplot as plt
import numpy as np

def read_output(output_file):
    data = {'k': [], 's': [], 'size': [], 'TrainingTime': [], 'Accuracy': []}
    with open(output_file, 'r') as f:
        next(f)  # skip header
        for line in f:
            k, s, size, trainingTime, accuracy = line.strip().split('\t')
            k = float(k)
            s = float(s)
            size = float(size)
            accuracy = float(accuracy)
            trainingTime = float(trainingTime)
            # Check if this is the first entry for this k or if accuracy is higher
            if k not in data['k'] or accuracy > max(data['Accuracy'][i] for i, k_val in enumerate(data['k']) if k_val == k):
                # Remove previous entries for this k
                indices = [i for i, k_val in enumerate(data['k']) if k_val == k]
                for idx in indices:
                    for key in data.keys():
                        del data[key][idx]
                # Append the new entry
                data['k'].append(k)
                data['s'].append(s)
                data['size'].append(size)
                data['Accuracy'].append(accuracy)
                data['TrainingTime'].append(trainingTime)
    return data


import matplotlib.pyplot as plt 

def plot_data(data):
    plt.figure(figsize=(15, 5))
    
    # Plot k vs accuracy
    plt.subplot(1, 3, 1)
    plt.plot(data['k'], data['Accuracy'], marker='s', linestyle='--', color='cornflowerblue', label='Accuracy')
    plt.xlabel('k',  fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.ylabel('Accuracy',  fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.title('k vs Accuracy',  fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.grid(linestyle='--') 
    plt.legend(loc='upper left')  # Moved legend to top left


    # Plot k vs size with varying color
    plt.subplot(1, 3, 2) 
    colors = plt.cm.Oranges(np.linspace(0.2, 1, len(data['k'])))  # Vary from light red to dark red
    for i in range(len(data['k'])):
        plt.plot(data['k'][i], data['size'][i], marker='s', linestyle='--', color=colors[i])
    
    for i in range(len(data['k'])):    
        plt.plot(data['k'][i:i+2], data['size'][i:i+2], linestyle='--', color=colors[i])
    plt.plot(data['k'][i:i+2], data['size'][i:i+2], linestyle='--', color=colors[i],  marker='s', label='Model Size (Mb)')
    plt.xlabel('k',  fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.ylabel('Model Size (Mb)',  fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.title('k vs Model Size (Binary File) ',  fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.grid(linestyle='--') 
    plt.legend(loc='upper left')  # Moved legend to top left

    # Plot k vs training time
    plt.subplot(1, 3, 3)
    colors = plt.cm.Oranges(np.linspace(0.2, 1, len(data['k'])))  # Vary from light red to dark red
    for i in range(len(data['k'])):
        plt.plot(data['k'][i], data['TrainingTime'][i], marker='s', linestyle='--', color=colors[i])
    
    for i in range(len(data['k'])):    
        plt.plot(data['k'][i:i+2], data['TrainingTime'][i:i+2], linestyle='--', color=colors[i])
        
    plt.plot(data['k'][i:i+2], data['TrainingTime'][i:i+2], linestyle='--', color=colors[i],  marker='s', label='Training Time (s)')
    plt.xlabel('k', fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.ylabel('Training Time (s)', fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.title('k vs Training Time', fontdict={'fontweight': 'normal', 'fontfamily': 'serif'})
    plt.grid(linestyle='--') 
    plt.legend(loc='upper left')  # Moved legend to top left

 
    
    plt.tight_layout()
    plt.show()
    
    


output_file = "output.txt"
data = read_output(output_file)
plot_data(data)
