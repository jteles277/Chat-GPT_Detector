def parse_input(input_file):
    data = []
    with open(input_file, 'r') as f:
        lines = f.readlines()

    current_entry = {}
    for i, line in enumerate(lines):
        if line.startswith("k ="):
            if current_entry:
                data.append(current_entry)
                current_entry = {}

            k = line.split(",")[0].split("=")[1].split()[0]
            s = line.split(",")[1].split("=")[1].split()[0]
            size = lines[i + 1].split("=")[1].strip().split()[0]
            training_time = float(line.split(":")[1].split()[0])

            current_entry['k'] = k
            current_entry['s'] = s
            current_entry['size'] = size
            current_entry['Training time'] = training_time
        elif line.startswith("Accuracy"): 
            accuracy = float(line.split(":")[1])
            current_entry['Accuracy'] = accuracy

    if current_entry:
        data.append(current_entry)

    return data


def write_output(data, output_file):
    with open(output_file, 'w') as f:
        f.write("k\ts\tsize\tTrainingTime\tAccuracy\n")
        for entry in data: 
            if len(entry) < 5:
                continue
            f.write(f"{entry['k']}\t{entry['s']}\t{entry['size']}\t{entry['Training time']}\t{entry['Accuracy']}\n")


input_file = "evaluation_results_2.txt"
output_file = "output.txt"

data = parse_input(input_file)
write_output(data, output_file)
