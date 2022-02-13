from matplotlib import pyplot as plt


SMALL_SIZE = 7


def drawResponse():
    x1 = ['single-queue, single-queue', 'single-queue, single-queue']
    x2 = ['single-queue, multi-queue', 'single-queue, multi-queue']
    x3 = ['multi-queue, single-queue', 'multi-queue, single-queue']
    x4 = ['multi-queue, multi-queue', 'multi-queue, multi-queue']
    x5 = ['sita-queue, single-queue', 'sita-queue, single-queue']
    x6 = ['sita-queue, multi-queue', 'sita-queue, multi-queue']

    # intervalli di confidenza per la media
    y1 = [6.791722170520226, 7.064354579479773]
    y2 = [6.831242101180908, 7.04896233631909]
    y3 = [6.791722170520226, 7.064354579479773]
    y4 = [6.831242101180908, 7.04896233631909]
    y5 = [13.761417182304541, 14.54496531769546]
    y6 = [13.81547553163143, 14.687244155868568]

    plt.xticks(rotation=45, ha='center', size=SMALL_SIZE)

    plt.plot(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6)

    # function to show the plot
    plt.savefig("graph/drawIcResponse.png", bbox_inches='tight')
    plt.close()


def drawIcResponseVariance():

    x1 = ['single-queue, single-queue', 'single-queue, single-queue']
    x2 = ['single-queue, multi-queue', 'single-queue, multi-queue']
    x3 = ['multi-queue, single-queue', 'multi-queue, single-queue']
    x4 = ['multi-queue, multi-queue', 'multi-queue, multi-queue']
    x5 = ['sita-queue, single-queue', 'sita-queue, single-queue']
    x6 = ['sita-queue, multi-queue', 'sita-queue, multi-queue']

    # intervalli di confidenza per la media
    y1 = [0.22733764230357895, 0.41017318256959745]
    y2 = [0.14498196414672984, 0.26158322505097636]
    y3 = [0.22733764230357895, 0.41017318256959745]
    y4 = [0.14498196414672984, 0.26158322505097636]
    y5 = [1.8777931593254191, 3.3880020420876904]
    y6 = [2.324442772136847, 4.193868115668667]

    plt.xticks(rotation=45, ha='center', size=SMALL_SIZE)

    plt.plot(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6)

    # function to show the plot
    plt.savefig("graph/drawIcResponseVariance.png", bbox_inches='tight')
    plt.close()