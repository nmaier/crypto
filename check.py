import hashlib
import sys

for x in sys.argv[2:]:
    with open(x, "rb") as ip:
        h = hashlib.new(sys.argv[1])
        h.update(ip.read())
        print h.hexdigest(), " " + x
