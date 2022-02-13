import csv
import os

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import scipy.stats as st

import drawICWait
import drawIcResponse
import formatter
import resultFile

file = 'dataset/datasetLA.csv'

fSimulazione = 'dataset/resultSimulation.csv'
fSimulazioneSita = 'dataset/resultSimulationSita.csv'
fSimulazioneNoCovidTest = 'dataset/resultNoCovidTest.csv'
fRepliche = 'dataset/repliche.csv'
fBatch = 'dataset/batch.csv'

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
        dataframe = pd.read_csv(f, usecols=c_name, sep=d)
        df1 = dataframe[dataframe[read_value] == search_value]
        return df1
    except:
        return False


"""
    Split the dataframe
    
    df: dataframe to split
    column_value: column's name into look 
    value: value to check for the split
"""


def splitDf(dataframe, column_value, value):
    try:
        df1 = dataframe[dataframe[column_value] == value]
        return df1
    except:
        return False


def selectMultipleValue(dataframe, column_value, offset, column_name):
    try:
        df1 = pd.DataFrame(columns=column_name)
        for index in range(len(offset)):
            df1 = df1.append(dataframe[dataframe[column_value] == offset[index]], ignore_index=True)
        return df1
    except:
        return False


"""
    Compute the avarage 
    
    df: dataframe used
    value: column used for compute the average
"""


def average(dataframe, value):
    try:
        media = int(dataframe[value].sum()) / len(dataframe)
        return media
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


def draw_istogramma(x, y, xlabel, ylabel, title, name):
    SMALL_SIZE = 7
    mpl.rcParams['toolbar'] = 'None'
    plt.bar(x, y, tick_label=x, width=0.8, color=['green'])

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.xticks(x, rotation='90', ha='center', size=SMALL_SIZE)
    plt.subplots_adjust(left=0.2)

    # plt.show()
    plt.savefig(name, bbox_inches='tight')
    plt.close()
    return


"""
    Compute the response time
    
    df: Dataframe used for the computation
"""


def response_time(dataframe):
    try:

        t = (int(dataframe['0-15'].sum()) * 15 + int(dataframe['16-30'].sum()) * 30 + int(
            dataframe['31-45'].sum()) * 45 + int(
            dataframe['46-60'].sum()) * 60 + int(dataframe['61-90'].sum()) * 90 + int(
            dataframe['91-120'].sum()) * 120 + int(
            dataframe['120 PLUS'].sum()) * 120)

        passengers = int(dataframe['Total'].sum())

        time = t / passengers

        return time
    except:
        return 0


"""
    Compute the service time

    df: Dataframe used for the computation
"""


def service_time(dataframe):
    try:

        t = (int(dataframe['0-15'].sum()) * 15 + int(dataframe['16-30'].sum()) * 30 + int(
            dataframe['31-45'].sum()) * 45 + int(
            dataframe['46-60'].sum()) * 60 + int(dataframe['61-90'].sum()) * 90 + int(
            dataframe['91-120'].sum()) * 120 + int(
            dataframe['120 PLUS'].sum()) * 120) - int(dataframe['Avarege Wait Time'].sum())

        passengers = int(dataframe['Total'].sum())

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


def multiplePlot(x, y1, y2, x_label, y_label, title, name, plot1, plot2):
    plt.plot(x, y1, '--r', label=plot1)
    plt.plot(x, y2, '--b', label=plot2)
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


def multiplePlot2(x, y1, y2, y3, x_label, y_label, title, name, plot1, plot2, plot3):
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
        dataframe = pd.read_csv(f, usecols=c, sep=',')
        return dataframe[value].unique().tolist()
    except:
        return False


def readValue(f, c, value):
    try:
        dataframe = pd.read_csv(f, usecols=c, sep=',')
        return dataframe[value].tolist()
    except:
        return False


