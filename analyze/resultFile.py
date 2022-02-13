import csv


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
    file.close()


def creaFileFinale(file, f1, f2, f3, f4, file_finale):

    with open(file, mode='r') as f:
        reader = csv.reader(f)
        header = next(reader)
    c = []
    for e in header:
        c.append(e)

    with open(file_finale, 'a') as f_object:
        writer_object = csv.writer(f_object)
        writer_object.writerow(c)
        f_object.close()

    unisci_file(file, c, file_finale)
    unisci_file(f1, c, file_finale)
    unisci_file(f2, c, file_finale)
    unisci_file(f3, c, file_finale)
    unisci_file(f4, c, file_finale)


def creaFileFinaleSita(file, f1, f2, f3, file_finale):

    with open(file, mode='r') as f:
        reader = csv.reader(f)
        header = next(reader)
    c = []
    for e in header:
        c.append(e)

    with open(file_finale, 'a') as f_object:
        writer_object = csv.writer(f_object)
        writer_object.writerow(c)
        f_object.close()

    unisci_file(file, c, file_finale)
    unisci_file(f1, c, file_finale)
    unisci_file(f2, c, file_finale)
    unisci_file(f3, c, file_finale)


def creaFileFinaleRepliche(f1, f2, f3, f4, f5, f6, file_finale):
    with open(f1, mode='r') as f:
        reader = csv.reader(f)
        header = next(reader)
    c = []
    for e in header:
        c.append(e)

    with open(file_finale, 'a') as f_object:
        writer_object = csv.writer(f_object)
        writer_object.writerow(c)
        f_object.close()

    unisci_file(f1, c, file_finale)
    unisci_file(f2, c, file_finale)
    unisci_file(f3, c, file_finale)
    unisci_file(f4, c, file_finale)
    unisci_file(f5, c, file_finale)
    unisci_file(f6, c, file_finale)
