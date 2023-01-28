#!/usr/bin/python3
import sys, os
import shutil, base64


content = sys.stdin.read()
content = content.split("\r\n")

# print(content)

print("<html>")
print("<body>")
print("<div><a href=\"/home\">Go to index</a></div>")
print(content)

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
if (content_type.split(': ')[1] != "image/jpeg"):
	upload_file = open(upload_path + filename, "w", encoding='utf-8')
	upload_file.write(file_content)
	upload_file.close()
	print("<h1>")
	print("FILE UPLOADED!!")
	print("</h1>")
# elif content_type.split(': ')[1] == "image/jpeg":
#     # Encode the binary data as a base64 string
#     encoded_file_content = base64.b64encode(file_content)
#     # Write the encoded string to the file
#     with open(upload_path + filename, "wb") as f:
#         f.write(base64.b64decode(encoded_file_content))
        # print(base64.b64decode(encoded_file_content).decode())
elif content_type.split(': ')[1] == "image/jpeg":
	with open(upload_path + filename, "wb") as f:
		binary_data = base64.b64decode(file_content)
		f.write(binary_data)

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
