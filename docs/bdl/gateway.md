# Gateway

A Gateway is a 2-way communication port owned by an executor.

```bdl
// Target specific composition.
composition esp32 {
    ip = String;
    ethernet = EthernetGateway(ip);
}
composition x86 {
    ip = String;
    ethernet = EthernetGateway(ip);
}

// System level composition.
composition {
    esp32.ip = "192.168.1.3" [override];
    x86.ip = "192.168.1.4" [override];
    connect(esp32.ethernet, x86.ethernet);  
}
```
