#!/usr/local/bin/python3
import cgi
import cgitb
import os
import time
import sys
cgitb.enable()

method = os.environ['REQUEST_METHOD']
if method == "GET":
    print("Content-Type: text/html; charset=utf-8", flush=True)
    print()
    html = """
    <form action="/form.py" method="post" enctype="multipart/form-data">
        <p><input type="text" name="text">
        <p><input type="file" name="file1">
        <p><button type="submit">Submit</button>
    </form>
    """
    print(html)
elif method == "POST":
    print("Content-Type: text/html; charset=utf-8", flush=True)
    print()
    # print(sys.stdin.read())
    form = cgi.FieldStorage()
    file = open("./test.png", "wb")
    file.write(form.getvalue("file1"))


# #!/usr/bin/python
# import cgi, sys
# import os
# import cgitb
# cgitb.enable()
# # import cgitb; cgitb.enable(display=0, logdir="./log")

# body = sys.stdin.read()
# print(body)



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
