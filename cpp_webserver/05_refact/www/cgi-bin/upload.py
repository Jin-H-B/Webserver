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

# print filename
filename = content[1].split(";")[2].split("=")[1].strip('"')
print("<h2>")
print("filename : " + filename)
print("</h2>")

# # save the file
file_content = content[4]


upload_path = str(os.environ.get("UPLOAD_PATH"))
# upload_path = "/Users/jinhyeok/Desktop/42seoul/git_webserver/cpp_webserver/05_refact/database/"
print("<h4>")
print(upload_path)
print("</h4>")
print("<h4>")
# print(file_content)

print("</h4>")
print("<div>")
if (content_type.split(': ')[1] == "text/plain"):
	upload_file = open(upload_path + filename, "w")
	upload_file.write(file_content)
	upload_file.close()
	print("<h1>")
	print("FILE UPLOADED!!")
	print("</h1>")
elif content_type.split(': ')[1] == "image/jpeg":
	with open(upload_path + filename, "w") as f:
		print(file_content)
		print("\n -> end of cotent")
		f.write(bytes(file_content, 'utf-8'))
		# f.write(file_content)

	print("<h1>")
	print("FILE UPLOADED!!")
	print("</h1>")

else:
    print("<h1>")
    print("ONLY TEXT UPLOAD")
    print("</h1>")
print("</div>")
# "r" - Read - Default value. Opens a file for reading, error if the file does not exist
# "a" - Append - Opens a file for appending, creates the file if it does not exist
# "w" - Write - Opens a file for writing, creates the file if it does not exist
# "x" - Create - Creates the specified file, returns an error if the file exist
# "t" - Text - Default value. Text mode
# "b" - Binary - Binary mode (e.g. images)


# print contents
file_contents = content[4].split('\n')
for line in file_contents:
    print("<div>")
    print("%s" %line)
    print("</div>")
print("</body>")
print("</html>")


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
