from coapthon.server.coap import CoAP
from coapthon.client.helperclient import HelperClient
import time
from coapthon import defines
from coapthon.resources.resource import Resource

class BasicResource(Resource):
    def __init__(self, name="BasicResource", coap_server=None):
        super(BasicResource, self).__init__(name, coap_server, visible=True,
                                            observable=True, allow_children=True)

        self.resource_type = "rt1"
        self.content_type = "text/plain"
        self.interface_type = "if1"

    def render_PUT_advanced(self, request, response):
        host = "192.168.1.108"
        port = 5683
        path = "/uid"
        client = HelperClient(server=(host, port))
        resNode = client.put(path, request.payload)
        print(response.pretty_print())
        client.stop()
        from coapthon.messages.response import Response
        assert(isinstance(response, Response))
        response.payload = resNode.payload
        response.code = defines.Codes.CHANGED.number
        return self, response


class CoAPServer(CoAP):
    def __init__(self, host, port):
        CoAP.__init__(self, (host, port))
        self.add_resource('/uid', BasicResource())

def main():
    server = CoAPServer("0.0.0.0", 5683)
    try:
        server.listen(1000)
    except KeyboardInterrupt:
        server.close()
        print('broker has been stopped')

if __name__ == '__main__':
    main()
