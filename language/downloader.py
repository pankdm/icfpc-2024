import os
import sys
from requests import post
from dotenv import load_dotenv
from interpreter import ICFPInterpreter
from parser import decode_token, encode_token
from progress.bar import Bar
from time import sleep

load_dotenv()
os.makedirs("problems", exist_ok=True)

def download_problem(name: str, number: int):
    command = encode_token(f"get {name}{number}")

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
        with open(f"problems/{name}{number}_encoded.txt", "w") as encoded_file:
            encoded_file.write(encoded)
        try:
            interpreter = ICFPInterpreter()
            tokens = interpreter.tokenize(encoded)
            tree = interpreter.parse(tokens)

            with open(f"problems/{name}{number}_decoded.txt", "w") as decoded_file:
                decoded_file.write(str(tree))
        except Exception as e:
            print(f"Error decoding {e}")

        return True
    else:
        print(f"Problem {name}{number} wasn't found")

        return False


if __name__ == "__main__":
    assert len(sys.argv) == 3
    _, problem, upto = sys.argv
    upto = int(upto)

    bar = Bar(f"Downloading {problem}", max=upto)
    for i in range(1, upto + 1):
        if download_problem(problem, i):
            bar.next()
            sleep(3.01)
        else:
            exit()
    bar.finish()
