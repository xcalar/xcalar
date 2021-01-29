import urllib.parse

from xcalar.container.connectors.webhdfs import WebHDFSConnector
import xcalar.container.target.base as target


@target.register(
    name="HttpFs with Kerberos",
    is_available=WebHDFSConnector.is_available(kerberos=True))
@target.param("httpfsnodes", "Semi-colon delimited list of httpfs nodes")
@target.param("keytab", "<user@realm>|<Path to keytab file>", optional=True)
@target.param(
    "config_path", "File path to kerberos config file", optional=True)
class HttpfsKerberosTarget(target.BaseTarget):
    """
    Connect to a Kerberized HDFS cluster via HttpFs.

    A keytab can be provided. Otherwise, we assume that the xcalar nodes have
    the necessary kerberos tokens already initialized. To create a keytab,
    refer to https://kb.iu.edu/d/aumh.

    Httpfs node to be specified in the form <FQDN of httpfs node>:<port number>.
    <port number> if omitted, defaults to 14000.

    Multiple httpfs nodes can be specified with semi-colon delimiter. E.g.

    http://httpfsnode1.example.com;http://httpfsnode2.example.com:14001;http://httpfsnode3.example.com;...

    You may use https instead of http as well. The default is http if
    protocol is not provided.
    """

    def __init__(self,
                 name,
                 path,
                 httpfsnodes,
                 keytab=None,
                 config_path=None,
                 **kwargs):
        super(HttpfsKerberosTarget, self).__init__(name)

        nodes = []
        for node in httpfsnodes.split(";"):
            parsed = urllib.parse.urlparse(node)
            protocol = "http" if parsed.scheme == "" else parsed.scheme
            hostname = parsed.netloc if parsed.netloc != "" else parsed.path
            if hostname == "":
                continue
            tmp = hostname.split(":")
            if (len(tmp) > 1):
                fqdn = tmp[0]
                port = int(tmp[1])
            else:
                fqdn = hostname
                port = 14000

            nodes.append("%s://%s:%d" % (protocol, fqdn, port))

        nn = ";".join(nodes)
        self.connector = WebHDFSConnector(
            nn,
            kerberos=True,
            keytab=keytab,
            target_name=self.name(),
            config_path=config_path)

    def is_global(self):
        return True

    def get_files(self, path, name_pattern, recursive, **user_args):
        return self.connector.get_files(path, name_pattern, recursive)

    def open(self, path, opts):
        return self.connector.open(path, opts)
