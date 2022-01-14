import csv

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl

file = 'dataset/datasetLA.csv'

fSimulazione = 'dataset/resultSimulation.csv'

"""
    Read the dataset from a file
     
     f: file where the dataset is storage
     search_value: values to look for in the columns' values
     c_name: list of columns' name of the dataset
     read_value: column where to look the search_value
     d: delimiter used in the file
"""


def readDataset(f, search_value, c_name, read_value, d):
    try:
        df = pd.read_csv(f, usecols=c_name, sep=d)
        df1 = df[df[read_value] == search_value]
        return df1
    except:
        return False


"""
    Split the dataframe
    
    df: dataframe to split
    column_value: column's name into look 
    value: value to check for the split
"""

def splitDf(df, column_value, value):
    try:
        df1 = df[df[column_value] == value]
        return df1
    except:
        return False


def selectMultipleValue(df, column_value, offsetlist, column_name):
    try:
        df1 = pd.DataFrame(columns=column_name)
        for index in range(len(offsetlist)):
            df1 = df1.append(df[df[column_value] == offsetlist[index]], ignore_index=True)
        return df1
    except:
        return False


"""
    Compute the avarage 
    
    df: dataframe used
    value: column used for compute the average
"""


def average(df, value):
    try:
        avg = int(df[value].sum()) / len(df)
        return avg
    except:
        return 0


"""
    Compute the histogram
    
    x: value used for x axis
    y: value used for y axis
    xlabel: vale use for x label
    ylabel: vale use for y label
    title: title of histogram
    name: name the file into save it
"""


def istogramma(x, y, xlabel, ylabel, title, name):
    SMALL_SIZE = 7
    MEDIUM_SIZE = 10
    BIGGER_SIZE = 12
    mpl.rcParams['toolbar'] = 'None'
    plt.bar(x, y, tick_label=x, width=0.8, color=['green'])

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.xticks(x, rotation='-45', ha='center', size=SMALL_SIZE)
    plt.subplots_adjust(left=0.2)

    #plt.show()
    plt.savefig(name, bbox_inches='tight')
    plt.close()
    return


"""
    Compute the response time
    
    df: Dataframe used for the computation
"""


def response_time(df):
    try:

        t = (int(df['0-15'].sum()) * 15 + int(df['16-30'].sum()) * 30 + int(df['31-45'].sum()) * 45 + int(
            df['46-60'].sum()) * 60 + int(df['61-90'].sum()) * 90 + int(df['91-120'].sum()) * 120 + int(df['120 PLUS'].sum()) * 120) - int(
            df['Avarege Wait Time'].sum())

        passengers = int(df['Total'].sum())

        time = t / passengers

        return time
    except:
        return 0


"""
    Read a file in order to take the header
    
    f: path to the file
"""


def readFile(f):
    # rows = []
    c = []
    with open(f, mode='r') as f:
        reader = csv.reader(f)
        header = next(reader)
        # for row in reader:
        #   rows.append(row)
    for element in header:
        c.append(element)
    return c


def plot(x, y, x_label, y_label, title, name):
    plt.plot(x, y)

    # naming the x axis
    plt.xlabel(x_label)
    # naming the y axis
    plt.ylabel(y_label)

    # giving a title to my graph
    plt.title(title)

    # function to show the plot
    plt.savefig(name, bbox_inches='tight')


def multiplePlot(x, y1, y2, y3, x_label, y_label, title, name, plot1, plot2, plot3):
    plt.plot(x, y1, '--r', label=plot1)
    plt.plot(x, y2, '--b', label=plot2)
    plt.plot(x, y3, '--g', label=plot3)
    plt.legend()

    # naming the x axis
    plt.xlabel(x_label)
    # naming the y axis
    plt.ylabel(y_label)

    # giving a title to my graph
    plt.title(title)

    # function to show the plot
    plt.savefig(name, bbox_inches='tight')
    plt.close()
    return


"""
    Read a file and return the unique value for a specific column
    
    f: path to file
    c: list of columns name
    value: the specific column in which take the values
"""


def readUniqueResult(f, c, value):
    try:
        df = pd.read_csv(f, usecols=c, sep=',')
        return df[value].unique().tolist()
    except:
        return False


