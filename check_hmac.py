import hashlib
import hmac
import sys

from argparse import ArgumentParser

p = ArgumentParser(description="Compute a hmac using a given secret")
p.add_argument("-a", "--algo", default="sha1", help="Algorithm to use")
p.add_argument("-s", "--secret", default="", help="Secret to use")
p.add_argument("files", metavar="FILE", nargs="*",
               help="Files to process, or stdin if ommitted")
args = p.parse_args()
if not args.files:
    args.files += "-",

for x in args.files:
    if x == "-":
        ip = sys.stdin
    else:
        ip = open(x, "rb")
    with ip:
        h = hmac.HMAC(args.secret, digestmod=getattr(hashlib, args.algo))
        while True:
            b = ip.read(1 << 17)
            if not b:
                break
            h.update(b)
        print "{}  ({}) {}".format(h.hexdigest(), args.secret, x)
