import sys


file = sys.argv[1]


with open(file, "r") as f:
    s = f.read()
    for row in s.split("\n"):
        values = []
        for value in row.split("\t"):
            if "acc" in value or "*" in value:
                values.append(".")
                continue
            if value == "":
                values.append(".")
            else:
                values.append(value)
        # print (values)
        print (" ".join(values))
        

