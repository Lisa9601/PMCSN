from matplotlib import pyplot as plt


SMALL_SIZE = 7


def drawIcWait():
    x1 = ['single-queue, single-queue', 'single-queue, single-queue']
    x2 = ['single-queue, multi-queue', 'single-queue, multi-queue']
    x3 = ['multi-queue, single-queue', 'multi-queue, single-queue']
    x4 = ['multi-queue, multi-queue', 'multi-queue, multi-queue']
    x5 = ['sita-queue, single-queue', 'sita-queue, single-queue']
    x6 = ['sita-queue, multi-queue', 'sita-queue, multi-queue']

    # intervalli di confidenza per la media
    y1 = [1.2124070017621988, 1.4677961857378012]
    y2 = [1.2304237789239156, 1.4472591273260846]
    y3 = [1.2124070017621988, 1.4677961857378012]
    y4 = [1.2304237789239156, 1.4472591273260846]
    y5 = [8.158555739983079, 8.947001135016919]
    y6 = [8.228058734993438, 9.094297390006561]

    plt.xticks(rotation=45, ha='center', size=SMALL_SIZE)

    plt.plot(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6)

    # function to show the plot
    plt.savefig("graph/drawIcWait.png", bbox_inches='tight')
    plt.close()


def drawIcWaitVariance():
    x1 = ['single-queue, single-queue', 'single-queue, single-queue']
    x2 = ['single-queue, multi-queue', 'single-queue, multi-queue']
    x3 = ['multi-queue, single-queue', 'multi-queue, single-queue']
    x4 = ['multi-queue, multi-queue', 'multi-queue, multi-queue']
    x5 = ['sita-queue, single-queue', 'sita-queue, single-queue']
    x6 = ['sita-queue, multi-queue', 'sita-queue, multi-queue']

    # intervalli di confidenza per la media
    y1 = [0.19949011955295612, 0.35992938256550805]
    y2 = [0.1438058501743144, 0.2594612253405808]
    y3 = [0.19949011955295612, 0.35992938256550805]
    y4 = [0.1438058501743144, 0.2594612253405808]
    y5 = [1.9013393292645544, 3.4304851406339605]
    y6 = [2.295046605871568, 4.140830180770613]

    plt.xticks(rotation=45, ha='center', size=SMALL_SIZE)

    plt.plot(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6)

    # function to show the plot
    plt.savefig("graph/drawIcWaitVariance.png", bbox_inches='tight')
    plt.close()
