FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y --no-install-recommends \
    calibre \
	wget \
	unzip \
	unrar \
	vim \
	sudo \
	python3 \
	gpg

# Install firefox: https://support.mozilla.org/en-US/kb/install-firefox-linux
RUN sudo install -d -m 0755 /etc/apt/keyrings 
RUN wget -q https://packages.mozilla.org/apt/repo-signing-key.gpg -O- | sudo tee /etc/apt/keyrings/packages.mozilla.org.asc > /dev/null
RUN gpg -n -q --import --import-options import-show /etc/apt/keyrings/packages.mozilla.org.asc | awk '/pub/{getline; gsub(/^ +| +$/,""); if($0 == "35BAA0B33E9EB396F59CA838C0BA5CE6DC6315A3") print "\nThe key fingerprint matches ("$0").\n"; else print "\nVerification failed: the fingerprint ("$0") does not match the expected one.\n"}' 
RUN echo "deb [signed-by=/etc/apt/keyrings/packages.mozilla.org.asc] https://packages.mozilla.org/apt mozilla main" | sudo tee -a /etc/apt/sources.list.d/mozilla.list > /dev/null 
RUN printf "\n\
Package: *\n\
Pin: origin packages.mozilla.org\n\
Pin-Priority: 1000\n\
" | sudo tee /etc/apt/preferences.d/mozilla
RUN sudo apt-get update && sudo apt-get install -y --no-install-recommends firefox 

# Install calibre plugins
RUN wget -O temp.zip https://github.com/noDRM/DeDRM_tools/releases/download/v10.0.3/DeDRM_tools_10.0.3.zip && unzip temp.zip -d DeDRM && rm temp.zip
RUN calibre-customize --add-plugin DeDRM/DeDRM_plugin.zip
RUN calibre-customize --add-plugin DeDRM/Obok_plugin.zip

ENTRYPOINT ["/bin/bash"]
