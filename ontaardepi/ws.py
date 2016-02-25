from wsgiref.simple_server import make_server
from webob import static, exc, Request, Response
import subprocess
import os

class Appie(object):
    """
    Basic class for providing a RESTful interface to resources.

    Simply register an AppieRestObject inherited class object

    GET = Retrieve
    PUT = Replace
    POST = New/Create
    DELETE = d'uh
    """
    def __init__(self, *args, **kwargs):
        # empty dict of restObjects
        self._restObjects = {}
        self._default = kwargs.pop('default', exc.HTTPNotFound('No such resource'))
        self._inject_headers = None
        for name in kwargs:
            self.register_rest_object(name, kwargs[name])

    def __call__(self, environ, start_response):
        req = Request(environ)
        try:
            resp = self._process(req)
        except ValueError as e:
            resp = exc.HTTPBadRequest(str(e))
        except exc.HTTPException as e:
            resp = e
        return resp(environ, start_response)

    def register_rest_object(self, name, obj):
        print('Appie: registering url: /%s' %name)
        self._restObjects.update({name : obj})

    def unregister_rest_object(self, name):
        print('Appie: unregistering url: /%s' %name)
        try:
            del self._restObjects[name]
        except KeyError as ke:
            print("No such object: %s" %ke)

    def inject_headers(self, headers):
        self._inject_headers = headers

    def _process(self, req):
        """
        a request URL is parsed as follow
        /object/resource?args
        object is the main python object
        GET /object is mapped to handle_get function(**kwargs)
        GET /object/resource is mapped to handle_get([resource,], **kwargs)
        GET /object/resource/resource2 is mapped to handle_get([resource,resource2], **kwargs)
        args are always passed on through **kwargs
        """
        # GET, POST, PUT or DELETE
        method = req.method
        object = req.path_info_peek()

        if object in self._restObjects.keys():
            #remove the object from the request path
            req.path_info_pop()
            try:
                resp = req.get_response(self._restObjects[object], req)
            except Exception as e:
                #print(e)
                return exc.HTTPNotFound('No such resource: %s. err: %s' %(object, e))
        else:
            resp = req.get_response(self._default, req)
        # special for Roderick
        if (self._inject_headers):
            resp.headers.update(self._inject_headers)
        return resp

class AppieRestObject(object):
    """
    template class for a simple REST object for Appie

    Inherit this class and override the methods you need
    """
    def __init__(self, *args, **kwargs):
        pass

    def __call__(self, environ, start_response):
        req = Request(environ)
        method = req.method
        try:
            handle_method = getattr(self, 'handle_'+method)
        except:
            raise exc.HTTPInternalServerError('No %s method on resource: %s' %(method,object))
        resp = handle_method(req)
        return resp(environ, start_response)

    def handle_GET(self, req, *args, **kwargs):
        return exc.HTTPMethodNotAllowed()

    def handle_POST(self, req, *args, **kwargs):
        return exc.HTTPMethodNotAllowed()

    def handle_PUT(self, req, *args, **kwargs):
        return exc.HTTPMethodNotAllowed()

    def handle_DELETE(self, req, *args, **kwargs):
        return exc.HTTPMethodNotAllowed()


class AppieSimpleFileServer(AppieRestObject):

    def __init__(self, root_dir="./", enable_del=False, *args, **kwargs):
        self.root_dir = root_dir
        super(AppieRestObject, self).__init__(*args, **kwargs)

    def handle_GET(self, req, *args, **kwargs):
        return static.DirectoryApp(self.root_dir)

    def handle_POST(self, req, *args, **kwargs):
        # uploading new files
        filename = req.path_info_pop()
        filepath = os.path.join(os.path.join(self._root_dir, filename))
        # if the file exists we raise an exception
        if filename == "":
            filename = req.params['file']
        if os.path.exists(filepath):
            return exc.HTTPForbidden("%s already exists" %filename)
        with open(os.path.join(self._root_dir, filename), 'wb') as saveFile:
            saveFile.write(self.req.body_file.read())
        saveFile.close()
        return exc.HTTPMethodAccepted("File uploaded")

    def handle_PUT(self, *args, **kwargs):
        if not enable_del:
            return exc.HTTPForbidden("%s permission denied" %filename)
        # This will overwrite files
        # uploading files to update
        filename = self.req.path_info_pop()
        filepath = os.path.join(os.path.join(self._root_dir, filename))
        # if the file doesn't exist we raise an exception
        if not os.path.exists(filepath):
            raise exc.HTTPNotFound("%s file not found" %filename)
        if filename == "":
            filename = req.params['file']
        with open(os.path.join(self._httpdRoot, filename), 'wb') as saveFile:
            saveFile.write(req.body_file.read())
        saveFile.close()
        return exc.HTTPMethodAccepted("File updated")

    def handle_DELETE(self, *args, **kwargs):
        if not enable_del:
            return exc.HTTPForbidden("%s permission denied" %filename)
        # This will overwrite files
        # uploading files to update
        filename = self.req.path_info_pop()
        filepath = os.path.join(os.path.join(self._root_dir, filename))
        # if the file doesn't exist we raise an exception
        if not os.path.exists(filepath):
            raise exc.HTTPNotFound("%s file not found" %filename)
        if filename == "":
            filename = req.params['file']
        with open(os.path.join(self._httpdRoot, filename), 'wb') as saveFile:
            saveFile.write(req.body_file.read())
        saveFile.close()
        return exc.HTTPMethodAccepted("File updated")


class AppieBoom(AppieRestObject):
    def handle_GET(self, req, *args, **kwargs):
        path = req.path_info_pop()
        print(path)
        if path == "rebuild":
            # do rebuild
            #ret = os.system("./opi.sh rebuild")
            ret = subprocess.check_output(["./opi.sh", "rebuild"])
            #print(ret)
            return Response(ret.decode('utf-8'), content_type="text/html")
            #return exc.HTTPOk(ret.decode('utf-8').replace("\n","<br/>"))
            #call("./opi.sh", "rebuild")
        elif path == "reset":
            # do reset
            ret = os.system("./opi.sh restart")
            if ret == 0:
                return exc.HTTPOk("command succeeded")
            else:
                return exc.HTTPOk("Some error was found: code {0}".format(ret))


AppieFS = AppieSimpleFileServer(".", enable_del=True)
appie_inst = Appie(default=AppieFS)
appie_inst.register_rest_object("static", static.DirectoryApp("."))
appie_inst.register_rest_object("boom", AppieBoom())
port = 8000
httpd = make_server('', port, appie_inst)
print("Serving on port {0}...".format(port))
# Serve until process is killed
httpd.serve_forever()
