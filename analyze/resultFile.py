import csv

file_finale = "dataset/resultSimulation.csv"

res = "dataset/example.csv"
res1 = "dataset/example1.csv"
res2 = "dataset/example2.csv"
res3 = "dataset/example3.csv"
res4 = "dataset/example4.csv"


def unisci_file(f, c, new_file):
    file = open(f, 'r')
    text = file.readlines()
    file.close()
    file = open(new_file, 'a')
    for line in text:
        if c[0] not in line:
            if not "\n" in line:
                file.write(line+"\n")
            else:
                file.write(line)
            print(line)
    file.close()


if __name__ == "__main__":

    with open("dataset/example.csv", mode='r') as f:
        reader = csv.reader(f)
        header = next(reader)
    c = []
    for e in header:
        c.append(e)
    print(c)

    with open(file_finale, 'a') as f_object:
        writer_object = csv.writer(f_object)
        writer_object.writerow(c)
        f_object.close()

    unisci_file(res, c, file_finale)
    unisci_file(res1, c, file_finale)
    unisci_file(res2, c, file_finale)
    unisci_file(res3, c, file_finale)
    unisci_file(res4, c, file_finale)
