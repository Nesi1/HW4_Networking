import random

length = 20

for i in range(1,6,1):
    file_name = "t2/h" + str(i) + ".in"
    f = open(file_name, "w")
    string = ""
    for j in range(length):
        time = random.randrange(1,9,1)
        type = random.choice(['M', 'P', 'V'])
        string += type
        string += str(time)

    f.write(string)
    f.close