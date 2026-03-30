FROM docker.blaizard.com/tools/ci/runner/linux:latest

RUN sudo apt-get update && sudo apt-get install -y \
	npm \
	nodejs

RUN sudo npm install -g opencode-ai@latest

COPY opencode.json /home/testuser/.config/opencode/opencode.json
