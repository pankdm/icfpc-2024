from functools import reduce
import sys

OFFSET = 33
BASE = 94
FROM_CHARS = "".join(chr(x) for x in range(OFFSET, OFFSET + BASE))
TO_CHARS = """abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!"#$%&'()*+,-./:;<=>?@[\\]^_`|~ \n"""
DECODE = str.maketrans(FROM_CHARS, TO_CHARS)
ENCODE = str.maketrans(TO_CHARS, FROM_CHARS)

def decode_token(source: str) -> object:
    indicator, body = source[0], source[1:]
    match indicator:
        case "T":
            return True
        case "F":
            return False
        case "I":
            return decode_int(body)
        case "S":
            return decode_string(body)
        case _:
            print("Unknown token type " + indicator + " body \"" + body + "\"")


def encode_token(token: object) -> str:
    match token:
        case bool():
            return "FT"[token]
        case int():
            return encode_int(token)
        case str():
            return encode_string(token)
        case _:
            raise NotImplementedError(f"Unknown type {type(token)}")

def decode_int(body: str) -> int:
    return reduce(lambda x, y: x * BASE + (ord(y) - OFFSET), body, 0)

def encode_int(value: int) -> str:
    out = []
    while value:
        out.append(chr(value % BASE + OFFSET))
        value //= BASE
    out.append("I")
    return "".join(out[::-1])

def decode_string(body: str) -> str:
    return body.translate(DECODE)

def encode_string(value: str) -> str:
    return "S" + value.translate(ENCODE)

if __name__ == "__main__":
    match sys.argv[1]:
        case "decode":
            assert len(sys.argv) > 2
            print(decode_token(sys.argv[2]))
        case "encode":
            assert len(sys.argv) > 2
            print(encode_token(sys.argv[2]))
        case _:
            print("Unknown command")
