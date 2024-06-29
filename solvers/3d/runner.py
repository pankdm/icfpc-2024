

# returns a map for the initial state
def read_prog(file):
    with open(file, "r") as f:
        s = f.read()
        return {}


if __name__ == "__main__":
    file = sys.argv[1]
    prog = read_prog(file)
    print (prog)