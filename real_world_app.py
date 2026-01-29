from http.server import HTTPServer, BaseHTTPRequestHandler
import sys

class SimpleHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(b"<h1>Gatekeeper Integration Active</h1>")
        self.wfile.write(b"<p>This request was just sniffed by the dashboard!</p>")

def run_server(port=8080):
    server_address = ('', port)
    httpd = HTTPServer(server_address, SimpleHandler)
    print(f"🌐 Real-World App running at http://localhost:{port}")
    print(f"📡 Start the Gatekeeper Sniffer on port {port} to monitor traffic...")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n🛑 Server stopped.")

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8080
    run_server(port)
