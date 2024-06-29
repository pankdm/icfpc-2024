import sys


file = sys.argv[1]


parts = file.split("/")

num = parts[-1].split(".")[0].split("-")[0]

output_file = f"solutions/3d/manual/{num}.txt"


max_dx = 0

data = []
with open(file, "r") as f:
    s = f.read()
    for row in s.split("\n"):
        values = []
        for value in row.split("\t"):
            if value in ["*", "="]:
                values.append(value)
                continue
            elif "acc" in value or "*" in value:
                values.append(".")
                continue
            elif "=" in value:
                values.append(value.split("=")[1])
            elif value == "":
                values.append(".")
            else:
                values.append(value)
        # print (values)
        max_dx = max(max_dx, len(values))
        data.append(" ".join(values))

max_dy = len(data)
max_dt = 3
print (f"volume: {max_dx} * {max_dy} * {max_dt} = {max_dx * max_dy * max_dt}")

print (f"writing to file {output_file}")
with open(output_file, "w") as out:
    out.write("\n".join(data))
