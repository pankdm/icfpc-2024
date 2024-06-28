

from parser import decode_token, encode_token
import sys

import os
import sys
from requests import post, exceptions
from dotenv import load_dotenv
from interpreter import ICFPInterpreter
from parser import decode_token, encode_token
from progress.bar import Bar
from time import sleep

load_dotenv()


if sys.argv[1] == "decode":
    while True:
        s = input("Enter token to decode:\n")
        print ("Decoded: ")
        print (decode_token(s))
if sys.argv[1] == "encode":
    while True:
        s = input("Enter token to encode:\n")
        print ("Encoded: ")
        print (encode_token(s))
if sys.argv[1] == "repl":
    while True:
        s = input("Enter command to send:\n")
        command = encode_token(s)
        print ("Encoded: ")
        print (f" >> {command}")
        try:
            resp = post(
                "https://boundvariable.space/communicate",
                data=command,
                headers={"Authorization": f"Bearer {os.environ["ICFPC_TOKEN"]}"}
            )
            resp.raise_for_status()
            print (f"  >> got {resp.text}")
            encoded = decode_token(resp.text)
            print (encoded)
            # sleep(3.01)
        except exceptions.RequestException as e:
            print("Error: ", e)



print(f"Unknown mode: {sys.argv[1]}")
