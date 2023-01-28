#!/usr/bin/python3

import cgi, cgitb , sys
import sys, os


origin = sys.stdin.read()

# # Create instance of FieldStorage
# form = cgi.FieldStorage()


print("<html>")
print("<body>")
# print(origin)
content = origin.split("\r\n")
# print(content)
filename = content[3]
print(filename)
print("<h1>")
print("</h1>")
print("</body>")
print("</html>")


# #!/usr/bin/python3
# import sys, os

# origin = sys.stdin.read()
# content = origin.split("\r\n")
# filename = content[1].split(";")[2].split("=")[1].strip('"')

# upload_path = str(os.environ.get("UPLOAD_PATH"))
# # os.unlink(upload_path + filename)

# print("<html>")
# print("<body>")
# print(filename)
# print("hello")
# print("<h1>")
# print("delete")
# print("</h1>")
# print("</body>")
# print("</html>")
