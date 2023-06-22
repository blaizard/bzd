FROM alpine:latest

RUN apk update
RUN apk add --no-cache php php-json bash wget

RUN mkdir /var/www
RUN cd /var/www/ && wget -O - https://raw.githubusercontent.com/vavrecan/keeweb-local-server/master/install.sh | bash

EXPOSE 8080
WORKDIR /var/www/keeweb-local-server
ENTRYPOINT ["php", "-S", "0.0.0.0:8080"]
