 # Docker image build

## Prequesits
Docker install 
Example (Docker desktop for windows: https://www.docker.com/products/docker-desktop/) (needs hypervisor ensable in bios and WSL)



## Build

get binaries in ./Release/DarwinServer.exe

docker build --pull --rm -f "Dockerfile" -t darwin:latest "." 

docker build . --tag mlaagerc2c/darwin && docker run -p 45323:45323  mlaagerc2c/darwin 

## Run

docker run -p 45323:45323  mlaagerc2c/darwin

## Publish:

docker push mlaagerc2c/darwin
