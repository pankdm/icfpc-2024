import os
import sys
from requests import post, exceptions
from dotenv import load_dotenv
from interpreter import ICFPInterpreter
from progress.bar import Bar
from time import sleep

load_dotenv()


# to be used in solvers
def send_solution(solution, name, num):
    interpreter = ICFPInterpreter()
    if name == "spaceship":
        command = "S" + interpreter.encode_string(f"solve {name}{num} {solution}")
    elif name == "3d":
        command = "S" + interpreter.encode_string(f"solve {name}{num}\n{solution}")
    else:
        command = solution
    request_size = len(command)
    print (f"  >> command size = {request_size}")
    if (request_size > 10**6):
        print ("Error: Request is too large!")
        return

    # print (f"  >> sending '{command}'")
    try:
        token = os.environ["ICFPC_TOKEN"]
        # print (token)
        resp = post(
            "https://boundvariable.space/communicate",
            data=command,
            headers={"Authorization": f"Bearer {os.environ['ICFPC_TOKEN']}"}
        )
        resp.raise_for_status()
        print (f"  >> got {resp.text}")
        encoded = interpreter.decode_string(resp.text[1:])
        print (encoded)
        return encoded
    except exceptions.RequestException as e:
        print("Error: ", e)
        return None


def read_and_send_solution(name, path, num):
    file = f"{path}/{i}.txt"
    token = os.environ["ICFPC_TOKEN"]
    print (token)
    if os.path.exists(file):
        print (f"Sending problem #{num} at {file}")
        with open(file, "r") as f:
            s = f.read()
            send_solution(s, name, num)
            # print (s)
            sleep(3.01)


# Usage: [path to solutions] [start] [end]
if __name__ == "__main__":
    assert len(sys.argv) >= 2
    path = sys.argv[1]

    parts = path.split("/")
    assert parts[0] == "solutions", "Can only send solutions from solutions/ directory"

    start = 1
    end = 100
    if len(sys.argv) >= 3:
        start = int(sys.argv[2])
        end = int(sys.argv[3])


    # automatically infer problem name
    name = parts[1]
    print (f"Sending solutions for problem '{name}'")

    for i in range(start, end + 1):
        read_and_send_solution(name, path, i)
