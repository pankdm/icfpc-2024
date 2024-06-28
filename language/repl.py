

from parser import decode_token, encode_token
import sys


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

print(f"Unknown mode: {sys.argv[1]}")
