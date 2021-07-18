#!/bin/bash

# Test locally building in the container
cd "tools/ci/jenkins"
docker-compose up
docker-compose down
