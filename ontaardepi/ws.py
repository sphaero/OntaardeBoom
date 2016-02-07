from wsgiref.simple_server import make_server
from webob import static

port = 8000
httpd = make_server('', port, static.DirectoryApp('.'))
print("Serving on port {0}...".format(port))
# Serve until process is killed
httpd.serve_forever()
