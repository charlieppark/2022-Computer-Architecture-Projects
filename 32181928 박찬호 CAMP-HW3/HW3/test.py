memory = list()
register = list(0 for i in range(0,32))
register[31] = 0xffffffff
register[29] = 0x1000000

with open("./test_prog/simple2.bin", "rb") as f:
    data = f.read(4)
    while data != b"":
        memory.append(data)
        data = f.read(4)

