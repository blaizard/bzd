FROM docker.blaizard.com/tools/ci/runner/linux:latest

RUN sudo apt-get update && sudo apt-get install -y \
	npm \
	nodejs

RUN sudo npm install -g opencode-ai@latest

# This ensure the migration of the database.
RUN opencode db path

# Set git user for the commit.
RUN git config --global user.name "agent"
RUN git config --global user.email "agent@blaizard.com"