def analisiDataset():
    # Analisi del dataset dell'aeroporto di Los Angeles

    ora_array = ["00:00 - 1:00", "1:00 - 2:00", "2:00 - 3:00", "3:00 - 4:00", "4:00 - 5:00", "5:00 - 6:00",
                 "6:00 - 7:00", "7:00 - 8:00", "8:00 - 9:00", "9:00 - 10:00", "10:00 - 11:00", "11:00 - 12:00",
                 "12:00 - 13:00", "13:00 - 14:00", "14:00 - 15:00", "15:00 - 16:00", "16:00 - 17:00", "17:00 - 18:00",
                 "18:00 - 19:00", "19:00 - 20:00", "20:00 - 21:00", "21:00 - 22:00", "22:00 - 23:00", "23:00 - 00:00"]

    column = ["Airport", "Terminal", "Date", "Hour", "Avarege Wait Time", "Max Wait Time", "0-15", "16-30", "31-45",
              "46-60", "61-90", "91-120",
              "120 PLUS", "Excluded", "Total", "Flights", "Booths"]

    averageWaitTimeNational = []
    averageWaitTimeInternational = []
    totalInternational = []
    totalNational = []
    rspNational = []
    rspInternational = []
    serviceNational = []
    serviceInternational = []

    for i in range(len(ora_array)):
        averageWaitTimeNational.append(
            average(splitDf(readDataset(file, "National", column, "Terminal", ';'), "Hour", ora_array[i]),
                    "Avarege Wait Time"))
        averageWaitTimeInternational.append(
            average(splitDf(readDataset(file, "International", column, "Terminal", ';'), "Hour", ora_array[i]),
                    "Avarege Wait Time"))
        totalInternational.append(
            average(splitDf(readDataset(file, "International", column, "Terminal", ';'), "Hour", ora_array[i]),
                    "Total"))
        totalNational.append(
            average(splitDf(readDataset(file, "National", column, "Terminal", ';'), "Hour", ora_array[i]), "Total"))
        rspNational.append(
            response_time(splitDf(readDataset(file, "National", column, "Terminal", ';'), "Hour", ora_array[i])))
        rspInternational.append(
            response_time(splitDf(readDataset(file, "International", column, "Terminal", ';'), "Hour", ora_array[i])))
        serviceNational.append(
            service_time(splitDf(readDataset(file, "National", column, "Terminal", ';'), "Hour", ora_array[i])))
        serviceInternational.append(
            service_time(splitDf(readDataset(file, "International", column, "Terminal", ';'), "Hour", ora_array[i])))

    # Grafici valori dataset

    draw_istogramma(ora_array, averageWaitTimeNational, 'Hour', 'Avarage Wait Time', 'National flight',
                    "graph/averageWaitTimeNational.png")
    draw_istogramma(ora_array, averageWaitTimeNational, 'Hour', 'Avarage Wait Time', 'International flight',
                    "graph/averageWaitTimeNational.png")
    draw_istogramma(ora_array, totalInternational, 'Hour', 'Number of passengers', 'International flight',
                    "graph/totalInternational.png")
    draw_istogramma(ora_array, totalNational, 'Hour', 'Number of passengers', 'National flight',
                    "graph/totalNational.png")
    draw_istogramma(ora_array, rspNational, 'Hour', 'Response Time', 'National flight', "graph/rspNational.png")
    draw_istogramma(ora_array, rspInternational, 'Hour', 'Response Time', 'International flight',
                    "graph/rspInternational.png")

    total = []

    for i in range(len(totalNational)):
        total.append(totalNational[i] + totalInternational[i])

    national = []

    for i in range(len(totalNational)):
        if total[i] > 0:
            national.append(totalNational[i] / total[i])
        else:
            national.append(0)

    international = []

    for i in range(len(totalNational)):
        if total[i] > 0:
            international.append(totalInternational[i] / total[i])
        else:
            international.append(0)

    data = {'ora': ora_array, 'national': national, 'international': international}
    df = pd.DataFrame(data=data)

    df.to_excel("output.xlsx", sheet_name='Sheet_name_1')

    draw_istogramma(ora_array, total, 'Hour', 'Number of passengers', 'Flight', "graph/totalPassengers.png")

    data2 = {'ora': ora_array, 'national': national, 'international': international, 'mediaNational': rspNational,
             'mInternational': rspInternational}
    df2 = pd.DataFrame(data=data2)


