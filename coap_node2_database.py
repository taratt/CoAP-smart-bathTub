from coapthon.server.coap import CoAP
from coapthon.client.helperclient import HelperClient
import time
from coapthon import defines
from coapthon.resources.resource import Resource

database = { "9A439680": (100, 40),"63BEB903":(20,15)}
class BasicResource(Resource):
    def __init__(self, name="BasicResource", coap_server=None):
        super(BasicResource, self).__init__(name, coap_server, visible=True,
                                            observable=True, allow_children=True)

        self.resource_type = "rt1"
        self.content_type = "text/plain"
        self.interface_type = "if1"

    def render_GET(self, request):
        self.payload = '12m67'
        return self

    def render_PUT_advanced(self, request, response):
        # self.payload = request.payload
        from coapthon.messages.response import Response
        assert(isinstance(response, Response))
        response.payload = str(database[request.payload][0])+'m'+str(database[request.payload][1])
        response.code = defines.Codes.CHANGED.number
        return self, response


class CoAPServer(CoAP):
    def __init__(self, host, port):
        CoAP.__init__(self, (host, port))
        self.add_resource('/uid', BasicResource())

def main():
    server = CoAPServer("0.0.0.0", 5684)
    try:
        server.listen(1000)
    except KeyboardInterrupt:
        server.close()
        print('node2 has been stopped')

if __name__ == '__main__':
    main()
