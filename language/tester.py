import os
import sys
from requests import post, exceptions
from dotenv import load_dotenv
from interpreter import ICFPInterpreter, run_fast
from progress.bar import Bar
from time import sleep

load_dotenv()

interpreter = ICFPInterpreter()
file = sys.argv[1]
a = sys.argv[2]
b = sys.argv[3]

with open(file, "r") as f:
    s = f.read()

parts = file.split("/")
name = parts[1]
num = parts[-1].split(".")[0]

ins = f"test 3d {a} {b}"

print (f"sending {ins}")

command = "S" + interpreter.encode_string(f"{ins}\n{s}")

try:
    resp = post(
        "https://boundvariable.space/communicate",
        data=command,
        headers={"Authorization": f"Bearer {os.environ["ICFPC_TOKEN"]}"}
    )
    resp.raise_for_status()
    print (f"  >> got {resp.text}")
    encoded = run_fast(resp.text)
    print (encoded)
except exceptions.RequestException as e:
    print("Error: ", e)
