from webdav3.client import Client

options = {
    "webdav_hostname": "http://localhost:8081/x/test/webdav",
    "webdav_login": "login",
    "webdav_password": "password"
}
client = Client(options)
client.verify = False  # To not check SSL certificates (Default = True)
client.execute_request("mkdir", 'directory_name')
