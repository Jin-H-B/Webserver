#!/usr/bin/python3

# Import modules for CGI handling
import cgi, cgitb , sys, time

# res = sys.stdin.read()

# print(res)

# Create instance of FieldStorage
form = cgi.FieldStorage()
print("FORM", form)
# # Get data from fields

first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')

# print(form)

print ("<html>")
print ("<head>")
print ("</head>")
print ("<body>")
print(form)
print ("<h2>Test Form (POST) ✨ Result ✨</h2>")
print ("<h3>%s's coalition is %s</h3>" %(first_name, last_name))
print ("</body>")
print ("</html>")
