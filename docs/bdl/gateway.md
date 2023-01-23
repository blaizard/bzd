# Gateway

A Gateway is a 2-way communication port owned by an executor.

```bdl
ethernet1 = EthernetGateway("192.168.0.1:3288") [executor(executor1)];
ethernet2 = EthernetGateway("192.168.0.1:3289") [executor(executor2)];
connect(ethernet1, ethernet2);
```

or

```bdl
composition esp32 {
    ethernet = EthernetGateway("192.168.0.1:3288") [executor(esp32.executor)];
}

composition x86 {
    ethernet = EthernetGateway("192.168.0.2:3288") [executor(x86.executor)];
}

composition {
    connect(esp32.ethernet, x86.ethernet);  
}
```