if __name__ == "__main__":

    # Analisi del dataset dell'aeroporto di Los Angeles

    ora_array = ["00:00 - 1:00", "1:00 - 2:00", "2:00 - 3:00", "3:00 - 4:00", "4:00 - 5:00", "5:00 - 6:00",
                 "6:00 - 7:00", "7:00 - 8:00", "8:00 - 9:00", "9:00 - 10:00", "10:00 - 11:00", "11:00 - 12:00", "12:00 - 13:00",
                 "13:00 - 14:00", "14:00 - 15:00", "15:00 - 16:00", "16:00 - 17:00", "17:00 - 18:00", "18:00 - 19:00", "19:00 - 20:00",
                 "20:00 - 21:00", "21:00 - 22:00", "22:00 - 23:00", "23:00 - 00:00"]

    column = ["Airport", "Terminal", "Date", "Hour", "Avarege Wait Time", "Max Wait Time", "0-15", "16-30", "31-45", "46-60", "61-90", "91-120",
              "120 PLUS", "Excluded", "Total", "Flights", "Booths"]

    averageWaitTimeNational = []
    averageWaitTimeInternational = []
    totalInternational = []
    totalNational = []
    rspNational = []
    rspInternational = []
    for i in range(len(ora_array)):
        averageWaitTimeNational.append(average(splitDf(readDataset(file, "National", column, "Terminal", ';'), "Hour", ora_array[i]),"Avarege Wait Time"))
        averageWaitTimeInternational.append(average(splitDf(readDataset(file, "International", column, "Terminal", ';'), "Hour", ora_array[i]),"Avarege Wait Time"))
        totalInternational.append(average(splitDf(readDataset(file, "International", column, "Terminal", ';'), "Hour", ora_array[i]),"Total"))
        totalNational.append(average(splitDf(readDataset(file, "National", column, "Terminal", ';'), "Hour", ora_array[i]), "Total"))
        rspNational.append(response_time(splitDf(readDataset(file, "National", column, "Terminal", ';'), "Hour", ora_array[i])))
        rspInternational.append(response_time(splitDf(readDataset(file, "International", column, "Terminal", ';'), "Hour", ora_array[i])))

    istogramma(ora_array, averageWaitTimeNational, 'Hour', 'Avarage Wait Time', 'National flight', "graph/averageWaitTimeNational.png")
    istogramma(ora_array, averageWaitTimeNational, 'Hour', 'Avarage Wait Time', 'International flight', "graph/averageWaitTimeNational.png")
    istogramma(ora_array, totalInternational, 'Hour', 'Number of passengers', 'International flight', "graph/totalInternational.png")
    istogramma(ora_array, totalNational, 'Hour', 'Number of passengers', 'National flight', "graph/totalNational.png")
    istogramma(ora_array, rspNational, 'Hour', 'Response Time', 'National flight', "graph/rspNational.png")
    istogramma(ora_array, rspInternational, 'Hour', 'Response Time', 'International flight', "graph/rspInternational.png")

    # Analisi dei risultati della simulazione

    columnSimulation = readFile(fSimulazione)

    num = readUniqueResult(fSimulazione, columnSimulation, 'off')
    num.sort()
    off = []
    for e in num:
        if e != 3:
            off.append(e)

    num_off_sita = []
    for e in num:
        if e != 1:
            num_off_sita.append(e)

    dfSingleQueue = selectMultipleValue(splitDf(readDataset(fSimulazione, 'single-queue', columnSimulation, 'checkin_type', ','), 'test_type', '(null)'), 'off', off, columnSimulation)
    responseSinqleQueue = dfSingleQueue['mresponse'].tolist()
    waitSingleQueue = dfSingleQueue['mwait'].tolist()
    serviceSingleQueue = dfSingleQueue['mservice'].tolist()

    dfSingleQueueCovidTest = selectMultipleValue(splitDf(readDataset(fSimulazione, 'single-queue', columnSimulation, 'checkin_type', ','), 'test_type', 'single-queue'), 'off', off, columnSimulation)
    responseSinqleQueueCovidTest = dfSingleQueueCovidTest['mresponse'].tolist()
    waitSingleQueueCovidTest = dfSingleQueueCovidTest['mwait'].tolist()
    serviceSingleQueueCovidTest = dfSingleQueueCovidTest['mservice'].tolist()

    dfSingleQueueMultiCovidTest = selectMultipleValue(splitDf(readDataset(fSimulazione, 'single-queue', columnSimulation, 'checkin_type', ','), 'test_type', 'multi-queue'), 'off', off, columnSimulation)
    responseSinqleQueueMultiCovidTest = dfSingleQueueMultiCovidTest['mresponse'].tolist()
    waitSingleQueueMultiCovidTest = dfSingleQueueMultiCovidTest['mwait'].tolist()
    serviceSinqleQueueMultiCovidTest = dfSingleQueueMultiCovidTest['mservice'].tolist()

    dfMultiQueue = selectMultipleValue(splitDf(readDataset(fSimulazione, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type', '(null)'), 'off', off, columnSimulation)
    responseMultiQueue = dfMultiQueue['mresponse'].tolist()
    waitMultiQueue = dfMultiQueue['mwait'].tolist()
    serviceMultiQueue = dfMultiQueue['mservice'].tolist()

    dfMultiQueueCovidTest = selectMultipleValue(splitDf(readDataset(fSimulazione, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type', 'single-queue'), 'off', off, columnSimulation)
    responseMultiQueueCovidTest = dfMultiQueueCovidTest['mresponse'].tolist()
    waitMultiQueueCovidTest = dfMultiQueueCovidTest['mwait'].tolist()
    serviceMultiQueueCovidTest = dfMultiQueueCovidTest['mservice'].tolist()

    dfMultiQueueMultiCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'multi-queue'), 'off', off, columnSimulation)
    responseMultiQueueMultiCovidTest = dfMultiQueueMultiCovidTest['mresponse'].tolist()
    waitMultiQueueMultiCovidTest = dfMultiQueueMultiCovidTest['mwait'].tolist()
    serviceMultiQueueMultiCovidTest = dfMultiQueueMultiCovidTest['mservice'].tolist()

    dfSitaQueue = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'sita-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                '(null)'), 'off', num_off_sita, columnSimulation)
    responseSitaQueue = dfSitaQueue['mresponse'].tolist()
    waitSitaQueue = dfSitaQueue['mwait'].tolist()
    serviceSitaQueue = dfSitaQueue['mservice'].tolist()

    dfSitaQueueCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'sita-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num_off_sita, columnSimulation)
    responseSitaQueueCovidTest = dfSitaQueueCovidTest['mresponse'].tolist()
    waitSitaQueueCovidTest = dfSitaQueueCovidTest['mwait'].tolist()
    serviceSitaQueueCovidTest = dfSitaQueueCovidTest['mservice'].tolist()

    dfSitaQueueMultiCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'sita-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'multi-queue'), 'off', num_off_sita, columnSimulation)
    responseSitaQueueMultiCovidTest = dfSitaQueueMultiCovidTest['mresponse'].tolist()
    waitSitaQueueMultiCovidTest = dfSitaQueueMultiCovidTest['mwait'].tolist()
    serviceSitaQueueMultiCovidTest = dfSitaQueueMultiCovidTest['mservice'].tolist()

    multiplePlot(off, responseSinqleQueue, waitSingleQueue, serviceSingleQueue, 'Number of servers', 'Time', 'Single queue', 'graph/singlequeue.png', 'Response Time', 'Wait Time', 'Service Time')
    multiplePlot(off, responseSinqleQueueCovidTest, waitSingleQueueCovidTest, serviceSingleQueueCovidTest, 'Number of servers', 'Time', 'Single queue with single queue covid test', 'graph/singleQueueCovidTest.png', 'Response Time', 'Wait Time', 'Service Time')
    multiplePlot(off, responseSinqleQueueMultiCovidTest, waitSingleQueueMultiCovidTest, serviceSinqleQueueMultiCovidTest, 'Number of servers', 'Time', 'Single queue with multi queue covid test', 'graph/singleQueueMultiCovidTest.png', 'Response Time', 'Wait Time', 'Service Time')

    multiplePlot(off, responseMultiQueue, waitMultiQueue, serviceMultiQueue, 'Number of servers', 'Time', 'Multi queue', 'graph/multiqueue.png', 'Response Time', 'Wait Time', 'Service Time')
    multiplePlot(off, responseMultiQueueCovidTest, waitMultiQueueCovidTest, serviceMultiQueueCovidTest, 'Number of servers', 'Time', 'Multi queue with single queue covid test', 'graph/multiQueueSingleCovidTest.png', 'Response Time', 'Wait Time', 'Service Time')
    multiplePlot(off, responseMultiQueueMultiCovidTest, waitMultiQueueMultiCovidTest, serviceMultiQueueMultiCovidTest, 'Number of servers', 'Time', 'Multi queue with multi queue covid test', 'graph/multiQueueMultiCovidTest.png', 'Response Time', 'Wait Time', 'Service Time')

    multiplePlot(num_off_sita, responseSitaQueue, waitSitaQueue, serviceSitaQueue, 'Number of servers', 'Time', 'Sita queue', 'graph/sitaqueue.png', 'Response Time', 'Wait Time', 'Service Time')
    multiplePlot(num_off_sita, responseSitaQueueCovidTest, waitSitaQueueCovidTest, serviceSitaQueueCovidTest, 'Number of servers', 'Time', 'Sita queue with single queue covid test', 'graph/sitaQueueSingleCovidTest.png', 'Response Time', 'Wait Time', 'Service Time')
    multiplePlot(num_off_sita, responseSitaQueueMultiCovidTest, waitSitaQueueMultiCovidTest, serviceSitaQueueMultiCovidTest, 'Number of servers', 'Time', 'sita queue with multi queue covid test', 'graph/sitaQueueMultiCovidTest.png', 'Response Time', 'Wait Time', 'Service Time')


