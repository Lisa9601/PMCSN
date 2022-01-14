def formatHour(ora):
    oraSplit = ora.split(" - ")
    primaParte = ""
    secondaParte = ""
    if (oraSplit[0][0] == "0") and (oraSplit[0][1] == "0"):
        primaParte = oraSplit[0][0:2] + ":" + oraSplit[0][2:]
    else:
        if oraSplit[0][0] != "0":
            primaParte = oraSplit[0][0:2] + ":" + oraSplit[0][2:]
        else:
            primaParte = oraSplit[0][1:2] + ":" + oraSplit[0][2:]
    if (oraSplit[1][0] == "0") and (oraSplit[1][1] == "0"):
        secondaParte = oraSplit[1][0:2] + ":" + oraSplit[1][2:]
    else:
        if oraSplit[1][0] != "0":
            secondaParte = oraSplit[1][0:2] + ":" + oraSplit[1][2:]
        else:
            secondaParte = oraSplit[1][1:2] + ":" + oraSplit[1][2:]
    hour = primaParte + " - " + secondaParte
    return hour

