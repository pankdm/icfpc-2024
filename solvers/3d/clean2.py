import sys

from uploader import send_solution
import re
from runner import parse_prog

def extract_score(input_string):
    pattern = r'(\d+)!'  # This pattern captures one or more digits followed by an exclamation mark
    match = re.search(pattern, input_string)
    if match:
        score = match.group(1)
        return int(score)
    else:
        return None  # Return None if no match is found

def parse_labels(cells):
    label_to_location = {}
    for y, row in enumerate(cells):
        for x, value in enumerate(row):
            # if "*" in value:
            #     continue
            if value in ["<", ">"]:
                continue
            if "<" in value:
                label = value.split("<")[1]   
                label_to_location[label] = (x, y)   

    print (f"labels = {label_to_location}")
    overwrites = {}
    for y, row in enumerate(cells):
        for x, value in enumerate(row):
            if "@" in value:
                if ">" not in value:
                    assert False, f"@-value '{value}' without labels!"
                label = value.split(">")[1]
                x0, y0 = label_to_location[label]

                dx = x - x0
                dy = y - y0

                overwrites[ (x - 1, y)] = str(dx)
                overwrites[ (x + 1, y)] = str(dy)
    
    return overwrites

def postprocess_value(value):
    if value in ["*", "=", "<", ">"]:
        return value
    elif "*" in value:
        return "."
    elif "<" in value:
        # note: recursive
        return postprocess_value(value.split("<")[0])
    elif ">" in value:
        # note: recursive
        return postprocess_value(value.split(">")[0])
    elif "=" in value:
        return value.split("=")[1]
    elif value == "":
        return "."
    else:
        return value


def postprocess_input(cells, overwrites):
    data = []
    for y, row in enumerate(cells):
        values = []
        for x, value in enumerate(row):            
            # Process overwrites first
            key = (x, y)
            if key in overwrites:                
                values.append(overwrites[key])
                continue
            after = postprocess_value(value)
            # print (f" '{value}' -> '{after}' ")
            values.append(after)
        # print (values)
        # max_dx = max(max_dx, len(values))
        data.append(values)
    return data


def get_solution(data):
    output = []
    for values in data:
        output.append(" ".join(values))
    return "\n".join(output)


file = sys.argv[1]


parts = file.split("/")

prefix = parts[-1].split(".")[0]
num = prefix.split("-")[0]

output_file = f"solutions/3d/organized/{prefix}.output"



data = []
with open(file, "r") as f:
    s = f.read()

    cells = []
    for row in s.split("\n"):
        values = []
        for value in row.split("\t"):
            value = value.replace(" ", "")
            values.append(value)
        cells.append(values)

    overwrites = parse_labels(cells)
    print (f"overwrites = {overwrites}")
    # print (f"cells = {cells}")
    data = postprocess_input(cells, overwrites)
    # print (f"data = {data}")


# max_dy = len(data)
# # TODO: get proper constant
# max_dt = 3
# print (f"volume: {max_dx} * {max_dy} * {max_dt} = {max_dx * max_dy * max_dt}")

print (f"writing to file {output_file}")
solution = get_solution(data)
with open(output_file, "w") as out:
    out.write(solution)

prog = parse_prog(solution)
padded_solution = prog.to_str_padded()
with open(f"{output_file}.nice", "w") as out:
    out.write(padded_solution)

print ("")
print ("[Sending the solution]")
msg = send_solution(solution, "3d", num)

if msg is not None and "Correct" in msg:
    score = extract_score(msg)

    score_output = f"solutions/3d/organized/{prefix}.score.{score}"
    print (f"Writing to file = {score_output}")
    with open(score_output, "w") as out:
        out.write(padded_solution)

