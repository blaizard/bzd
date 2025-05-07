FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y --no-install-recommends \
    calibre \
	wget \
	unzip \
	vim \
	sudo \
	python3

RUN wget -O temp.zip https://github.com/noDRM/DeDRM_tools/releases/download/v10.0.3/DeDRM_tools_10.0.3.zip && unzip temp.zip -d DeDRM && rm temp.zip
RUN calibre-customize --add-plugin DeDRM/DeDRM_plugin.zip
RUN calibre-customize --add-plugin DeDRM/Obok_plugin.zip

ENTRYPOINT ["/bin/bash"]

# Remove the DRM
# calibredb add La\ via\ secrète\ des\ arbres.epub --with-library=library
# rm -rfd library/ && mkdir library

# Set the key
# vim /root/.config/calibre/plugins/dedrm.json

# Convert to PDF
# ebook-convert La\ via\ secrète\ des\ arbres.epub temp.pdf
