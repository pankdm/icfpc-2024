from functools import reduce
import sys

FROM_CHARS = "".join(chr(x) for x in range(33, 127))
TO_CHARS = """abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!"#$%&'()*+,-./:;<=>?@[\\]^_`|~ \n"""
DECODE = str.maketrans(FROM_CHARS, TO_CHARS)


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


def decode_int(body: str) -> int:
    return reduce(lambda x, y: x * 94 + (ord(y) - 33), body, 0)


def decode_string(body: str) -> str:
    return body.translate(DECODE)

if __name__ == "__main__":
    match sys.argv[1]:
        case "decode":
            assert len(sys.argv) > 2
            print(decode_token(sys.argv[2]))
        case _:
            print("Unknown command")
