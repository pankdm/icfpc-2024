import os
import sys
from requests import post
from dotenv import load_dotenv
from interpreter import ICFPInterpreter
from progress.bar import Bar
from time import sleep

load_dotenv()
os.makedirs("problems", exist_ok=True)
interpreter = ICFPInterpreter()

def download_problem(name: str, number: int):
    command = interpreter.encode_string(f"get {name}{number}")

    if not os.path.exists(f"problems/{name}/"):
        os.mkdir(f"problems/{name}/")

    resp = post(
        "https://boundvariable.space/communicate",
        data=command,
        headers={"Authorization": f"Bearer {os.environ["ICFPC_TOKEN"]}"}
    )
    encoded = resp.text

    # Unknown instruction
    if encoded.startswith("SO.+./7.}).3425#4)/."):
        return False

    if encoded:
        with open(f"problems/{name}/raw_{name}{number}.txt", "w") as encoded_file:
            encoded_file.write(encoded)
        try:
            tokens = interpreter.tokenize(encoded)
            tree = interpreter.parse(tokens)

            with open(f"problems/{name}/{name}{number}.txt", "w") as decoded_file:
                decoded_file.write(eval(str(tree)))
        except Exception as e:
            print(f"Error decoding {e}")

        return True
    else:
        print(f"Problem {name}{number} wasn't found")

        return False


if __name__ == "__main__":
    assert len(sys.argv) == 4
    _, problem, start, upto = sys.argv
    upto = int(upto)
    start = int(start)

    bar = Bar(f"Downloading {problem}", max=(upto - start))
    for i in range(start, upto + 1):
        if download_problem(problem, i):
            bar.next()
            sleep(3.01)
        else:
            exit()
    bar.finish()
