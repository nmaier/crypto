from collections import deque

ops = deque("eabcd")

for x in range(0, 80):
    if not x % 10:
        print "  // Round {}".format(x)

    if x >= 16:
        print "  w{:02} = rol((w{:02} ^ w{:02} ^ w{:02} ^ w{:02}), 1),".format(x & 0xf, (x - 3) & 0xf, (x - 8) & 0xf, (x - 14) & 0xf, (x - 16) & 0xf),

    print "  r{}({}, w{:02});".format((x // 20) + 1, ", ".join(ops), x & 0xf)
    ops.rotate(1)

    if not (x + 1) % 10:
        print
