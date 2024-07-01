import os
import sys
from requests import post, exceptions
from dotenv import load_dotenv
from interpreter import ICFPInterpreter
from progress.bar import Bar
from time import sleep

load_dotenv()

interpreter = ICFPInterpreter()

def send(s):
    command = "S" + interpreter.encode_string(s)
    print (f"  >> Sending {command}")
    try:
        resp = post(
            "https://boundvariable.space/communicate",
            data=command,
            headers={"Authorization": f"Bearer {os.environ['ICFPC_TOKEN']}"}
        )
        resp.raise_for_status()
        print (f"  << got {resp.text}")
        encoded = interpreter.decode_string(resp.text[1:])
        print ()
        print (encoded)
        # sleep(3.01)
    except exceptions.RequestException as e:
        print("Error: ", e)



if sys.argv[1] == "decode":
    while True:
        s = input("Enter token to decode:\n")
        print ("Decoded: ")
        print (interpreter.decode_string(s))
if sys.argv[1] == "encode":
    while True:
        s = input("Enter token to encode:\n")
        print ("Encoded: ")
        print ("S" + interpreter.encode_string(s))
if sys.argv[1] == "repl":
    while True:
        s = input("Enter command to send:\n")
        send(s)
if sys.argv[1] == "send":
    s = sys.argv[2]
    print (s)
    send(s)
else:
    print(f"Unknown mode: {sys.argv[1]}")