def batch(f, checkin_type, test_type, header, column_value):
    dataframe = splitDf(readDataset(f, checkin_type, header, 'checkin_type', ','), 'test_type', test_type)

    value = dataframe[column_value].sum()

    return value / len(dataframe)


def intervalliConfidenzaMedia(f, checkin_type, test_type, header, column_value):
    dataframe = splitDf(readDataset(f, checkin_type, header, 'checkin_type', ','), 'test_type', test_type)

    value = dataframe[column_value].tolist()

    return st.t.interval(0.95, len(value) - 1, loc=np.mean(value), scale=st.sem(value))


def intervalliConfidenzaVarianza(f, checkin_type, test_type, header, column_value):
    dataframe = splitDf(readDataset(f, checkin_type, header, 'checkin_type', ','), 'test_type', test_type)

    value = dataframe[column_value].tolist()

    mean, var, std = st.bayes_mvs(value)

    return var[1]


def Varianza(f, checkin_type, test_type, header, column_value):
    dataframe = splitDf(readDataset(f, checkin_type, header, 'checkin_type', ','), 'test_type', test_type)

    value = dataframe[column_value].tolist()

    mean, var, std = st.bayes_mvs(value)

    v = var[0]

    return v


if __name__ == "__main__":

    if os.path.exists(fSimulazione):
        os.remove(fSimulazione)

    if os.path.exists(fSimulazioneSita):
        os.remove(fSimulazioneSita)

    if os.path.exists(fSimulazioneNoCovidTest):
        os.remove(fSimulazioneNoCovidTest)

    formatter.formatter()

    analisiDataset()

    # Analisi dei risultati della simulazione

    # Analisi single queue e multi queue

    resultFile.creaFileFinale("dataset/example.csv", "dataset/example1.csv", "dataset/example2.csv",
                              "dataset/example3.csv", "dataset/example4.csv", fSimulazione)

    columnSimulation = readFile(fSimulazione)

    num_off = readUniqueResult(fSimulazione, columnSimulation, 'off')
    num_off.sort()

    # Single queue with single covid test

    dfSingleQueueCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'single-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num_off, columnSimulation)
    responseSinqleQueueCovidTest = dfSingleQueueCovidTest['mresponse'].tolist()
    waitSingleQueueCovidTest = dfSingleQueueCovidTest['mwait'].tolist()

    # Single queue with multi covid test

    dfSingleQueueMultiCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'single-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'multi-queue'), 'off', num_off, columnSimulation)
    responseSinqleQueueMultiCovidTest = dfSingleQueueMultiCovidTest['mresponse'].tolist()
    waitSingleQueueMultiCovidTest = dfSingleQueueMultiCovidTest['mwait'].tolist()

    # Multi queue with single covid test

    dfMultiQueueCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num_off, columnSimulation)
    responseMultiQueueCovidTest = dfMultiQueueCovidTest['mresponse'].tolist()
    waitMultiQueueCovidTest = dfMultiQueueCovidTest['mwait'].tolist()

    # Multi queue with multi covid test

    dfMultiQueueMultiCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'multi-queue'), 'off', num_off, columnSimulation)
    responseMultiQueueMultiCovidTest = dfMultiQueueMultiCovidTest['mresponse'].tolist()
    waitMultiQueueMultiCovidTest = dfMultiQueueMultiCovidTest['mwait'].tolist()

    # Analisi sita queue

    resultFile.creaFileFinale("dataset/sita.csv", "dataset/sita1.csv", "dataset/sita2.csv", "dataset/sita3.csv",
                              'dataset/sita4.csv', fSimulazioneSita)
    num = readUniqueResult(fSimulazioneSita, columnSimulation, 'off')
    num.sort()

    # Sita queue with single covid test

    dfSitaQueueCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'sita-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num_off, columnSimulation)
    responseSitaQueueCovidTest = dfSitaQueueCovidTest['mresponse'].tolist()
    waitSitaQueueCovidTest = dfSitaQueueCovidTest['mwait'].tolist()
    withdrawalSitaQueueCovidTest = dfSitaQueueCovidTest['withdrawal'].tolist()
    sitaTestwithdrawal = dfSitaQueueCovidTest['test_withdrawal'].tolist()

    # Sita queue with multi covid test

    dfSitaQueueMultiCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'sita-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'multi-queue'), 'off', num_off, columnSimulation)
    responseSitaQueueMultiCovidTest = dfSitaQueueMultiCovidTest['mresponse'].tolist()
    waitSitaQueueMultiCovidTest = dfSitaQueueMultiCovidTest['mwait'].tolist()
    withdrawalSitaQueueMultiCovidTest = dfSitaQueueMultiCovidTest['withdrawal'].tolist()
    sitaMultiTestwithdrawal = dfSitaQueueMultiCovidTest['test_withdrawal'].tolist()

    # Analisi

    dfSQCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'single-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num_off, columnSimulation)
    withdrawalSingleQueueCovidTest = dfSQCovidTest['withdrawal'].tolist()
    sqTestwithdrawal = dfSQCovidTest['test_withdrawal'].tolist()

    dfSQMultiCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'single-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'multi-queue'), 'off', num_off, columnSimulation)
    withdrawalSingleQueueMultiCovidTest = dfSQMultiCovidTest['withdrawal'].tolist()
    sqMultiTestwithdrawal = dfSQMultiCovidTest['test_withdrawal'].tolist()

    dfMQCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num_off, columnSimulation)
    withdrawalMultiQueueCovidTest = dfMQCovidTest['withdrawal'].tolist()
    mqTestwithdrawal = dfMQCovidTest['test_withdrawal'].tolist()

    dfMQMultiCovidTest = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'multi-queue'), 'off', num_off, columnSimulation)
    withdrawalMultiQueueMultiCovidTest = dfMQMultiCovidTest['withdrawal'].tolist()
    mqMultiTestwithdrawal = dfMQMultiCovidTest['test_withdrawal'].tolist()

    # Grafici delle perdite

    multiplePlot2(num_off, withdrawalSingleQueueCovidTest, withdrawalMultiQueueCovidTest, withdrawalSitaQueueCovidTest,
                  'Number of servers',
                  'Withdrawal', 'Withdrawal with single queue covid test', 'graph/withdrawalqueueCovidTest.png',
                  'withdrawal Single queue',
                  'withdrawal Multi queue', 'withdrawal Sita queue')

    multiplePlot2(num_off, withdrawalSingleQueueMultiCovidTest, withdrawalMultiQueueMultiCovidTest,
                  withdrawalSitaQueueMultiCovidTest,
                  'Number of servers',
                  'Withdrawal', 'Withdrawal with multi queue covid test', 'graph/withdrawalqueueMultiCovidTest.png',
                  'withdrawal Single queue',
                  'withdrawal Multi queue', 'withdrawal Sita queue')

    multiplePlot2(num_off, sqTestwithdrawal, mqTestwithdrawal,
                  sitaTestwithdrawal,
                  'Number of servers',
                  'Withdrawal', 'Covid test withdrawal with single queue', 'graph/testcovid.png',
                  'withdrawal Single queue',
                  'withdrawal Multi queue', 'withdrawal Sita queue')

    multiplePlot2(num_off, sqMultiTestwithdrawal, mqMultiTestwithdrawal,
                  sitaMultiTestwithdrawal,
                  'Number of servers',
                  'Withdrawal', 'Covid test withdrawal with multi queue', 'graph/testcovidMulti.png',
                  'withdrawal Single queue',
                  'withdrawal Multi queue', 'withdrawal Sita queue')

    # Confronto Sita Queue

    dfSita1 = selectMultipleValue(
        splitDf(readDataset(fSimulazione, 'sita-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num_off, columnSimulation)
    responseSita1 = dfSita1['mresponse'].tolist()
    waitSita1 = dfSita1['mwait'].tolist()

    dfSita2 = selectMultipleValue(
        splitDf(readDataset(fSimulazioneSita, 'sita-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num_off, columnSimulation)
    responseSita2 = dfSita2['mresponse'].tolist()
    waitSita2 = dfSita2['mwait'].tolist()

    multiplePlot(num_off, responseSita1, responseSita2, 'Number of servers', 'Time',
                 'Response time sita queue', 'graph/sitaQueueConfrontoResponse.png', 'sita1',
                 'sita2')

    multiplePlot(num_off, waitSita1, waitSita2, 'Number of servers', 'Time',
                 'Wait time sita queue', 'graph/sitaQueueConfrontoWait.png', 'sita1',
                 'sita2')

    # Confronto tempi di attesa delle tre configurazioni

    waitSQCovidTest = dfSQCovidTest['mwait'].tolist()
    waitMQCovidTest = dfMQCovidTest['mwait'].tolist()

    multiplePlot2(num_off, waitSQCovidTest, waitMQCovidTest, waitSitaQueueCovidTest, 'Number of servers',
                  'Average Wait time', 'Average Wait time with single covid test queue',
                  'graph/confrontoAverageWaitTimeCovidTest.png',
                  'Average Single queue',
                  'Average Multi queue', 'Average Sita queue')

    waitSQMultiCovidTest = dfSQMultiCovidTest['mwait'].tolist()
    waitMQMultiCovidTest = dfMQMultiCovidTest['mwait'].tolist()

    multiplePlot2(num_off, waitSQMultiCovidTest, waitMQMultiCovidTest, waitSitaQueueMultiCovidTest, 'Number of servers',
                  'Average Wait time', 'Average Wait time with multi covid test queue',
                  'graph/confrontoAverageWaitTimeMultiCovidTest.png',
                  'Average Single queue',
                  'Average Multi queue', 'Average Sita queue')

    # Confronto tempi di risposta delle tre configurazioni

    rspSQCovidTest = dfSQCovidTest['mresponse'].tolist()
    rspMQCovidTest = dfMQCovidTest['mresponse'].tolist()

    multiplePlot2(num_off, rspSQCovidTest, rspMQCovidTest, responseSitaQueueCovidTest, 'Number of servers',
                  'Average Response time', 'Average Response time with single covid test queue',
                  'graph/confrontoAverageResponseTimeCovidTest.png',
                  'Average Single queue',
                  'Average Multi queue', 'Average Sita queue')

    rspSQMultiCovidTest = dfSQMultiCovidTest['mresponse'].tolist()
    rspMQMultiCovidTest = dfMQMultiCovidTest['mresponse'].tolist()

    multiplePlot2(num_off, rspSQMultiCovidTest, rspMQMultiCovidTest, responseSitaQueueMultiCovidTest,
                  'Number of servers',
                  'Average Response time', 'Average Response time with multi covid test queue',
                  'graph/confrontoAverageResponseTimeMultiCovidTest.png',
                  'Average Single queue',
                  'Average Multi queue', 'Average Sita queue')

    resultFile.creaFileFinale("dataset/noCovidTest.csv", "dataset/noCovidTest1.csv", "dataset/noCovidTest2.csv",
                              "dataset/noCovidTest3.csv", "dataset/noCovidTest4.csv", fSimulazioneNoCovidTest)

    # Single queue without covid test

    dfSingleQueue = selectMultipleValue(
        splitDf(readDataset(fSimulazioneNoCovidTest, 'single-queue', columnSimulation, 'checkin_type', ','),
                'test_type',
                'single-queue'), 'off', num_off, columnSimulation)

    responseSinqleQueue = dfSingleQueue['mresponse'].tolist()
    waitSingleQueue = dfSingleQueue['mwait'].tolist()

    # Multi queue without covid test

    dfMultiQueue = selectMultipleValue(
        splitDf(readDataset(fSimulazioneNoCovidTest, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'),
        'off', num_off, columnSimulation)
    responseMultiQueue = dfMultiQueue['mresponse'].tolist()
    waitMultiQueue = dfMultiQueue['mwait'].tolist()

    # Sita queue without covid test

    dfSitaQueue = selectMultipleValue(
        splitDf(readDataset(fSimulazioneNoCovidTest, 'sita-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'), 'off', num, columnSimulation)
    responseSitaQueue = dfSitaQueue['mresponse'].tolist()
    waitSitaQueue = dfSitaQueue['mwait'].tolist()
    withdrawalSitaQueue = dfSitaQueue['withdrawal'].tolist()

    dfSQ = selectMultipleValue(
        splitDf(readDataset(fSimulazioneNoCovidTest, 'single-queue', columnSimulation, 'checkin_type', ','),
                'test_type',
                'single-queue'), 'off', num_off, columnSimulation)
    withdrawalSingleQueue = dfSQ['withdrawal'].tolist()

    dfMQ = selectMultipleValue(
        splitDf(readDataset(fSimulazioneNoCovidTest, 'multi-queue', columnSimulation, 'checkin_type', ','), 'test_type',
                'single-queue'),
        'off', num, columnSimulation)
    withdrawalMultiQueue = dfMQ['withdrawal'].tolist()

    waitSQ = dfSQ['mwait'].tolist()
    waitMQ = dfMQ['mwait'].tolist()

    rspSQ = dfSQ['mresponse'].tolist()
    rspMQ = dfMQ['mresponse'].tolist()

    multiplePlot2(num, waitSQ, waitMQ, waitSita2, 'Number of servers',
                  'Average Wait time', 'Average Wait time without covid test',
                  'graph/confrontoAverageWaitTime.png',
                  'Average Single queue',
                  'Average Multi queue', 'Average Sita queue')

    multiplePlot2(num_off, withdrawalSingleQueue, withdrawalMultiQueue, withdrawalSitaQueue, 'Number of servers',
                  'Withdrawal', 'Withdrawal', 'graph/withdrawalqueue.png', 'withdrawal Single queue',
                  'withdrawal Multi queue', 'withdrawal Sita queue')

    multiplePlot2(num, rspSQ, rspMQ, responseSitaQueue, 'Number of servers',
                  'Average Response time', 'Average Response time without covid test',
                  'graph/confrontoAverageResponseTime.png',
                  'Average Single queue',
                  'Average Multi queue', 'Average Sita queue')

    # Grafici per la single queue

    multiplePlot(num_off, responseSinqleQueueCovidTest, responseSinqleQueueMultiCovidTest, 'Number of servers', 'Time',
                 'Single queue with single covid test vs single queue with multi covid test',
                 'graph/ConfrontoResponseSingle.png', 'Single queue with single covid test',
                 'single queue with multi covid test')

    multiplePlot(num_off, waitSingleQueueCovidTest, waitSingleQueueMultiCovidTest, 'Number of servers', 'Time',
                 'Single queue with single covid test vs single queue with multi covid test',
                 'graph/ConfrontoWaitSingle.png', 'Single queue with single covid test',
                 'single queue with multi covid test')

    # Metodo delle repliche

    resultFile.creaFileFinaleRepliche("dataset/SingleSingle.csv", "dataset/SingleMulti.csv", "dataset/MultiSingle.csv",
                                      "dataset/MultiMulti.csv", "dataset/SitaSingle.csv", "dataset/SitaMulti.csv",
                                      fRepliche)

    c = readFile(fRepliche)

    dfSingleSingle = pd.read_csv('dataset/SingleSingle.csv', usecols=c, sep=',')
    waitSingleSingle = dfSingleSingle['mwait'].tolist()
    numSingleSingle = dfSingleSingle['num_pass'].tolist()

    dfSingleMulti = pd.read_csv('dataset/SingleSingle.csv', usecols=c, sep=',')
    waitSingleMulti = dfSingleMulti['mwait'].tolist()
    numSingleMulti = dfSingleMulti['num_pass'].tolist()

    dfMultiSingle = pd.read_csv('dataset/MultiSingle.csv', usecols=c, sep=',')
    waitMultiSingle = dfMultiSingle['mwait'].tolist()
    numMultiSingle = dfMultiSingle['num_pass'].tolist()

    dfMultiMulti = pd.read_csv('dataset/MultiMulti.csv', usecols=c, sep=',')
    waitMultiMulti = dfMultiMulti['mwait'].tolist()
    numMultiMulti = dfMultiMulti['num_pass'].tolist()

    dfSitaSingle = pd.read_csv('dataset/SitaSingle.csv', usecols=c, sep=',')
    waitSitaSingle = dfSitaSingle['mwait'].tolist()
    numSitaSingle = dfSitaSingle['num_pass'].tolist()

    dfSitaMulti = pd.read_csv('dataset/SitaMulti.csv', usecols=c, sep=',')
    waitSitaMulti = dfSitaMulti['mwait'].tolist()
    numSitaMulti = dfSitaMulti['num_pass'].tolist()

    column = readFile(fBatch)

    attesa = [batch(fBatch, 'single-queue', 'single-queue', column, 'mwait'),
              batch(fBatch, 'single-queue', 'multi-queue', column, 'mwait'),
              batch(fBatch, 'multi-queue', 'single-queue', column, 'mwait'),
              batch(fBatch, 'multi-queue', 'multi-queue', column, 'mwait'),
              batch(fBatch, 'sita-queue', 'single-queue', column, 'mwait'),
              batch(fBatch, 'sita-queue', 'multi-queue', column, 'mwait')]

    ic = [intervalliConfidenzaMedia(fBatch, 'single-queue', 'single-queue', column, 'mwait'),
          intervalliConfidenzaMedia(fBatch, 'single-queue', 'multi-queue', column, 'mwait'),
          intervalliConfidenzaMedia(fBatch, 'multi-queue', 'single-queue', column, 'mwait'),
          intervalliConfidenzaMedia(fBatch, 'single-queue', 'multi-queue', column, 'mwait'),
          intervalliConfidenzaMedia(fBatch, 'sita-queue', 'single-queue', column, 'mwait'),
          intervalliConfidenzaMedia(fBatch, 'sita-queue', 'multi-queue', column, 'mwait')]

    checkin = ['single-queue', 'single-queue', 'multi-queue', 'multi-queue', 'sita-queue', 'sita-queue']

    test = ['single-queue', 'multi-queue', 'single-queue', 'multi-queue', 'single-queue', 'multi-queue']

    risposta = [batch(fBatch, 'single-queue', 'single-queue', column, 'mresponse'),
                batch(fBatch, 'single-queue', 'multi-queue', column, 'mresponse'),
                batch(fBatch, 'multi-queue', 'single-queue', column, 'mresponse'),
                batch(fBatch, 'multi-queue', 'multi-queue', column, 'mresponse'),
                batch(fBatch, 'sita-queue', 'single-queue', column, 'mresponse'),
                batch(fBatch, 'sita-queue', 'multi-queue', column, 'mresponse')]

    icRisposta = [intervalliConfidenzaMedia(fBatch, 'single-queue', 'single-queue', column, 'mresponse'),
                  intervalliConfidenzaMedia(fBatch, 'single-queue', 'multi-queue', column, 'mresponse'),
                  intervalliConfidenzaMedia(fBatch, 'multi-queue', 'single-queue', column, 'mresponse'),
                  intervalliConfidenzaMedia(fBatch, 'single-queue', 'multi-queue', column, 'mresponse'),
                  intervalliConfidenzaMedia(fBatch, 'sita-queue', 'single-queue', column, 'mresponse'),
                  intervalliConfidenzaMedia(fBatch, 'sita-queue', 'multi-queue', column, 'mresponse')]

    varianzaRisposta = [Varianza(fBatch, 'single-queue', 'single-queue', column, 'mresponse'),
                        Varianza(fBatch, 'single-queue', 'multi-queue', column, 'mresponse'),
                        Varianza(fBatch, 'multi-queue', 'single-queue', column, 'mresponse'),
                        Varianza(fBatch, 'multi-queue', 'multi-queue', column, 'mresponse'),
                        Varianza(fBatch, 'sita-queue', 'single-queue', column, 'mresponse'),
                        Varianza(fBatch, 'sita-queue', 'multi-queue', column, 'mresponse')]

    var_ic_risp = [intervalliConfidenzaVarianza(fBatch, 'single-queue', 'single-queue', column, 'mresponse'),
                   intervalliConfidenzaVarianza(fBatch, 'single-queue', 'multi-queue', column, 'mresponse'),
                   intervalliConfidenzaVarianza(fBatch, 'multi-queue', 'single-queue', column, 'mresponse'),
                   intervalliConfidenzaVarianza(fBatch, 'multi-queue', 'multi-queue', column, 'mresponse'),
                   intervalliConfidenzaVarianza(fBatch, 'sita-queue', 'single-queue', column, 'mresponse'),
                   intervalliConfidenzaVarianza(fBatch, 'sita-queue', 'multi-queue', column, 'mresponse')]

    dataRisposta = {'checkin_type': checkin, 'test_type': test, 'response': risposta, 'confidence interval for mean': icRisposta,
                    'variance': varianzaRisposta,
                    'confidence interval for variance': var_ic_risp}

    dfRisposta = pd.DataFrame(data=dataRisposta)

    dfRisposta.to_excel("confidenzaRisposta.xlsx", sheet_name='Sheet_name_1')

    varianza = [Varianza(fBatch, 'single-queue', 'single-queue', column, 'mwait'),
                Varianza(fBatch, 'single-queue', 'multi-queue', column, 'mwait'),
                Varianza(fBatch, 'multi-queue', 'single-queue', column, 'mwait'),
                Varianza(fBatch, 'multi-queue', 'multi-queue', column, 'mwait'),
                Varianza(fBatch, 'sita-queue', 'single-queue', column, 'mwait'),
                Varianza(fBatch, 'sita-queue', 'multi-queue', column, 'mwait')]

    var_ic = [intervalliConfidenzaVarianza(fBatch, 'single-queue', 'single-queue', column, 'mwait'),
              intervalliConfidenzaVarianza(fBatch, 'single-queue', 'multi-queue', column, 'mwait'),
              intervalliConfidenzaVarianza(fBatch, 'multi-queue', 'single-queue', column, 'mwait'),
              intervalliConfidenzaVarianza(fBatch, 'multi-queue', 'multi-queue', column, 'mwait'),
              intervalliConfidenzaVarianza(fBatch, 'sita-queue', 'single-queue', column, 'mwait'),
              intervalliConfidenzaVarianza(fBatch, 'sita-queue', 'multi-queue', column, 'mwait')]

    data = {'checkin_type': checkin, 'test_type': test, 'mean wait': attesa, 'confidence interval for mean': ic,
            'variance': varianza,
            'confidence interval for variance': var_ic}
    df = pd.DataFrame(data=data)

    df.to_excel("confidenzaAttesa.xlsx", sheet_name='Sheet_name_1')

    drawICWait.drawIcWait()
    drawICWait.drawIcWaitVariance()
    drawIcResponse.drawResponse()
    drawIcResponse.drawIcResponseVariance()
