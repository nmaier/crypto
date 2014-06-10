import hashlib
import hmac
import sys

print sys.argv[1], hashlib.new(sys.argv[1]).digest_size
