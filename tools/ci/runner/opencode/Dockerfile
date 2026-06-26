FROM docker.blaizard.com/tools/ci/runner/linux:latest

RUN sudo apt-get update && sudo apt-get install -y \
	npm \
	nodejs

RUN sudo npm install -g opencode-ai@latest
RUN OPENCODE_CONFIG_CONTENT='{"$schema":"https://opencode.ai/config.json","permission":{"external_directory":"allow"}}'

# This ensure the migration of the database.
RUN opencode db path

# Add playwright for browser automation.
RUN sudo npm install -g @playwright/cli@latest
RUN playwright-cli install-browser chrome --with-deps

# Set git user for the commit.
RUN git config --global user.name "agent"
RUN git config --global user.email "agent@blaizard.com"
