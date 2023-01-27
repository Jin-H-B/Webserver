#!/usr/bin/python3
from distutils.command.upload import upload
import sys, os

content = sys.stdin.read()
content = content.split("\r\n")

# print(content)

print("<html>")
print("<body>")
print("<div><a href=\"/home\">Go to index</a></div>")

content_type = content[2]
print(content_type)
# print filename
filename = content[1].split(";")[2].split("=")[1].strip('"')
print("<h2>")
print("filename : " + filename)
print("</h2>")

# # save the file
file_content = content[4]
upload_path = str(os.environ.get("UPLOAD_PATH"))
upload_path = "/Users/jinhyeok/Desktop/42seoul/git_webserver/cpp_webserver/04_file_read/database/"

print("<h4>")
print(upload_path)
print("</h4>")
print("<div>")

upload_file = open(upload_path + filename, "wt")
upload_file.write(file_content)
upload_file.close()
print("<h1>")
print("FILE UPLOADED!!")
print("</h1>")

print("</div>")


# #!/usr/bin/python3
# import cgi, sys
# import os
# import cgitb
# cgitb.enable()

# form = cgi.FieldStorage()
# print(form)

# print("POST.PY!!")

# body = sys.stdin.read()
# print(body)

# method = os.environ['REQUEST_METHOD']
# print(method)

# form = cgi.FieldStorage()
# print(form)

# # Parse the form data
# form = cgi.FieldStorage()
# print(form)
# # Extract the uploaded files
# files = {}
# for field in form.keys():
#     # Check if the field is a file field
#     if isinstance(form[field], cgi.FieldStorage) and form[field].filename:
#         # Save the file to the specified location
#         with open(save_path + form[field].filename, 'wb') as f:
#             f.write(form[field].file.read())
#         files[field] = form[field].filename

# if len(files) > 0:
#     message = "file uploaded success"
# else:
#     message = "file uploaded failed"

# # Print the uploaded files
# response = "<html><body><center>"
# response += "<h1>{}<h1>".format(message)
# for field, filename in files.items():
#     response += "<p>{}: {}</p>\n".format(field, filename)
# response += "</center></body></html>"

# print("\r")
# print(response)

# exit()


# #!/usr/bin/python3

# # Import modules for CGI handling
# import cgi, io, sys

# # res = sys.stdin.read()
# # print(res)

# content = sys.stdin.read()
# content_list = content.split("\r\n")
# # print(content_list[0][2:])

# ret = cgi.parse_multipart(io.BytesIO(bytes(content)), {"boundary": bytes(content_list[0][2:])})
# print(ret)

# # form = cgi.FieldStorage()

# # print(form)
