#!/usr/bin/python3
import sys, os

content = sys.stdin.read()
content = content.split("\r\n")

# print(content)
#boundary is content[0]
#content[1] is
# print(content[1])
print("<html>")
print("<body>")
print(content[4])
print("</body>")
print("</html>")
# script = str(os.environ.get("PATH_TRANSLATED"))
# print(script)
