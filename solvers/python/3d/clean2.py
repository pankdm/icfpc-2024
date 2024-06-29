import sys

from uploader import send_solution


file = sys.argv[1]


parts = file.split("/")

prefix = parts[-1].split(".")[0]
num = prefix.split("-")[0]

output_file = f"solutions/3d/organized/{prefix}.output"


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
            elif "*" in value:
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
# TODO: get proper constant
max_dt = 3
print (f"volume: {max_dx} * {max_dy} * {max_dt} = {max_dx * max_dy * max_dt}")

print (f"writing to file {output_file}")
solution = "\n".join(data)
with open(output_file, "w") as out:
    out.write(solution)


print ("")
print ("[Sending the solution]")
send_solution(solution, "3d", num)