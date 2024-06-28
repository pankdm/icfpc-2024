import os
import sys
from requests import post
from dotenv import load_dotenv
from parser import decode_token, encode_token
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
            tokens = [str(decode_token(token)) for token in encoded.split()]
        except:
            tokens = []

        with open(f"problems/{name}{number}_decoded.txt", "w") as decoded_file:
            decoded_file.writelines(tokens)

        return True
    else:
        print(f"Problem {name}{number} wasn't found")

        return False


if __name__ == "__main__":
    assert len(sys.argv) == 3
    _, problem, upto = sys.argv

    for i in range(1, int(upto) + 1):
        if download_problem(problem, i):
            sleep(3.01)
        else:
            exit()
