import oraFormatter

"""Formatta il datase in maniera opportuna"""


def formatter():
    file = open("dataset/ExcelSheetName.csv", 'r')
    text = file.readlines()
    file.close()
    new_file = [
        "Airport;Terminal;Date;Hour;Avarege Wait Time;Max Wait Time;0-15;16-30;31-45;46-60;61-90;91-120;120 PLUS;Excluded;Total;Flights;Booths\n"]

    for line in text:
        if "LAX" in line:
            app = line.split(";")
            ora = oraFormatter.formatHour(app[3])
            if "International" in app[1]:
                new_line = app[0] + \
                           ";" + app[1].replace(app[1], "International") + \
                           ";" + app[2] + \
                           ";" + ora + \
                           ";" + app[8] + \
                           ";" + app[9] + \
                           ";" + app[10] + \
                           ";" + app[11] + \
                           ";" + app[12] + \
                           ";" + app[13] + \
                           ";" + app[14] + \
                           ";" + app[15] + \
                           ";" + app[16] + \
                           ";" + app[17] + \
                           ";" + app[18] + \
                           ";" + app[19] + \
                           ";" + app[20]
                new_file.append(new_line)
            else:
                new_line = app[0] + \
                           ";" + app[1].replace(app[1], "National") + \
                           ";" + app[2] + \
                           ";" + ora + \
                           ";" + app[8] + \
                           ";" + app[9] + \
                           ";" + app[10] + \
                           ";" + app[11] + \
                           ";" + app[12] + \
                           ";" + app[13] + \
                           ";" + app[14] + \
                           ";" + app[15] + \
                           ";" + app[16] + \
                           ";" + app[17] + \
                           ";" + app[18] + \
                           ";" + app[19] + \
                           ";" + app[20]
                new_file.append(new_line)
    file = open("dataset/datasetLA.csv", 'w')
    text = file.writelines(new_file)
    file.close()
